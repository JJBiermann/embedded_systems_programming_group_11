
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

#include "freertos/queue.h"


#define I2C_NUM    0
#define SDA_PIN    2
#define SCL_PIN    3
#define I2C_SPEED   100000  // 100kHz speed

#define I2C_MASTER_FREQ_HZ 75000 //Reduce it to 50000 if the temperature/umidity sensor fails
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_SDA_GPIO 2
#define I2C_MASTER_SCL_GPIO 3
// #define I2C_NUM 0

#define ERROR_TAG "ERROR"

static union SensorDataUnion data;
static QueueHandle_t displayQueue;

void setupQueues() {
    displayQueue = xQueueCreate(10, sizeof(struct Message));

    if (displayQueue == NULL) {
        printf("Queue creation went wrong!\n");
    } else {
        printf("Queue creation was successful!\n");
    }
}

void soilPollCB(TimerHandle_t xTimer) {
    struct SoilData soil = soilPoll();
    struct Message* msg = (struct Message*) malloc(sizeof(struct Message));
    msg->mode = 'S';
    msg->sensorData.soil = soil;
    if(xQueueSend(displayQueue, (void*) msg, (TickType_t) 1000) != pdTRUE) {
        ESP_LOGI(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
}

void tempHumidPollCB(TimerHandle_t xTimer) {
    struct AirData air = tempHumidPoll();
    struct Message* msg = (struct Message*) malloc(sizeof(struct Message));
    msg->mode = 'A';
    msg->sensorData.air = air;
    if(xQueueSend(displayQueue, (void*) msg, (TickType_t) 1000) != pdTRUE) {
        ESP_LOGI(ERROR_TAG, "There was an error, transmitting data from the air sensor!\n");
    }
}

void lightPollCB(TimerHandle_t xTimer) {
    int light = lightPoll();
    struct Message* msg = (struct Message*) malloc(sizeof(struct Message));
    msg->mode = 'L';
    msg->sensorData.light = light;
    if(xQueueSend(displayQueue, (void*) msg, (TickType_t) 1000) != pdTRUE) {
        ESP_LOGI(ERROR_TAG, "There was an error, transmitting data from the light sensor!\n");
    }
}

void display() {
    printf("Light: %d, ", data.light);
    if (data.soil.valid) {
        printf("Soil Moisture: %d, Soil Temp.: %.1f, ", data.soil.moist, data.soil.temp);
    } else {
        printf("Soil Moisture: NA, Soil Temp.: NA, ");
    }
    if (data.air.valid) {
        printf("Air Temp.: %.1f, Air Humidity: %.1f\n", data.air.temp, data.air.humid);
    } else {
        printf("Air Temp.: NA, Air Humidity: NA\n");
    }
    
    if (data.air.valid) {
        if (15 <= data.air.temp && data.air.temp <= 25) {
            setRGB(RGB_GREEN);
        } else if ((10 <= data.air.temp && data.air.temp < 15) || (25 < data.air.temp && data.air.temp <= 30)) {
            setRGB(RGB_YELLOW);
        } else {
            setRGB(RGB_RED);
        }
    } else {
        setRGB(RGB_LIGHT_BLUE);
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

    setupQueues();
    setupSensors();
    setupLED();
    setup_display();
    //setupWifi();

    TimerHandle_t light_poll, temp_humid_poll, soil_poll;
    TaskHandle_t display_task_handle = NULL;

    light_poll      = xTimerCreate("light_poll", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, lightPollCB);
    temp_humid_poll = xTimerCreate("temp_humid_poll", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, tempHumidPollCB);
    soil_poll       = xTimerCreate("soil_poll", pdMS_TO_TICKS(1000), pdTRUE, (void *)0, soilPollCB);

    xTaskCreate(update_display, "display", 8192, (void*) displayQueue, 10, &display_task_handle);
    //xTaskCreate(&postData, "post_data", 8192, (void*) &data, 5, NULL);
    
    xTimerStart(light_poll, pdMS_TO_TICKS(500));
    xTimerStart(temp_humid_poll, pdMS_TO_TICKS(500));
    xTimerStart(soil_poll, pdMS_TO_TICKS(500));
}
