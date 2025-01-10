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


static void callback(void* arg) {
    gpio_set_level(RED_LED_GPIO, LOW);
    printf("One-shot timer of 5 seconds finished.\n");
    int64_t start = *(int64_t*) arg;
    int64_t end = esp_timer_get_time();
    printf("Elapsed time: %lld\n", end - start);
}

void app_main(void) {
    esp_timer_handle_t timerHandle;
    esp_timer_create_args_t timerArg;

    int64_t start = esp_timer_get_time();

    gpio_reset_pin(RED_LED_GPIO);
    gpio_set_direction(RED_LED_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(RED_LED_GPIO, HIGH);
    
    printf("One-shot timer of 5 seconds started.\n");
    timerArg.callback = &callback;
    timerArg.arg = &start;
    esp_timer_create(&timerArg, &timerHandle);
    esp_timer_start_once(timerHandle, TIMEOUT);
}
