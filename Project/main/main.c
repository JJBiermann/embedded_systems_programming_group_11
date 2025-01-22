
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"

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

#include "TaskSetup.h"
#include "WifiTool.h"
#include "ButtonTool.h"

//#include "gpio_types.h"
#include "esp_intr_alloc.h"
#include "esp_timer.h"
#include "hal/wdt_hal.h"
#include "esp_task_wdt.h"

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

    #ifdef DASHBOARD_ACTIVE
    // This will inside of the success callback then call "setupQueuesAndTasks()"
    setupWifi(); 
    #else
    setupQueuesAndTasks();
    #endif

    setupInterruptButton();
    setupInterruptButtonTwo();
}
