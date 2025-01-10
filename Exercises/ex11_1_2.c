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

static int on = 0;

void toogleLED() {
    on = !on;
    gpio_set_level(RED_LED_GPIO, on);
}

void app_main(void) {

    gpio_reset_pin(RED_LED_GPIO);
    gpio_set_direction(RED_LED_GPIO, GPIO_MODE_OUTPUT);
    
    printf("starting blinking. \n");

    esp_timer_handle_t timerHandle;
    esp_timer_create_args_t timerArg;
    timerArg.callback = &toggleLED;

    esp_timer_create(&timerArg, &timerHandle);
    
    esp_timer_start_periodic(timerHandle, 1 * 1000 * 1000);
}