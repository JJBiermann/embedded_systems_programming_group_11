/*
 * A components demo for course 02112
 */
// configTIMER_TASK_PRIORITY 

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include <string.h>
#include "esp_log.h"

// Driver libraries
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
// #include "driver/adc.h"

// Display libraies
#include "ssd1306.h"
#include "font8x8_basic.h"

// Temperature/humidity sensor library
#include <am2320.h>

// Stemma soil sensor library
#include "Adafruit_Stemma_soil_sensor.h"

#include "SensorTool.h"

// New include
#include "led_buzzer.h"
#include "esp_task_wdt.h"

#define tag "EXAMPLE_ALL"

#define RED_LED_GPIO 8
#define BUTTON_1_GPIO_PIN 18
#define BUTTON_2_GPIO_PIN 19

#define I2C_MASTER_FREQ_HZ 75000 // Reduce it to 50000 if the temperature/umidity sensor fails
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_SDA_GPIO 2
#define I2C_MASTER_SCL_GPIO 3
#define I2C_NUM 0

static struct SensorData data;
static SSD1306_t oled_display;

// This area for display
void setup_display()
{
    // int top = 0;

    // Initialize the display (shared i2c) only once after boot.
    i2c_master_shared_i2c_init(&oled_display);

    // Uncomment this if you want to flip the display
    // dev._flip = true;

    ssd1306_init(&oled_display, 128, 64);
}

void soilPollCB(TimerHandle_t xTimer)
{
    struct SoilData soil = soilPoll();
    data.soil = soil;
}

void tempHumidPollCB(TimerHandle_t xTimer)
{
    struct AirData air = tempHumidPoll();
    data.air = air;
}

void lightPollCB(TimerHandle_t xTimer)
{
    int light = lightPoll();
    data.light = light;
    // printf("Light sensor: %d\n", light);
}

void display()
{
    printf("Light: %d, ", data.light);
    if (data.soil.valid)
    {
        printf("Soil Moisture: %d, Soil Temp.: %.1f, ", data.soil.moist, data.soil.temp);
    }
    else
    {
        printf("Soil Moisture: NA, Soil Temp.: NA, ");
    }
    if (data.air.valid)
    {
        printf("Air Temp.: %.1f, Air Humidity: %.1f\n", data.air.temp, data.air.humid);
    }
    else
    {
        printf("Air Temp.: NA, Air Humidity: NA\n");
    }
}

// This area for display
// Add the display task here
void oled_display_task(void *pvParameters)
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    esp_task_wdt_add(NULL);
    struct SensorData *data = (struct SensorData *)pvParameters;

    // Set up OLED
    setup_display();
    // Clear screen
    ssd1306_clear_screen(&oled_display, false);
    

    while (1)
    {
        // esp_task_wdt_reset();
        //  Build string to display
        char lightStr[32], soilMoistStr[32], soilTempStr[32];
        char airTempStr[32], airHumidStr[32];

        snprintf(lightStr, sizeof(lightStr), "Light: %d", data->light);

        if (data->soil.valid)
        {
            snprintf(soilMoistStr, sizeof(soilMoistStr), "Soil Moist: %d", data->soil.moist);
            snprintf(soilTempStr, sizeof(soilTempStr), "Soil Temp: %.1f", data->soil.temp);
        }
        else
        {
            snprintf(soilMoistStr, sizeof(soilMoistStr), "Soil Moist: NA");
            snprintf(soilTempStr, sizeof(soilTempStr), "Soil Temp: NA");
        }

        if (data->air.valid)
        {
            snprintf(airTempStr, sizeof(airTempStr), "Air Temp: %.2f", data->air.temp);
            snprintf(airHumidStr, sizeof(airHumidStr), "Air Humid: %.2f", data->air.humid);
        }
        else
        {
            snprintf(airTempStr, sizeof(airTempStr), "Air Temp: NA");
            snprintf(airHumidStr, sizeof(airHumidStr), "Air Humid: NA");
        }
        printf("Formatted Light String: %s\n", lightStr); // Debug output
        // Display info on OLED
        ssd1306_clear_screen(&oled_display, false);//Add the clear screen here to avoid string residue.
        ssd1306_display_text(&oled_display, 3, lightStr, strlen(lightStr), false);
        ssd1306_display_text(&oled_display, 0, soilMoistStr, strlen(soilMoistStr), false);
        ssd1306_display_text(&oled_display, 1, soilTempStr, strlen(soilTempStr), false);
        ssd1306_display_text(&oled_display, 4, airTempStr, strlen(airTempStr), false);
        ssd1306_display_text(&oled_display, 5, airHumidStr, strlen(airHumidStr), false);
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void warning_task(void *pvParameters)
{
    vTaskDelay(500 / portTICK_PERIOD_MS);
    esp_task_wdt_add(NULL);
    struct SensorData *data = (struct SensorData *)pvParameters;
    setup_led();
    setup_buzzer();
    while (1)
    {
        //For not emergency warning: air info, light info, we use led to show
        if ((data->air.temp >= 15.0 && data->air.temp <= 35.0) && (data->light <= 1000.0))
        {
            // led
            led_green();
            // buzzer
            //buzzer_off();
        }
        else // set LED Red, Buzzer On
        {
            // led
            led_red();
            // buzzer
            //buzzer_on();
        }
        //For emergency warning: soil moist, temp, we use buzzer to show
        if ((data->soil.moist >= 1.0 && data->soil.moist <= 1500.0)&&(data->soil.temp>=25 && data->soil.temp<=35)) 
        {
            buzzer_off();
        }
        else
        {
            buzzer_on();
        }
        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    // Initialize common I2C port for display, soil sensor, and temperature/humidity sensor
    // Initialized it as follows only once here in the main, then use the shared_init
    // functions for the different components as shown in this demo (see _demo functions).
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
    TaskHandle_t OLED_task_handle, warning_task_handle;
    UBaseType_t istack_OLED, istack_warning;
    setupSensors();

    TimerHandle_t light_poll, temp_humid_poll, soil_poll;
    light_poll = xTimerCreate("light_poll", pdMS_TO_TICKS(5000), pdTRUE, (void *)0, lightPollCB);               // poll light sensor every 5s
    temp_humid_poll = xTimerCreate("temp_humid_poll", pdMS_TO_TICKS(6000), pdTRUE, (void *)0, tempHumidPollCB); // poll temp_humid sensor every 5s
    soil_poll = xTimerCreate("soil_poll", pdMS_TO_TICKS(10000), pdTRUE, (void *)0, soilPollCB);                 // poll soil every 10s

    xTimerStart(light_poll, pdMS_TO_TICKS(500));
    xTimerStart(temp_humid_poll, pdMS_TO_TICKS(500));
    xTimerStart(soil_poll, pdMS_TO_TICKS(500));

    xTaskCreate(oled_display_task, "OLED Display Task", 4096, &data, 1, &OLED_task_handle); // OLED display task created change stack from 2048 to 3072
    xTaskCreate(warning_task, "warning_task", 4096, &data, 2, &warning_task_handle);

    while (1)
    {
        display();
        istack_OLED = uxTaskGetStackHighWaterMark(OLED_task_handle);
        istack_warning = uxTaskGetStackHighWaterMark(warning_task_handle);
        printf("Display thread stack using: %d\n", istack_OLED);
        printf("Warning thread stack using: %d\n", istack_warning);
        //esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    // fflush(stdout);

    // vTaskDelay(pdMS_TO_TICKS(5000));  // Delay for 1 second

    // This would automatically restart the ESP32
    //  esp_restart();
}