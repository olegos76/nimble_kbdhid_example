#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include "gpio_func.h"
#include "hid_codes.h"

#define task_delay_ms(PAR_MS) vTaskDelay(pdMS_TO_TICKS(PAR_MS))

// time to wait for rattle to end in milliseconds
#define ANTI_RATTLE_TIME 5

#define ESP_INTR_FLAG_DEFAULT 0

/* default LED pin is 2 for ESP32 Devkit V1 */
#ifdef CONFIG_BLINK_GPIO
#define CAPS_GPIO CONFIG_BLINK_GPIO
#else
#define CAPS_GPIO 2
#endif

static const char *tag = "NimBLEKBD_gpio";

// #define WAIT_TICKS pdMS_TO_TICKS(ANTI_RATTLE_TIME)
// #if WAIT_TICKS == (0)
// #define WAIT_TICKS  (1)
// #endif

// ticks in antirattling time
int Ticks_to_wait = 1;

// buttons array
static struct kbd_button {
    // button's GPIO
    uint32_t gpio;

    // ticks from program start when rattling will over
    // if this not 0, then it is rattling right now
    TickType_t max_ticks;

    // button to emulate, 32 bit
    // bits 0-7: button keycode (from hid_codes.h)
    // bits 24-26: key code type: 1 - keyboard, 2 - consumer control, 3 - mouse keys
    // bits 8 to 15  (byte 1) - mouse axis X changes
    // bits 16 to 23 (byte 2) - mouse axis Y changes
    uint32_t hid_button;

    // last state of the button
    uint32_t last_state;
} Hid_buttons[] = {
    { .gpio = 13, .hid_button = HID_CONSUMER_VOLUME_DOWN    | BUTTON_TYPE_CC },
    { .gpio = 12, .hid_button = HID_CONSUMER_VOLUME_UP      | BUTTON_TYPE_CC },
    // { .gpio = 13, .hid_button = HID_KEY_LEFT_ARROW          | BUTTON_TYPE_KEYBOARD },
    // { .gpio = 12, .hid_button = HID_KEY_RIGHT_ARROW         | BUTTON_TYPE_KEYBOARD },
};
static int Hid_buttons_count = sizeof(Hid_buttons)/sizeof(Hid_buttons[0]);

static SemaphoreHandle_t ISR_semaphore = NULL;

static void IRAM_ATTR
gpio_isr_handler1(void* arg)  // gpio isr
{
    uint32_t gpio_num = (uint32_t)arg;
    int cur_button = -1;

    // find the index of button in Hid_buttons array
    for (int i = 0; i < Hid_buttons_count; ++i) {
        if (Hid_buttons[i].gpio == gpio_num) {
            cur_button = i;
            break;
        }
    }
    if (cur_button == -1) return;

    uint32_t cur_ticks = xTaskGetTickCountFromISR(),
             old_ticks = Hid_buttons[cur_button].max_ticks;

    Hid_buttons[cur_button].max_ticks = cur_ticks + Ticks_to_wait;

    // "give" semaphore to start gpio_btn_task watching at this gpio pin
    // ISR will not give seamphore on rattle interrupts
    if (old_ticks == 0) {
        xSemaphoreGiveFromISR(ISR_semaphore, NULL);
    }
}

void
gpio_reset()
{
    uint64_t in_pins = 0;
    for (int i = 0; i < Hid_buttons_count; ++i) {
        in_pins |= (1ULL << Hid_buttons[i].gpio);
    }
    gpio_config_t io_conf;
    memset(&io_conf, 0, sizeof(io_conf));
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = in_pins;
    io_conf.mode = GPIO_MODE_DISABLE;
    gpio_config(&io_conf);
    gpio_uninstall_isr_service();
}

void
gpio_setup()
{
    gpio_pad_select_gpio(CAPS_GPIO);
    gpio_set_direction(CAPS_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(CAPS_GPIO, 0);

    uint64_t in_pins = 0;
    for (int i = 0; i < Hid_buttons_count; ++i) {
        in_pins |= (1ULL << Hid_buttons[i].gpio);
    }
    gpio_config_t io_conf;
    memset(&io_conf, 0, sizeof(io_conf));
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = in_pins;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // GPIO ISR binding
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    for (uint32_t i = 0; i < Hid_buttons_count; ++i) {
        // zero button state values
        Hid_buttons[i].max_ticks = 0;
        Hid_buttons[i].last_state = Hid_buttons[i].hid_button | BUTTON_RELEASED_BIT;
        gpio_isr_handler_add(Hid_buttons[i].gpio, gpio_isr_handler1, (void *) Hid_buttons[i].gpio);
    }
}

int
set_leds(uint8_t hid_leds)
{
    // LEDS: bit 0 NUM LOCK, 1 CAPS LOCK, 2 SCROLL LOCK, 3 COMPOSE, 4 KANA, 5 to 7 CONSTANT
    gpio_set_level(CONFIG_BLINK_GPIO, (hid_leds & 2)>>1 ); // capslock
    return 0;
}

void IRAM_ATTR
gpio_btn_task(void* arg)
{
    QueueHandle_t buttons_queue = arg;
    TickType_t delay_time = portMAX_DELAY, cur_ticks;
    uint32_t button;

    ISR_semaphore = xSemaphoreCreateBinary();
    if (!ISR_semaphore || !buttons_queue) {
        ESP_LOGE(tag, "Can not create semaphore! %p %p", buttons_queue, ISR_semaphore);
        vTaskDelay(pdMS_TO_TICKS(30000));
        esp_restart();
    }

    // when ticks per second is too small, rattle period can be zero, but it is unacceptable
    Ticks_to_wait = pdMS_TO_TICKS(ANTI_RATTLE_TIME) > 0 ? pdMS_TO_TICKS(ANTI_RATTLE_TIME) : 1;

    gpio_setup();

    while(1) {

        xSemaphoreTake(ISR_semaphore, delay_time);

        cur_ticks = xTaskGetTickCount();

        delay_time = portMAX_DELAY;

        for (int i = 0; i < Hid_buttons_count; ++i) {

            if (Hid_buttons[i].max_ticks) {
                if (Hid_buttons[i].max_ticks <= cur_ticks) {
                    // this button does not rattle any more

                    button = Hid_buttons[i].hid_button;
                    // gpio level 0 is pressed, 1 is released
                    if (gpio_get_level(Hid_buttons[i].gpio) > 0) {
                        button |= BUTTON_RELEASED_BIT; // it is released
                    }

                    if (Hid_buttons[i].last_state == button) {
                        // false state change
                        Hid_buttons[i].max_ticks = 0;
                    } else {
                        if (xQueueSend(buttons_queue, (void *) &button, 0) == pdTRUE) {
                            Hid_buttons[i].max_ticks = 0;
                            Hid_buttons[i].last_state = button;
                            continue;
                        } else {
                            // no room in queue, trying to send it on next tick
                            ESP_LOGI(tag, "No room in out queue!");
                            Hid_buttons[i].max_ticks = cur_ticks + 1;
                        }
                    }
                }

                // find shortest time among the rattling buttons
                if (Hid_buttons[i].max_ticks > cur_ticks &&
                    Hid_buttons[i].max_ticks - cur_ticks < delay_time) {
                    delay_time = Hid_buttons[i].max_ticks - cur_ticks;
                }
            }
        }
    }
}
