#ifndef H_GPIO_FUNC_
#define H_GPIO_FUNC_

#define BUTTON_RELEASED_BIT     (uint32_t)(1 << 31)
#define BUTTON_TYPE_MASK        (uint32_t)(3 << 24)
#define BUTTON_TYPE_KEYBOARD    (uint32_t)(1 << 24)
#define BUTTON_TYPE_CC          (uint32_t)(2 << 24)
#define BUTTON_TYPE_MOUSE       (uint32_t)(3 << 24)

extern void gpio_btn_task(void* arg);

extern int set_leds(uint8_t hid_leds);

#endif