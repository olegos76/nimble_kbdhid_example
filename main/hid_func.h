
#ifndef H_HID_FUNC_
#define H_HID_FUNC_

#include "host/ble_gap.h"

extern void hid_clean_vars(struct ble_gap_conn_desc *desc);
extern void hid_set_disconnected();
extern void hid_set_notify(uint16_t attr_handle, uint8_t cur_notify, uint8_t cur_indicate);
extern bool hid_set_suspend(bool need_suspend);
extern bool hid_set_report_mode(bool boot_mode);

extern uint8_t hid_battery_level_get(void);

extern int hid_battery_level_set(uint8_t level);
extern int hid_keyboard_change_key(uint8_t key, bool pressed);
extern int hid_cc_change_key(int key, bool pressed);
extern int hid_mouse_change_key(int cmd, int8_t move_x, int8_t move_y, bool pressed);
extern int hid_leds_write(struct os_mbuf *buf);

extern int hid_write_buffer(struct os_mbuf *buf, int handle_num);

extern int hid_read_buffer(struct os_mbuf *buf, int handle_num);

#endif
