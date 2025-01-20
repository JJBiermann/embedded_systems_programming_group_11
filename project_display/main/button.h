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
#include "driver/adc.h"

// Display libraies
#include "ssd1306.h"
#include "font8x8_basic.h"

// Temperature/humidity sensor library
#include <am2320.h>

// Stemma soil sensor library
#include "Adafruit_Stemma_soil_sensor.h"
#include "esp_intr_alloc.h"
extern volatile int button1flag;
extern volatile int button2flag;
void setup_Button();