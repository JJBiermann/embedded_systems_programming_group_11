#include "ButtonTool.h"
#include "TaskSetup.h"

#include "driver/gpio.h"

void setupInterruptButton() {
    // Set button 1 pin to interrupt on high level
    gpio_config_t io_conf; 
    io_conf.pin_bit_mask = (1ULL<<BTN_1_GPIO_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_1_GPIO_PIN, &buttonOneInterruptHandler, (void*) 0);
}

void setupInterruptButtonTwo() {
    // Set button 2 pin to interrupt on high level
    gpio_config_t io_conf; 
    io_conf.pin_bit_mask = (1ULL<<BTN_2_GPIO_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_2_GPIO_PIN, &buttonTwoInterruptHandler, (void*) 0);
}