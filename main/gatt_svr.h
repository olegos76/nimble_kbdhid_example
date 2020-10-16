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

#ifndef H_BLEPRPH_
#define H_BLEPRPH_

#include <stdbool.h>

#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/bas/ble_svc_bas.h"
#include "svc_dis.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/util/util.h"

#include "modlog/modlog.h"
#include "hid_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

/** GATT server. */
#define GATT_SVR_SVC_ALERT_UUID                 0x1811
#define GATT_SVR_CHR_SUP_NEW_ALERT_CAT_UUID     0x2A47
#define GATT_SVR_CHR_NEW_ALERT                  0x2A46
#define GATT_SVR_CHR_SUP_UNR_ALERT_CAT_UUID     0x2A48
#define GATT_SVR_CHR_UNR_ALERT_STAT_UUID        0x2A45
#define GATT_SVR_CHR_ALERT_NOT_CTRL_PT          0x2A44

#define GATT_UUID_HID_SERVICE                   0x1812

#define GATT_UUID_HID_INFORMATION               0x2A4A
#define GATT_UUID_HID_REPORT_MAP                0x2A4B
#define GATT_UUID_HID_CONTROL_POINT             0x2A4C
#define GATT_UUID_HID_REPORT                    0x2A4D
#define GATT_UUID_HID_PROTO_MODE                0x2A4E
#define GATT_UUID_HID_BT_KB_INPUT               0x2A22
#define GATT_UUID_HID_BT_KB_OUTPUT              0x2A32
#define GATT_UUID_HID_BT_MOUSE_INPUT            0x2A33

#define GATT_UUID_BAT_PRESENT_DESCR             0x2904
#define GATT_UUID_EXT_RPT_REF_DESCR             0x2907
#define GATT_UUID_RPT_REF_DESCR                 0x2908

#define BLE_SVC_DIS_MODEL_NUMBER_DEFAULT        "0x0102"
#define BLE_SVC_DIS_SERIAL_NUMBER_DEFAULT       "0x0001"
#define BLE_SVC_DIS_FIRMWARE_REVISION_DEFAULT   "0x1409"
#define BLE_SVC_DIS_HARDWARE_REVISION_DEFAULT   "0x0001"
#define BLE_SVC_DIS_SOFTWARE_REVISION_DEFAULT   "0x1409"
#define BLE_SVC_DIS_MANUFACTURER_NAME_DEFAULT   "https://github.com/olegos76"
#define BLE_SVC_DIS_SYSTEM_ID_DEFAULT           "esp32"
#define BLE_SVC_DIS_PNP_INFO_DEFAULT            {0x00,0x47,0x00,0xff,0xff,0xff,0xff}

/*
    Defines default permissions for reading characteristics. Can be
    zero to allow read without extra permissions or combination of:
        BLE_GATT_CHR_F_READ_ENC
        BLE_GATT_CHR_F_READ_AUTHEN
        BLE_GATT_CHR_F_READ_AUTHOR
*/

#define BLE_SVC_DIS_MODEL_NUMBER_READ_PERM      0
#define BLE_SVC_DIS_SERIAL_NUMBER_READ_PERM     0
#define BLE_SVC_DIS_HARDWARE_REVISION_READ_PERM 0
#define BLE_SVC_DIS_FIRMWARE_REVISION_READ_PERM 0
#define BLE_SVC_DIS_SOFTWARE_REVISION_READ_PERM 0
#define BLE_SVC_DIS_MANUFACTURER_NAME_READ_PERM 0
#define BLE_SVC_DIS_SYSTEM_ID_READ_PERM         0

/* Attribute value lengths */
#define HID_PROTOCOL_MODE_LEN           1         // HID Protocol Mode
#define HID_INFORMATION_LEN             4         // HID Information
#define HID_REPORT_REF_LEN              2         // HID Report Reference Descriptor
#define HID_EXT_REPORT_REF_LEN          2         // External Report Reference Descriptor

// HID Report IDs for the service
#define HID_RPT_ID_KB_OUT               0   // LED output report ID from report map
#define HID_RPT_ID_MOUSE_IN             1   // Mouse input report ID from report map
#define HID_RPT_ID_KB_IN                2   // Keyboard input report ID from report map
#define HID_RPT_ID_CC_IN                3   // Consumer Control input report ID from report map
#define HID_RPT_ID_FEATURE              4   // Feature report ID from report map

// boot report cb_access args
#define HID_BOOT_KB_IN                  6   // Keyboard input report ID
#define HID_BOOT_KB_OUT                 7   // LED output report ID
#define HID_BOOT_MOUSE_IN               8   // Mouse input report ID

// HID Report types
#define HID_REPORT_TYPE_INPUT           1
#define HID_REPORT_TYPE_OUTPUT          2
#define HID_REPORT_TYPE_FEATURE         3

#define HID_CC_RPT_MUTE                 1
#define HID_CC_RPT_POWER                2
#define HID_CC_RPT_LAST                 3
#define HID_CC_RPT_ASSIGN_SEL           4
#define HID_CC_RPT_PLAY                 5
#define HID_CC_RPT_PAUSE                6
#define HID_CC_RPT_RECORD               7
#define HID_CC_RPT_FAST_FWD             8
#define HID_CC_RPT_REWIND               9
#define HID_CC_RPT_SCAN_NEXT_TRK        10
#define HID_CC_RPT_SCAN_PREV_TRK        11
#define HID_CC_RPT_STOP                 12

#define HID_CC_RPT_CHANNEL_UP           0x01
#define HID_CC_RPT_CHANNEL_DOWN         0x03
#define HID_CC_RPT_VOLUME_UP            0x40
#define HID_CC_RPT_VOLUME_DOWN          0x80

// HID Consumer Control report bitmasks
#define HID_CC_RPT_NUMERIC_BITS         0xF0
#define HID_CC_RPT_CHANNEL_BITS         0xCF
#define HID_CC_RPT_VOLUME_BITS          0x3F
#define HID_CC_RPT_BUTTON_BITS          0xF0
#define HID_CC_RPT_SELECTION_BITS       0xCF


// Macros for the HID Consumer Control 2-byte report
#define HID_CC_RPT_SET_NUMERIC(s, x)    (s)[0] &= HID_CC_RPT_NUMERIC_BITS;   \
                                        (s)[0] |= (x)
#define HID_CC_RPT_SET_CHANNEL(s, x)    (s)[0] &= HID_CC_RPT_CHANNEL_BITS;   \
                                        (s)[0] |= ((x) & 0x03) << 4
#define HID_CC_RPT_SET_VOLUME(s, x)     (s)[0] &= HID_CC_RPT_VOLUME_BITS;    \
                                        (s)[0] |= (x)
#define HID_CC_RPT_SET_BUTTON(s, x)     (s)[1] &= HID_CC_RPT_BUTTON_BITS;    \
                                        (s)[1] |= (x)
#define HID_CC_RPT_SET_SELECTION(s, x)  (s)[1] &= HID_CC_RPT_SELECTION_BITS; \
                                        (s)[1] |= ((x) & 0x03) << 4

// Keyboard report size
#define HIDD_LE_REPORT_KB_IN_SIZE       (8)

// Mouse report size
#define HIDD_LE_REPORT_MOUSE_SIZE       (4)

// LEDS report size
#define HIDD_LE_REPORT_KB_OUT_SIZE      (1)

// Consumer control report size
#define HIDD_LE_REPORT_CC_SIZE          (2)

// battery level data size
#define HIDD_LE_BATTERY_LEVEL_SIZE      (1)

// feature data size
#define HIDD_LE_REPORT_FEATURE          (6)

/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE           0x01      // RemoteWake
#define HID_FLAGS_NORMALLY_CONNECTABLE  0x02      // NormallyConnectable

/* Control point commands */
#define HID_CMD_SUSPEND                 0x00      // Suspend
#define HID_CMD_EXIT_SUSPEND            0x01      // Exit Suspend

/* HID protocol mode values */
#define HID_PROTOCOL_MODE_BOOT          0x00      // Boot Protocol Mode
#define HID_PROTOCOL_MODE_REPORT        0x01      // Report Protocol Mode

// HID feature flags
//#define HID_KBD_FLAGS                   HID_FLAGS_NORMALLY_CONNECTABLE
#define HID_KBD_FLAGS                   HID_FLAGS_REMOTE_WAKE

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define DEFAULT_MIN_KEY_SIZE            0

#define NVS_IO_CAP_NUM "io_cap_number"
#define DEFAULT_IO_CAP BLE_HS_IO_NO_INPUT_OUTPUT

#define HID_KEYBOARD_APPEARENCE         0x03c1 // appearance field in advertising packet

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;


// Battery level presentation descriptor value format
struct prf_char_pres_fmt
{
    /// Format
    uint8_t format;
    /// Exponent
    uint8_t exponent;
    /// Unit (The Unit is a UUID)
    uint16_t unit;
    /// Name space
    uint8_t name_space;
    /// Description
    uint16_t description;
} __attribute__((packed));

enum attr_handles {
    HANDLE_BATTERY_LEVEL,               //  0
    HANDLE_DIS_MODEL_NUMBER,            //  1
    HANDLE_DIS_SERIAL_NUMBER,           //  2
    HANDLE_DIS_HARDWARE_REVISION,       //  3
    HANDLE_DIS_FIRMWARE_REVISION,       //  4
    HANDLE_DIS_SOFWARE_REVISION,        //  5
    HANDLE_DIS_MANUFACTURER_NAME,       //  6
    HANDLE_DIS_SYSTEM_ID,               //  7
    HANDLE_DIS_PNP_INFO,                //  8

    // HID SERVICE
    HANDLE_HID_INFORMATION,             //  9
    HANDLE_HID_CONTROL_POINT,           // 10
    HANDLE_HID_REPORT_MAP,              // 11
    HANDLE_HID_PROTO_MODE,              // 12
    HANDLE_HID_MOUSE_REPORT,            // 13
    HANDLE_HID_KB_IN_REPORT,            // 14
    HANDLE_HID_KB_OUT_REPORT,           // 15
    HANDLE_HID_CC_REPORT,               // 16
    HANDLE_HID_BOOT_KB_IN_REPORT,       // 17
    HANDLE_HID_BOOT_KB_OUT_REPORT,      // 18
    HANDLE_HID_BOOT_MOUSE_REPORT,       // 19
    HANDLE_HID_FEATURE_REPORT,          // 20
    HANDLE_HID_COUNT                    // 21
};

struct report_reference_table {
    int id;
    uint8_t hidReportRef[HID_REPORT_REF_LEN];
} __attribute__((packed));


void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);


int hid_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg);

int ble_svc_report_access(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg);

/* Access function for battery service */
int ble_svc_battery_access(uint16_t conn_handle, uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt, void *arg);

/* Access function for device information service */
int ble_svc_dis_access(uint16_t conn_handle, uint16_t attr_handle,
                   struct ble_gatt_access_ctxt *ctxt, void *arg);

// Globals

extern const uint8_t Hid_report_map[];

extern size_t Hid_report_map_size;

/* handles for all characteristics in GATT services */
extern uint16_t Svc_char_handles[];

extern const struct ble_gatt_svc_def Gatt_svr_included_services[];
extern const struct ble_gatt_svc_def Gatt_svr_svcs[];

extern const uint8_t HidInfo[];

// HID External Report Reference Descriptor
extern uint16_t HidExtReportRefDesc;
extern uint8_t HidProtocolMode;

extern struct report_reference_table Hid_report_ref_data[];
extern size_t Hid_report_ref_data_count;

extern struct prf_char_pres_fmt Battery_level_units;

extern struct ble_svc_dis_data Hid_dis_data;

#ifdef __cplusplus
}
#endif

#endif
