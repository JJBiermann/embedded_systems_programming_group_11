#include "RGBTool.h"
#include "SensorTool.h"



void setupLED() {
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY, 
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel_red = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_RED,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO_RED,
        .duty           = 0,
        .hpoint         = 0
    };

    ledc_channel_config_t ledc_channel_green = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_GREEN,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO_GREEN,
        .duty           = 0,
        .hpoint         = 0
    };

    ledc_channel_config_t ledc_channel_blue = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_BLUE,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO_BLUE,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_red));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_green));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel_blue));
}


void setRGB(struct RGB rgb) {
    assert(0 <= rgb.r && rgb.r <= 255);
    assert(0 <= rgb.g && rgb.g <= 255);
    assert(0 <= rgb.b && rgb.b <= 255);

    int scale = DUTY_MAX / RGB_RANGE;

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_RED, 8192 - rgb.r * scale));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_GREEN, 8192 - rgb.g * scale));
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_BLUE, 8192 - rgb.b * scale));
    
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_RED));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_GREEN));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_BLUE));
}


void update_rgbled(void* pvParameters) {
    QueueHandle_t rgbQueue = (QueueHandle_t) pvParameters;
    struct Message* msg;
    while(1) {
        if(xQueueReceive(rgbQueue, &msg, (TickType_t) 0) == pdTRUE) {
            if (msg->mode != 'A') {
                ESP_LOGE("ERR", "DID NOT WORK");
            }
            if (msg->sensorData.air.valid) {
                if (15 <= msg->sensorData.air.temp && msg->sensorData.air.temp <= 25) {
                    setRGB(RGB_GREEN);
                } else if ((10 <= msg->sensorData.air.temp && msg->sensorData.air.temp < 15) || (25 < msg->sensorData.air.temp && msg->sensorData.air.temp <= 30)) {
                    setRGB(RGB_YELLOW);
                } else {
                    setRGB(RGB_RED);
                }
            } else {
                setRGB(RGB_LIGHT_BLUE);
            }   
            free(msg);
        } else {
            ESP_LOGW("RGB", "Queue empty.");
        }
        vTaskDelay(pdMS_TO_TICKS(getPollDelay()));
    }

}