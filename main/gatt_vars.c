#include <stdio.h>

#include "gatt_svr.h"

#define SUPPORT_REPORT_VENDOR false

// HID Report Map characteristic value
// Keyboard report descriptor (using format for Boot interface descriptor)
const uint8_t Hid_report_map[] = {
    /*** MOUSE REPORT ***/
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x02,  // Usage (Mouse)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x01,  // Report Id (1)
    0x09, 0x01,  //   Usage (Pointer)
    0xA1, 0x00,  //   Collection (Physical)
    0x05, 0x09,  //     Usage Page (Buttons)
    0x19, 0x01,  //     Usage Minimum (01) - Button 1
    0x29, 0x03,  //     Usage Maximum (03) - Button 3
    0x15, 0x00,  //     Logical Minimum (0)
    0x25, 0x01,  //     Logical Maximum (1)
    0x75, 0x01,  //     Report Size (1)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x02,  //     Input (Data, Variable, Absolute) - Button states
    0x75, 0x05,  //     Report Size (5)
    0x95, 0x01,  //     Report Count (1)
    0x81, 0x01,  //     Input (Constant) - Padding or Reserved bits
    0x05, 0x01,  //     Usage Page (Generic Desktop)
    0x09, 0x30,  //     Usage (X)
    0x09, 0x31,  //     Usage (Y)
    0x09, 0x38,  //     Usage (Wheel)
    0x15, 0x81,  //     Logical Minimum (-127)
    0x25, 0x7F,  //     Logical Maximum (127)
    0x75, 0x08,  //     Report Size (8)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x06,  //     Input (Data, Variable, Relative) - X coordinate, Y coordinate, wheel
    0xC0,        //   End Collection
    0xC0,        // End Collection

    /*** KEYBOARD REPORT ***/
    0x05, 0x01,  // Usage Pg (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection: (Application)
    0x85, 0x02,  // Report Id (2)
    //
    0x05, 0x07,  //   Usage Pg (Key Codes)
    0x19, 0xE0,  //   Usage Min (224)
    0x29, 0xE7,  //   Usage Max (231)
    0x15, 0x00,  //   Log Min (0)
    0x25, 0x01,  //   Log Max (1)
    //
    //   Modifier byte
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input: (Data, Variable, Absolute)
    //
    //   Reserved byte
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x08,  //   Report Size (8)
    0x81, 0x01,  //   Input: (Constant)
    //
    //   LED report
    0x95, 0x05,  //   Report Count (5)
    0x75, 0x01,  //   Report Size (1)
    0x05, 0x08,  //   Usage Pg (LEDs)
    0x19, 0x01,  //   Usage Min (1)
    0x29, 0x05,  //   Usage Max (5)
    0x91, 0x02,  //   Output: (Data, Variable, Absolute)
    //
    //   LED report padding
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x03,  //   Report Size (3)
    0x91, 0x01,  //   Output: (Constant)
    //
    //   Key arrays (6 bytes)
    0x95, 0x06,  //   Report Count (6)
    0x75, 0x08,  //   Report Size (8)
    0x15, 0x00,  //   Log Min (0)
    0x25, 0x65,  //   Log Max (101)
    0x05, 0x07,  //   Usage Pg (Key Codes)
    0x19, 0x00,  //   Usage Min (0)
    0x29, 0x65,  //   Usage Max (101)
    0x81, 0x00,  //   Input: (Data, Array)
    //
    0xC0,        // End Collection
    //
    /*** CONSUMER DEVICE REPORT ***/
    0x05, 0x0C,   // Usage Pg (Consumer Devices)
    0x09, 0x01,   // Usage (Consumer Control)
    0xA1, 0x01,   // Collection (Application)
    0x85, 0x03,   // Report Id (3)
    0x09, 0x02,   //   Usage (Numeric Key Pad)
    0xA1, 0x02,   //   Collection (Logical)
    0x05, 0x09,   //     Usage Pg (Button)
    0x19, 0x01,   //     Usage Min (Button 1)
    0x29, 0x0A,   //     Usage Max (Button 10)
    0x15, 0x01,   //     Logical Min (1)
    0x25, 0x0A,   //     Logical Max (10)
    0x75, 0x04,   //     Report Size (4)
    0x95, 0x01,   //     Report Count (1)
    0x81, 0x00,   //     Input (Data, Ary, Abs)
    0xC0,         //   End Collection
    0x05, 0x0C,   //   Usage Pg (Consumer Devices)
    0x09, 0x86,   //   Usage (Channel)
    0x15, 0xFF,   //   Logical Min (-1)
    0x25, 0x01,   //   Logical Max (1)
    0x75, 0x02,   //   Report Size (2)
    0x95, 0x01,   //   Report Count (1)
    0x81, 0x46,   //   Input (Data, Var, Rel, Null)
    0x09, 0xE9,   //   Usage (Volume Up)
    0x09, 0xEA,   //   Usage (Volume Down)
    0x15, 0x00,   //   Logical Min (0)
    0x75, 0x01,   //   Report Size (1)
    0x95, 0x02,   //   Report Count (2)
    0x81, 0x02,   //   Input (Data, Var, Abs)
    0x09, 0xE2,   //   Usage (Mute)
    0x09, 0x30,   //   Usage (Power)
    0x09, 0x83,   //   Usage (Recall Last)
    0x09, 0x81,   //   Usage (Assign Selection)
    0x09, 0xB0,   //   Usage (Play)
    0x09, 0xB1,   //   Usage (Pause)
    0x09, 0xB2,   //   Usage (Record)
    0x09, 0xB3,   //   Usage (Fast Forward)
    0x09, 0xB4,   //   Usage (Rewind)
    0x09, 0xB5,   //   Usage (Scan Next)
    0x09, 0xB6,   //   Usage (Scan Prev)
    0x09, 0xB7,   //   Usage (Stop)
    0x15, 0x01,   //   Logical Min (1)
    0x25, 0x0C,   //   Logical Max (12)
    0x75, 0x04,   //   Report Size (4)
    0x95, 0x01,   //   Report Count (1)
    0x81, 0x00,   //   Input (Data, Ary, Abs)
    0x09, 0x80,   //   Usage (Selection)
    0xA1, 0x02,   //   Collection (Logical)
    0x05, 0x09,   //     Usage Pg (Button)
    0x19, 0x01,   //     Usage Min (Button 1)
    0x29, 0x03,   //     Usage Max (Button 3)
    0x15, 0x01,   //     Logical Min (1)
    0x25, 0x03,   //     Logical Max (3)
    0x75, 0x02,   //     Report Size (2)
    0x81, 0x00,   //     Input (Data, Ary, Abs)
    0xC0,         //   End Collection
    0x81, 0x03,   //   Input (Const, Var, Abs)
    0xC0,         // End Collection
};

size_t Hid_report_map_size = sizeof(Hid_report_map);

#define NO_MINKEYSIZE     .min_key_size = DEFAULT_MIN_KEY_SIZE
#define NO_ARG_MINKEYSIZE .arg = NULL, NO_MINKEYSIZE
#define NO_ARG_DESCR_MKS  .descriptors = NULL, NO_ARG_MINKEYSIZE
#define NO_DESCR_MKS      .descriptors = NULL, NO_MINKEYSIZE

#define MY_NOTIFY_FLAGS (BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE)

const struct ble_gatt_svc_def Gatt_svr_included_services[] = {
    {
        /*** Battery Service. */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_SVC_BAS_UUID16),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[]) { {
        /*** Battery level characteristic */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_BAS_CHR_UUID16_BATTERY_LEVEL),
            .access_cb = ble_svc_battery_access,
            .arg = (void *) HANDLE_BATTERY_LEVEL,
            .val_handle = &Svc_char_handles[HANDLE_BATTERY_LEVEL],
            .flags = MY_NOTIFY_FLAGS,
            NO_MINKEYSIZE,
            .descriptors = (struct ble_gatt_dsc_def[]) { {
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_BAT_PRESENT_DESCR),
                .att_flags = BLE_ATT_F_READ | BLE_ATT_F_READ_ENC,
                .access_cb = ble_svc_battery_access,
                NO_ARG_MINKEYSIZE,
            }, {
                0, /* No more descriptors in this characteristic. */
            } },
        }, {
            0, /* No more characteristics in this service. */
        } },
    },

    { /*** Service: Device Information Service (DIS). */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_UUID16),
        .includes = NULL,
        .characteristics = (struct ble_gatt_chr_def[]) { {
        /*** Characteristic: Model Number String */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_MODEL_NUMBER),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_MODEL_NUMBER],
            .flags = BLE_GATT_CHR_F_READ | (BLE_SVC_DIS_MODEL_NUMBER_READ_PERM),
            NO_ARG_DESCR_MKS,
        }, {
        /*** Characteristic: Serial Number String */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_SERIAL_NUMBER),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_SERIAL_NUMBER],
            .flags = BLE_GATT_CHR_F_READ | (BLE_SVC_DIS_SERIAL_NUMBER_READ_PERM),
            NO_ARG_DESCR_MKS,
        }, {
        /*** Characteristic: Hardware Revision String */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_HARDWARE_REVISION),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_HARDWARE_REVISION],
            .flags = BLE_GATT_CHR_F_READ | (BLE_SVC_DIS_HARDWARE_REVISION_READ_PERM),
            NO_ARG_DESCR_MKS,
        }, {
        /*** Characteristic: Firmware Revision String */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_FIRMWARE_REVISION),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_FIRMWARE_REVISION],
            .flags = BLE_GATT_CHR_F_READ | (BLE_SVC_DIS_FIRMWARE_REVISION_READ_PERM),
            NO_ARG_DESCR_MKS,
        }, {
        /*** Characteristic: Software Revision String */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_SOFTWARE_REVISION),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_SOFWARE_REVISION],
            .flags = BLE_GATT_CHR_F_READ | (BLE_SVC_DIS_SOFTWARE_REVISION_READ_PERM),
            NO_ARG_DESCR_MKS,
        }, {
        /*** Characteristic: Manufacturer Name */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_MANUFACTURER_NAME),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_MANUFACTURER_NAME],
            .flags = BLE_GATT_CHR_F_READ | (BLE_SVC_DIS_MANUFACTURER_NAME_READ_PERM),
            NO_ARG_DESCR_MKS,
        }, {
      /*** Characteristic: System Id */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_SYSTEM_ID),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_SYSTEM_ID],
            .flags = BLE_GATT_CHR_F_READ | (BLE_SVC_DIS_SYSTEM_ID_READ_PERM),
            NO_ARG_DESCR_MKS,
        }, {
      /*** Characteristic: System Id */
            .uuid = BLE_UUID16_DECLARE(BLE_SVC_DIS_CHR_UUID16_PNP_INFO),
            .access_cb = ble_svc_dis_access,
            .val_handle = &Svc_char_handles[HANDLE_DIS_PNP_INFO],
            .flags = BLE_GATT_CHR_F_READ,
            NO_ARG_DESCR_MKS,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },

    {
        0, /* No more services. */
    },
};

const struct ble_gatt_svc_def *Inc_svcs[] = {
    &Gatt_svr_included_services[0],
    NULL,
};

const struct ble_gatt_svc_def Gatt_svr_svcs[] = {
    {
        /*** HID Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_SERVICE),
        .includes = Inc_svcs,
        .characteristics = (struct ble_gatt_chr_def[])
        {
            {
            /*** HID INFO characteristic */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_INFORMATION),
                .access_cb = hid_svr_chr_access,
                .val_handle = &Svc_char_handles[HANDLE_HID_INFORMATION],
                .flags = BLE_GATT_CHR_F_READ, // | BLE_GATT_CHR_F_READ_ENC,
                NO_ARG_DESCR_MKS,
            }, {
            /*** HID Control Point */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_CONTROL_POINT),
                .access_cb = hid_svr_chr_access,
                .val_handle = &Svc_char_handles[HANDLE_HID_CONTROL_POINT],
                .flags = BLE_GATT_CHR_F_WRITE, // | BLE_GATT_CHR_F_WRITE_ENC,
                NO_ARG_DESCR_MKS,
            }, {
            /*** Report Map */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_REPORT_MAP),
                .access_cb = hid_svr_chr_access,
                .val_handle = &Svc_char_handles[HANDLE_HID_REPORT_MAP],
                .flags = BLE_GATT_CHR_F_READ,
                NO_ARG_MINKEYSIZE,
                .descriptors = (struct ble_gatt_dsc_def[]) { {
                    /*** External Report Reference Descriptor */
                    .uuid = BLE_UUID16_DECLARE(GATT_UUID_EXT_RPT_REF_DESCR),
                    .att_flags = BLE_ATT_F_READ,
                    .access_cb = hid_svr_chr_access,
                    NO_ARG_MINKEYSIZE,
                }, {
                    0, /* No more descriptors in this characteristic. */
                } },
            }, {
            /*** Protocol Mode Characteristic */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_PROTO_MODE),
                .access_cb = hid_svr_chr_access,
                .val_handle = &Svc_char_handles[HANDLE_HID_PROTO_MODE],
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                NO_ARG_DESCR_MKS,
            }, {
            /*** Mouse hid report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_REPORT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_MOUSE_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_MOUSE_REPORT],
                .flags = MY_NOTIFY_FLAGS,
                .min_key_size = DEFAULT_MIN_KEY_SIZE,
                .descriptors = (struct ble_gatt_dsc_def[]) { {
                    /* Report Reference Descriptor */
                    .uuid = BLE_UUID16_DECLARE(GATT_UUID_RPT_REF_DESCR),
                    .att_flags = BLE_ATT_F_READ,
                    .access_cb = ble_svc_report_access,
                    .arg = (void *)HANDLE_HID_MOUSE_REPORT,
                    .min_key_size = DEFAULT_MIN_KEY_SIZE,
                }, {
                    0, /* No more descriptors in this characteristic. */
                } },
            }, {
            /*** Keyboard hid report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_REPORT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_KB_IN_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_KB_IN_REPORT],
                .flags = MY_NOTIFY_FLAGS,
                .min_key_size = DEFAULT_MIN_KEY_SIZE,
                .descriptors = (struct ble_gatt_dsc_def[]) { {
                    /* Report Reference Descriptor */
                    .uuid = BLE_UUID16_DECLARE(GATT_UUID_RPT_REF_DESCR),
                    .att_flags = BLE_ATT_F_READ,
                    .access_cb = ble_svc_report_access,
                    .arg = (void *)HANDLE_HID_KB_IN_REPORT,
                    .min_key_size = DEFAULT_MIN_KEY_SIZE,
                }, {
                    0, /* No more descriptors in this characteristic. */
                } },
            }, {
            /*** Keyboard hid out (LED IN/OUT) report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_REPORT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_KB_OUT_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_KB_OUT_REPORT],
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
                .min_key_size = DEFAULT_MIN_KEY_SIZE,
                .descriptors = (struct ble_gatt_dsc_def[]) { {
                    /* Report Reference Descriptor */
                    .uuid = BLE_UUID16_DECLARE(GATT_UUID_RPT_REF_DESCR),
                    .att_flags = BLE_ATT_F_READ,
                    .access_cb = ble_svc_report_access,
                    .arg = (void *)HANDLE_HID_KB_OUT_REPORT,
                    .min_key_size = DEFAULT_MIN_KEY_SIZE,
                }, {
                    0, /* No more descriptors in this characteristic. */
                } },
            }, {
            /*** Consumer control hid report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_REPORT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_CC_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_CC_REPORT],
                .flags = MY_NOTIFY_FLAGS,
                .min_key_size = DEFAULT_MIN_KEY_SIZE,
                .descriptors = (struct ble_gatt_dsc_def[]) { {
                    /* Report Reference Descriptor */
                    .uuid = BLE_UUID16_DECLARE(GATT_UUID_RPT_REF_DESCR),
                    .att_flags = BLE_ATT_F_READ,
                    .access_cb = ble_svc_report_access,
                    .arg = (void *)HANDLE_HID_CC_REPORT,
                    .min_key_size = DEFAULT_MIN_KEY_SIZE,
                }, {
                    0, /* No more descriptors in this characteristic. */
                } },
            }, {
            /*** Keyboard input boot hid report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_BT_KB_INPUT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_BOOT_KB_IN_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_BOOT_KB_IN_REPORT],
                .flags = MY_NOTIFY_FLAGS,
                NO_DESCR_MKS,
            }, {
            /*** Keyboard output boot hid report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_BT_KB_OUTPUT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_BOOT_KB_OUT_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_BOOT_KB_OUT_REPORT],
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
                NO_DESCR_MKS,
            }, {
            /*** Mouse input boot hid report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_BT_MOUSE_INPUT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_BOOT_MOUSE_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_BOOT_MOUSE_REPORT],
                .flags = MY_NOTIFY_FLAGS,
                NO_DESCR_MKS,
            }, {
            /*** Feature hid report */
                .uuid = BLE_UUID16_DECLARE(GATT_UUID_HID_REPORT),
                .access_cb = ble_svc_report_access,
                .arg = (void *)HANDLE_HID_FEATURE_REPORT,
                .val_handle = &Svc_char_handles[HANDLE_HID_FEATURE_REPORT],
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
                .min_key_size = DEFAULT_MIN_KEY_SIZE,
                .descriptors = (struct ble_gatt_dsc_def[]) { {
                    /* Report Reference Descriptor */
                    .uuid = BLE_UUID16_DECLARE(GATT_UUID_RPT_REF_DESCR),
                    .att_flags = BLE_ATT_F_READ,
                    .access_cb = ble_svc_report_access,
                    .arg = (void *)HANDLE_HID_FEATURE_REPORT,
                    .min_key_size = DEFAULT_MIN_KEY_SIZE,
                }, {
                    0, /* No more descriptors in this characteristic. */
                } },
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },

    {
        0, /* No more services. */
    },
};

/* handles for all characteristics in GATT services */
uint16_t Svc_char_handles[HANDLE_HID_COUNT];

#define BCDHID_DATA 0x0111

const uint8_t HidInfo[HID_INFORMATION_LEN] = {
    LO_UINT16(BCDHID_DATA), HI_UINT16(BCDHID_DATA),   // bcdHID (USB HID version)
    0x00,                                             // bCountryCode
    HID_KBD_FLAGS                                     // Flags
};

// HID External Report Reference Descriptor
uint16_t HidExtReportRefDesc = BLE_SVC_BAS_CHR_UUID16_BATTERY_LEVEL;
uint8_t  HidProtocolMode = HID_PROTOCOL_MODE_REPORT;

/* Report reference table, byte 0 - report id from report map, byte 1 - report type (in,out,feature)*/
struct report_reference_table Hid_report_ref_data[] = {
    { .id = HANDLE_HID_MOUSE_REPORT,    .hidReportRef = { HID_RPT_ID_MOUSE_IN,  HID_REPORT_TYPE_INPUT   }},
    { .id = HANDLE_HID_KB_IN_REPORT,    .hidReportRef = { HID_RPT_ID_KB_IN,     HID_REPORT_TYPE_INPUT   }},
    { .id = HANDLE_HID_KB_OUT_REPORT,   .hidReportRef = { HID_RPT_ID_KB_IN,     HID_REPORT_TYPE_OUTPUT  }},
    { .id = HANDLE_HID_CC_REPORT,       .hidReportRef = { HID_RPT_ID_CC_IN,     HID_REPORT_TYPE_INPUT   }},
    { .id = HANDLE_HID_FEATURE_REPORT,  .hidReportRef = { HID_RPT_ID_FEATURE,   HID_REPORT_TYPE_FEATURE }},
};
size_t Hid_report_ref_data_count = sizeof(Hid_report_ref_data)/sizeof(Hid_report_ref_data[0]);

// battery level unit - percents
struct prf_char_pres_fmt Battery_level_units = {
    .format = 4,      // Unsigned 8-bit
    .exponent = 0,
    .unit = 0x27AD,   // percentage
    .name_space = 1,  // BLUETOOTH SIG
    .description = 0
};

/* Device information
variable name restricted in ESP_IDF
components/bt/host/nimble/nimble/nimble/host/services/dis/include/services/dis/ble_svc_dis.h
*/
struct ble_svc_dis_data Hid_dis_data = {
    .model_number      = BLE_SVC_DIS_MODEL_NUMBER_DEFAULT,
    .serial_number     = BLE_SVC_DIS_SERIAL_NUMBER_DEFAULT,
    .firmware_revision = BLE_SVC_DIS_FIRMWARE_REVISION_DEFAULT,
    .hardware_revision = BLE_SVC_DIS_HARDWARE_REVISION_DEFAULT,
    .software_revision = BLE_SVC_DIS_SOFTWARE_REVISION_DEFAULT,
    .manufacturer_name = BLE_SVC_DIS_MANUFACTURER_NAME_DEFAULT,
    .system_id         = BLE_SVC_DIS_SYSTEM_ID_DEFAULT,
    .pnp_info          = BLE_SVC_DIS_PNP_INFO_DEFAULT,
};
