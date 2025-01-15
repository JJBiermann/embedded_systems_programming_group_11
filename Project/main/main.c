/*
 * A components demo for course 02112
 */
#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"

//Driver libraries
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/adc.h"

//Display libraies
#include "ssd1306.h"
#include "font8x8_basic.h"

//Temperature/humidity sensor library
#include <am2320.h>

//Stemma soil sensor library
#include "Adafruit_Stemma_soil_sensor.h"

#include "SensorTool.h"
#include "RGBTool.h"




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



static struct SensorData data;



void soilPollCB(TimerHandle_t xTimer) {
    struct SoilData soil = soilPoll();
    data.soil = soil;
}

void tempHumidPollCB(TimerHandle_t xTimer) {
    struct AirData air = tempHumidPoll();
    data.air = air;
}


void lightPollCB(TimerHandle_t xTimer) {
    int light = lightPoll();
    data.light = light;
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
    setupSensors();
    setupLED();
    
    TimerHandle_t light_poll, temp_humid_poll, soil_poll;

    light_poll = xTimerCreate("light_poll", pdMS_TO_TICKS(3000), pdTRUE, (void *)0, lightPollCB);                   // poll light sensor every 5s
    temp_humid_poll = xTimerCreate("temp_humid_poll", pdMS_TO_TICKS(6000), pdTRUE, (void *)0, tempHumidPollCB);     // poll temp_humid sensor every 5s
    soil_poll = xTimerCreate("soil_poll", pdMS_TO_TICKS(10000), pdTRUE, (void *)0, soilPollCB);                     // poll soil every 10s
    
    xTimerStart(light_poll, pdMS_TO_TICKS(500));
    xTimerStart(temp_humid_poll, pdMS_TO_TICKS(500));
    xTimerStart(soil_poll, pdMS_TO_TICKS(500));

    //Initialize common I2C port for display, soil sensor, and temperature/humidity sensor
    //Initialized it as follows only once here in the main, then use the shared_init 
    //functions for the different components as shown in this demo (see _demo functions).
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

    while (1) {
        display();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // fflush(stdout);

    // vTaskDelay(pdMS_TO_TICKS(5000));  // Delay for 1 second

    //This would automatically restart the ESP32
    // esp_restart();
}

