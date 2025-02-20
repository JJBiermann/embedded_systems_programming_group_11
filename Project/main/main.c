
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_http_client.h"
#include "cJSON.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_rom_gpio.h"
#include "esp_timer.h"

#include "nvs_flash.h"
#include "sdkconfig.h"

//Driver libraries
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/adc.h"

//Display libraies
#include "ssd1306.h"
#include "font8x8_basic.h"
#include "display.h"

//Temperature/humidity sensor library
#include <am2320.h>

//Stemma soil sensor library
#include "Adafruit_Stemma_soil_sensor.h"

#include "SensorTool.h"
#include "RGBTool.h"
#include "WifiTool.h"
#include "RickRollBuzzer.h"
#include "Config.h"
#include "PollConfig.h"

#include "freertos/queue.h"
#include "esp_intr_alloc.h"

#include "hal/wdt_hal.h"
#include "esp_task_wdt.h"

#define I2C_NUM     0
#define SDA_PIN     2
#define SCL_PIN     3
#define I2C_SPEED   100000 

#define I2C_MASTER_FREQ_HZ 75000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_SDA_GPIO 2
#define I2C_MASTER_SCL_GPIO 3

#define ERROR_TAG "ERROR"



static QueueHandle_t displayQueue;
static QueueHandle_t rgbQueue;
static QueueHandle_t dashboardQueue;
static QueueHandle_t buttonOneIntQueue;
static QueueHandle_t RRQueue;

#define BTN_1_GPIO_PIN 18
#define BTN_2_GPIO_PIN 19

typedef struct {
    TimerHandle_t light;
    TimerHandle_t air;
    TimerHandle_t soil;
} TimerHandles;


void IRAM_ATTR buttonOneInterruptHandler(void* args) {
    TimerHandles* timerHandles = (TimerHandles*) args;
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = esp_timer_get_time(); 
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; // TODO: What is this? 
    if (current_time - last_interrupt_time > 200000) {
        if (getPollRate() == FAST_POLL_RATE) {
            xTimerChangePeriodFromISR(timerHandles->light, pdMS_TO_TICKS(SLOW_POLL_RATE), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(timerHandles->air, pdMS_TO_TICKS(SLOW_POLL_RATE), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(timerHandles->soil, pdMS_TO_TICKS(SLOW_POLL_RATE), &xHigherPriorityTaskWoken);
            setPollRate(SLOW_POLL_RATE);
        } else {
            xTimerChangePeriodFromISR(timerHandles->light, pdMS_TO_TICKS(FAST_POLL_RATE), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(timerHandles->air, pdMS_TO_TICKS(FAST_POLL_RATE), &xHigherPriorityTaskWoken);
            xTimerChangePeriodFromISR(timerHandles->soil, pdMS_TO_TICKS(FAST_POLL_RATE), &xHigherPriorityTaskWoken);
            setPollRate(FAST_POLL_RATE);
        }
    }
    last_interrupt_time = current_time;
}

void IRAM_ATTR buttonTwoInterruptHandler(void* args) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = esp_timer_get_time();  // Get time in microseconds
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; 
    int flag = 0;
    if (current_time - last_interrupt_time > 200000) {  // 200 ms debounce time
        flag = !flag;
        xQueueSendFromISR(RRQueue, &flag, &xHigherPriorityTaskWoken);
    }
    last_interrupt_time = current_time;
}

void setupInterruptButtons(TimerHandles* args) {
    gpio_install_isr_service(0);
    gpio_config_t io_conf; 
    io_conf.pin_bit_mask = (1ULL << BTN_1_GPIO_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_1_GPIO_PIN, &buttonOneInterruptHandler, args));

    // Second Button
    io_conf.pin_bit_mask = (1ULL << BTN_2_GPIO_PIN);
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_ERROR_CHECK(gpio_isr_handler_add(BTN_2_GPIO_PIN, &buttonTwoInterruptHandler, (void*) 0));
}

void setupQueues() {
    displayQueue = xQueueCreate(20, sizeof(struct Message*));
    if (displayQueue == NULL) {
        printf("Queue creation went wrong!\n");
    } else {
        printf("Queue creation was successful!\n");
    }
    rgbQueue = xQueueCreate(20, sizeof(struct Message*));
    if (rgbQueue == NULL) {
        printf("Queue creation went wrong!\n");
    } else {
        printf("Queue creation was successful!\n");
    }
    dashboardQueue = xQueueCreate(20, sizeof(struct Message*));
    if (dashboardQueue == NULL) {
        printf("Queue creation went wrong!\n");
    } else {
        printf("Queue creation was successful!\n");
    }
    RRQueue = xQueueCreate(5, sizeof(int));
    if (RRQueue == NULL) {
        printf("Queue creation went wrong!\n");
    } else {
        printf("Queue creation was successful!\n");
    }
}

void checkRRQueue() {
    int flag = 0;
    while(1) {
        if (xQueueReceive(RRQueue, &flag, (TickType_t) 10) == pdTRUE) {
            if (flag) {
                rickroll();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(getPollDelay()));
    }
}

void setupRR() {
    
    
}

void soilPollCB(TimerHandle_t xTimer) {
    struct SoilData soil = soilPoll();
    struct Message* msg1 = (struct Message*) malloc(sizeof(struct Message));
    msg1->mode = 'S';
    msg1->sensorData.soil.temp = soil.temp;
    msg1->sensorData.soil.moist = soil.moist;
    if(xQueueSend(displayQueue, (void*) &msg1, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the soil sensor1!\n");
    }
    struct Message* msg2 = (struct Message*) malloc(sizeof(struct Message));
    msg2->mode = 'S';
    msg2->sensorData.soil.temp = soil.temp;
    msg2->sensorData.soil.moist = soil.moist;
    if(xQueueSend(dashboardQueue, (void*) &msg2, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the soil sensor2!\n");
    }
}

void tempHumidPollCB(TimerHandle_t xTimer) {
    struct AirData air = tempHumidPoll();
    struct Message* msg1 = (struct Message*) malloc(sizeof(struct Message));
    msg1->mode = 'A';
    msg1->sensorData.air.valid = true;
    msg1->sensorData.air.temp = air.temp;
    msg1->sensorData.air.humid = air.humid;
    struct Message* msg2 = (struct Message*) malloc(sizeof(struct Message));
    msg2->mode = 'A';
    msg2->sensorData.air.valid = true;
    msg2->sensorData.air.temp = air.temp;
    msg2->sensorData.air.humid = air.humid;
    struct Message* msg3 = (struct Message*) malloc(sizeof(struct Message));
    msg3->mode = 'A';
    msg3->sensorData.air.valid = true;
    msg3->sensorData.air.temp = air.temp;
    msg3->sensorData.air.humid = air.humid;

    if(xQueueSend(displayQueue, (void*) &msg1, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
    if(xQueueSend(rgbQueue, (void*) &msg2, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
    if(xQueueSend(dashboardQueue, (void*) &msg3, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
}

void lightPollCB(TimerHandle_t xTimer) {
    int light = lightPoll();
    struct Message* msg1 = (struct Message*) malloc(sizeof(struct Message));
    msg1->mode = 'L';
    msg1->sensorData.light = light;
    if(xQueueSend(displayQueue, (void*) &msg1, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the light sensor!\n");
    }
    struct Message* msg2 = (struct Message*) malloc(sizeof(struct Message));
    msg2->mode = 'L';
    msg2->sensorData.light = light;
    if(xQueueSend(dashboardQueue, (void*) &msg2, (TickType_t) 0) != pdTRUE) {
        ESP_LOGE(ERROR_TAG, "There was an error, transmitting data from the light sensor!\n");
    }
}

void app_main(void)
{
    //Initialize common I2C port for display, soil sensor, and temperature/humidity sensor
    //Initialized it as follows only once here in the main, then use the shared_init 
    //functions for the different components as shown in this demo (see _demo functions).
    // NOTE: This block has to be first, since it sets parameters for the sensor init.
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_GPIO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_GPIO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;
    i2c_param_config(I2C_NUM, &conf);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    setup_display();

    setPollRate(FAST_POLL_RATE);

    setupQueues();
    setupSensors();
    setupLED();
    setupWifi();
    setupBuzzer();
    
    TimerHandle_t light_poll, temp_humid_poll, soil_poll;

    light_poll      = xTimerCreate("light_poll", pdMS_TO_TICKS(FAST_POLL_RATE), pdTRUE, (void *)0, lightPollCB);
    temp_humid_poll = xTimerCreate("temp_humid_poll", pdMS_TO_TICKS(FAST_POLL_RATE), pdTRUE, (void *)0, tempHumidPollCB);
    soil_poll       = xTimerCreate("soil_poll", pdMS_TO_TICKS(FAST_POLL_RATE), pdTRUE, (void *)0, soilPollCB);

    TimerHandles* handles = (TimerHandles*) malloc(sizeof(TimerHandles));
    handles->light = light_poll; 
    handles->soil = soil_poll;
    handles->air = temp_humid_poll;

    setupInterruptButtons((void*) handles);

    vTaskDelay(pdMS_TO_TICKS(20000));
    
    xTaskCreate(update_display, "display", TASK_STACK, (void*) displayQueue, 2, NULL);
    xTaskCreate(update_rgbled, "rgbled", TASK_STACK, (void*) rgbQueue, 1, NULL);
    xTaskCreate(post_data, "post_data", TASK_STACK, (void*) dashboardQueue, 3, NULL);
    xTaskCreate(checkRRQueue, "RR", TASK_STACK, (void*) RRQueue, 8, NULL);

    xTimerStart(light_poll, pdMS_TO_TICKS(500));
    vTaskDelay(pdMS_TO_TICKS(FAST_POLL_RATE / 3));
    xTimerStart(temp_humid_poll, pdMS_TO_TICKS(500));
    vTaskDelay(pdMS_TO_TICKS(FAST_POLL_RATE / 3));
    xTimerStart(soil_poll, pdMS_TO_TICKS(500));

    while(1) {
        vTaskDelay(10);
    }
}
