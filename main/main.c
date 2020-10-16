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

#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include "hid_codes.h"
#include "hid_func.h"
#include "gpio_func.h"

/* for nvs_storage*/
#define LOCAL_NAMESPACE "storage"


#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 0, 0)
typedef  nvs_handle nvs_handle_t;
#endif

static const char *tag = "NimBLEKBD_main";

nvs_handle_t Nvs_storage_handle = 0;

/* from ble_func.c */
extern void ble_init();

void
app_main(void)
{
    /* Initialize NVS — it is used to store PHY calibration data and Nimble bonding data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK( nvs_open(LOCAL_NAMESPACE, NVS_READWRITE, &Nvs_storage_handle) );


    QueueHandle_t buttons_queue = xQueueCreate(10, sizeof(uint32_t));
    if (!buttons_queue) {
        ESP_LOGE(tag, "Can not create queue!");
        vTaskDelay(pdMS_TO_TICKS(30000));
        esp_restart();
    }

    if (xTaskCreate(gpio_btn_task, "gpio_btn_task", 2048, buttons_queue, 10, NULL) != pdPASS) {
        ESP_LOGE(tag, "Can not create gpio_btn_task!");
        vTaskDelay(pdMS_TO_TICKS(30000));
        esp_restart();
    }

    ble_init();
    ESP_LOGI(tag, "BLE init ok, waiting for buttons ...");

    while (1) {
        uint32_t button, key_to_send;
        if (xQueueReceive(buttons_queue, &button, portMAX_DELAY) == pdTRUE) {

            // released or pressed?
            bool pressed = true;
            if (button & BUTTON_RELEASED_BIT) pressed = false;

            // byte 0 have a key code
            key_to_send = button & 0xff;

            ESP_LOGI(tag, "button %d type %08X (src %08X) %s",
                key_to_send, button & BUTTON_TYPE_MASK, button,
                pressed ? "pressed" : "released");

            switch (button & BUTTON_TYPE_MASK) {
                case BUTTON_TYPE_KEYBOARD:
                    hid_keyboard_change_key(key_to_send, pressed);
                    break;

                case BUTTON_TYPE_CC:
                    hid_cc_change_key(key_to_send, pressed);
                    break;

                case BUTTON_TYPE_MOUSE:
                    hid_mouse_change_key(key_to_send, 0, 0, pressed);
                    break;

                default:
                    ESP_LOGI(tag, "unknown button type %d", (button & BUTTON_TYPE_MASK) >> 24);
            }
        }
    }
}
