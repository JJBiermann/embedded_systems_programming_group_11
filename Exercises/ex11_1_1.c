/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "driver/gpio.h"

#define RED_LED_GPIO 8
#define TIMEOUT 5 * 1000 * 1000
#define LOW 0
#define HIGH 1


void turnOffLED() {
    gpio_set_level(RED_LED_GPIO, LOW);
    printf("One-shot timer of 5 seconds finished.\n");
}

void app_main(void) {

    gpio_reset_pin(RED_LED_GPIO);
    gpio_set_direction(RED_LED_GPIO, GPIO_MODE_OUTPUT);

    printf("One-shot timer of 5 seconds started.\n");

    esp_timer_handle_t timerHandle;
    esp_timer_create_args_t timerArg;
    timerArg.callback = &turnOffLED;
    esp_timer_create(&timerArg, &timerHandle);
    
    gpio_set_level(RED_LED_GPIO, HIGH);
    esp_timer_start_once(timerHandle, TIMEOUT);
}