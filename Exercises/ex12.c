#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_GPIO_PIN 18
#define ESP_INTR_FLAG_DEFAULT 0

static int flag = 0;

static void IRAM_ATTR button_isr_handler(void *arg) {
    flag = 1;
}

static void configure_button(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO_PIN),  
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO_PIN, button_isr_handler, (void *)BUTTON_GPIO_PIN));
}


static void button_task() {
    configure_button();
    while (1) {
        if (flag) {
            printf("Button was pressed!\n");
            flag = 0;
        }
    }
}

void app_main(void) {
    button_task();
}