/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "gatt_svr.h"
#include "hid_func.h"

static const char *tag = "NimBLEKBD_GATT_SVR";

int
gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
                   void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

int
hid_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg)
{
    uint16_t uuid16 = ble_uuid_u16(ctxt->chr->uuid);
    int rc;

    ESP_LOGI("", "%s: UUID %04X attr %04X arg %d op %d", __FUNCTION__,
        uuid16, attr_handle, (int)arg, ctxt->op);

    switch (uuid16) {

    case GATT_UUID_HID_INFORMATION:
        if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
            ESP_LOGI(tag, "invalid op %d", ctxt->op);
            break;
        }
        rc = os_mbuf_append(ctxt->om, HidInfo,
                            HID_INFORMATION_LEN);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

    case GATT_UUID_HID_CONTROL_POINT: {
        if (ctxt->op != BLE_GATT_ACCESS_OP_WRITE_CHR) {
            ESP_LOGI(tag, "invalid op %d", ctxt->op);
            break;
        }

        uint8_t new_suspend_state;

        rc = gatt_svr_chr_write(ctxt->om, 1, 1, &new_suspend_state, NULL);
        if (!rc) {
            bool old_state = hid_set_suspend((bool) new_suspend_state);

            ESP_LOGI(tag, "HID_CONTROL_POINT received new suspend state: %d, old state is: %d",
                (int)new_suspend_state, (int)old_state);
        }
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    case GATT_UUID_HID_REPORT_MAP: {
        if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) {
            ESP_LOGI(tag, "invalid op %d", ctxt->op);
            break;
        }

        rc = os_mbuf_append(ctxt->om, Hid_report_map, Hid_report_map_size);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    case GATT_UUID_EXT_RPT_REF_DESCR: {
        if (ctxt->op != BLE_GATT_ACCESS_OP_READ_DSC) {
            ESP_LOGI(tag, "invalid op %d", ctxt->op);
            break;
        }

        rc = os_mbuf_append(ctxt->om, (uint8_t *)&HidExtReportRefDesc,
            sizeof(HidExtReportRefDesc));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    case GATT_UUID_HID_PROTO_MODE: {

        if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {

            rc = os_mbuf_append(ctxt->om, &HidProtocolMode,
                                sizeof(HidProtocolMode));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        } else if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {

            uint8_t new_protocol_mode;

            rc = gatt_svr_chr_write(ctxt->om, 1, sizeof(new_protocol_mode),
                &new_protocol_mode, NULL);
            if (!rc) {
                HidProtocolMode = new_protocol_mode;
                // send true if new mode is boot mode, else guess
                hid_set_report_mode(HidProtocolMode == HID_PROTOCOL_MODE_BOOT);

                ESP_LOGI(tag, "Received new protocol mode: %d",
                    (int)new_protocol_mode);
            }

            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        } else {
            ESP_LOGI(tag, "invalid op %d", ctxt->op);
        }
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    default:
        ESP_LOGI(tag, "invalid UUID %02X", uuid16);
        break;
    }
    return BLE_ATT_ERR_UNLIKELY;
}

/* Report access function for all reports */

int
ble_svc_report_access(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg)
{
    uint16_t uuid16 = ble_uuid_u16(ctxt->chr->uuid);
    int handle_num = (int) arg;
    int rc = BLE_ATT_ERR_UNLIKELY;

    ESP_LOGI("", "%s: UUID %04X attr %04X arg %d op %d",
         __FUNCTION__, uuid16, attr_handle, (int)arg, ctxt->op);


    do {
        // Report reference descriptors
        if (uuid16 == GATT_UUID_RPT_REF_DESCR) {
            if (ctxt->op != BLE_GATT_ACCESS_OP_READ_DSC) {
                ESP_LOGI(tag, "invalid op %d", ctxt->op);
                break;
            }
            int rpt_ind = -1;

            for (int i = 0; i < Hid_report_ref_data_count; ++i) {
                if ((int)Hid_report_ref_data[i].id == handle_num) {
                    rpt_ind = i;
                    break;
                }
            }
            if (rpt_ind != -1) {
                rc = os_mbuf_append(ctxt->om,
                    (uint8_t *)Hid_report_ref_data[rpt_ind].hidReportRef,
                    HID_REPORT_REF_LEN);
                if (rc) {
                    rc = BLE_ATT_ERR_INSUFFICIENT_RES;
                }
            } else {
                rc = BLE_ATT_ERR_UNLIKELY;
            }

            break;
        }

        /* reports read */
        if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR && (
                (uuid16 == GATT_UUID_HID_REPORT)            ||
                (uuid16 == GATT_UUID_HID_BT_MOUSE_INPUT)    ||
                (uuid16 == GATT_UUID_HID_BT_KB_INPUT)       ||
                (uuid16 == GATT_UUID_HID_BT_KB_OUTPUT)      )) {
            rc = hid_read_buffer(ctxt->om, handle_num);
            if (rc) {
                rc = BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            break;
        }

        // keyboard out report (leds in/out)
        if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
            switch (handle_num) {
                case HANDLE_HID_KB_OUT_REPORT:
                case HANDLE_HID_FEATURE_REPORT:
                    rc = hid_write_buffer(ctxt->om, handle_num);
                    if (rc) {
                        rc = BLE_ATT_ERR_INSUFFICIENT_RES;
                    }
                    break;
                default:
                    rc = BLE_ATT_ERR_INSUFFICIENT_RES;
            }
            break;
        }

    } while (0);

    return rc;
}

/**
 * BAS access function
 */
int
ble_svc_battery_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt,
                          void *arg)
{
    uint16_t uuid16 = ble_uuid_u16(ctxt->chr->uuid);
    int rc = 0;

    ESP_LOGI("", "%s: UUID %04X attr %04X arg %d op %d",
         __FUNCTION__, uuid16, attr_handle, (int) arg, ctxt->op);

    switch (uuid16) {
        case BLE_SVC_BAS_CHR_UUID16_BATTERY_LEVEL:
            if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
                rc = hid_read_buffer(ctxt->om, (int) arg);
                // rc = hid_battery_level_get(ctxt->om);
                if (rc) {
                    ESP_LOGW(tag, "Error reading battery buffer, rc = %d", rc);
                    rc = BLE_ATT_ERR_INSUFFICIENT_RES;
                }
            } else {
                rc = BLE_ATT_ERR_UNLIKELY;
            }
            break;

        case GATT_UUID_BAT_PRESENT_DESCR:
            if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
                ESP_LOGI(tag, "battery character presentation descriptor read, op: %d", ctxt->op);
                rc = os_mbuf_append(ctxt->om, &Battery_level_units,
                                    sizeof Battery_level_units);

                if (rc) {
                    rc = BLE_ATT_ERR_INSUFFICIENT_RES;
                }
            }
            break;

        default:
            rc = BLE_ATT_ERR_UNLIKELY;
    }

    return rc;
}

/**
 * Simple read access callback for the device information service
 * characteristic.
 */
int
ble_svc_dis_access(uint16_t conn_handle, uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid    = ble_uuid_u16(ctxt->chr->uuid);
    const char *info = NULL;
    int rc = 0;
    int data_len = 0;

    ESP_LOGI("", "%s: UUID %04X attr %04X arg %d op %d",
         __FUNCTION__, uuid, attr_handle, (int)arg, ctxt->op);

    switch(uuid) {
    case BLE_SVC_DIS_CHR_UUID16_MODEL_NUMBER:
        info = Hid_dis_data.model_number;
        if (info == NULL) {
            info = (BLE_SVC_DIS_MODEL_NUMBER_DEFAULT);
        }
        break;
    case BLE_SVC_DIS_CHR_UUID16_SERIAL_NUMBER:
        info = Hid_dis_data.serial_number;
        if (info == NULL) {
            info = (BLE_SVC_DIS_SERIAL_NUMBER_DEFAULT);
        }
        break;
    case BLE_SVC_DIS_CHR_UUID16_FIRMWARE_REVISION:
        info = Hid_dis_data.firmware_revision;
        if (info == NULL) {
            info = (BLE_SVC_DIS_FIRMWARE_REVISION_DEFAULT);
        }
        break;
    case BLE_SVC_DIS_CHR_UUID16_HARDWARE_REVISION:
        info = Hid_dis_data.hardware_revision;
        if (info == NULL) {
            info = (BLE_SVC_DIS_HARDWARE_REVISION_DEFAULT);
        }
        break;
    case BLE_SVC_DIS_CHR_UUID16_SOFTWARE_REVISION:
        info = Hid_dis_data.software_revision;
        if (info == NULL) {
            info = (BLE_SVC_DIS_SOFTWARE_REVISION_DEFAULT);
        }
        break;
    case BLE_SVC_DIS_CHR_UUID16_MANUFACTURER_NAME:
        info = Hid_dis_data.manufacturer_name;
        if (info == NULL) {
            info = (BLE_SVC_DIS_MANUFACTURER_NAME_DEFAULT);
        }
        break;
    case BLE_SVC_DIS_CHR_UUID16_SYSTEM_ID:
        info = Hid_dis_data.system_id;
        if (info == NULL) {
            info = (BLE_SVC_DIS_SYSTEM_ID_DEFAULT);
        }
        break;
    case BLE_SVC_DIS_CHR_UUID16_PNP_INFO:
        info = (char *)Hid_dis_data.pnp_info;
        data_len = sizeof(Hid_dis_data.pnp_info);
        if (info == NULL) {
            info = BLE_SVC_DIS_SYSTEM_ID_DEFAULT;
        }
        break;
    default:
        rc = BLE_ATT_ERR_UNLIKELY;
    }

    if (!data_len) {
        data_len =  strlen(info);
    }

    if (info != NULL) {
        rc = os_mbuf_append(ctxt->om, info, data_len);
        if (rc) {
            rc = BLE_ATT_ERR_INSUFFICIENT_RES;
        }
    }

    return rc;
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC:
            ESP_LOGI("service","uuid16 %s handle=%d (%04X)",
                ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                ctxt->svc.handle, ctxt->svc.handle);
            break;

        case BLE_GATT_REGISTER_OP_CHR:
            ESP_LOGI("charact",
                "uuid16 %s arg %d def_handle=%d (%04X) val_handle=%d (%04X)",
                ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                (int)ctxt->chr.chr_def->arg,
                ctxt->chr.def_handle, ctxt->chr.def_handle,
                ctxt->chr.val_handle, ctxt->chr.val_handle);
            break;

        case BLE_GATT_REGISTER_OP_DSC:
            ESP_LOGI("descrip", "uuid16 %s arg %d handle=%d (%04X)",
                ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                (int)ctxt->dsc.dsc_def->arg,
                ctxt->dsc.handle, ctxt->dsc.handle);
            break;
    }
}

#define BREAK_IF_NOT_ZERO(EXPR2TEST) if ((EXPR2TEST)!= 0) break

int
gatt_svr_init(void)
{
    int rc = 0;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    memset(&Svc_char_handles, 0, sizeof(Svc_char_handles[0]) * HANDLE_HID_COUNT);

    do {
        BREAK_IF_NOT_ZERO( rc = ble_gatts_count_cfg(Gatt_svr_included_services) );

        BREAK_IF_NOT_ZERO( rc = ble_gatts_add_svcs(Gatt_svr_included_services) );

        ESP_LOGI(tag, "GATT included services added");

        BREAK_IF_NOT_ZERO( rc = ble_gatts_count_cfg(Gatt_svr_svcs) );

        BREAK_IF_NOT_ZERO( rc = ble_gatts_add_svcs(Gatt_svr_svcs) );

        ESP_LOGI(tag, "GATT root services added");

    } while (0);

    return rc;
}
