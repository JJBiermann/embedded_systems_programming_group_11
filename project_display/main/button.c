#include "button.h"
#include "SensorTool.h"

#define BUTTON_1_GPIO_PIN 18
#define BUTTON_2_GPIO_PIN 19 // reboot
#define ESP_INTR_FLAG_DEFAULT 0
// static struct SensorData data;
volatile int button1flag = 0;
volatile int button2flag = 0;
TaskHandle_t displayTaskHandle = NULL;
TaskHandle_t monitorButtonsTaskHandle = NULL;
TimerHandle_t light_poll, temp_humid_poll, soil_poll;

void IRAM_ATTR button1_isr_handler(void *arg)
{
    button1flag = 1;
    ESP_EARLY_LOGI("BUTTON_ISR", "Button1 pressed in ISR");
}

void IRAM_ATTR button2_isr_handler(void *arg)
{

    button2flag = 1;
    ESP_EARLY_LOGI("BUTTON_ISR", "Button2 pressed in ISR");
}

void setup_Button()
{
    gpio_config_t io_conf;

    // BUTTON 1
    io_conf.pin_bit_mask = (1ULL << BUTTON_1_GPIO_PIN);
    io_conf.mode = GPIO_MODE_INPUT,
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      // 启用上拉电阻
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // 禁用下拉电阻
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);

    // BUTTON 2
    io_conf.pin_bit_mask = (1ULL << BUTTON_2_GPIO_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      // 启用上拉电阻
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // 禁用下拉电阻
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&io_conf);
    if (gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3) != ESP_OK)
    {
        printf("Failed to install ISR service\n");
        return;
    }

    // 添加 ISR 处理程序
    if (gpio_isr_handler_add(BUTTON_1_GPIO_PIN, button1_isr_handler, NULL) != ESP_OK)
    {
        printf("Failed to add button1 ISR handler\n");
    }

    if (gpio_isr_handler_add(BUTTON_2_GPIO_PIN, button2_isr_handler, NULL) != ESP_OK)
    {
        printf("Failed to add button2 ISR handler\n");
    }
}
