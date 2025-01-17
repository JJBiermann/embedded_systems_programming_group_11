#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H
/*
 * A components demo for course 02112
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include <string.h>
#include "esp_log.h"

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


#define I2C_NUM 0

static i2c_dev_t dev = {0};


struct AirData {
    bool valid;
    float temp;
    float humid;
};

struct SoilData {
    bool valid;
    uint16_t moist;
    float temp;
};

union SensorDataUnion {
    int light;
    struct AirData air;
    struct SoilData soil;
};


struct SensorData {
    int light;
    struct AirData air;
    struct SoilData soil;
};

struct Message {
    char mode; // can be L=light, S=soil or A=Air
    union SensorDataUnion sensorData; // has to be read, according to the mode it is operating in.
};

void setupSensors();
int lightPoll();
struct AirData tempHumidPoll();
struct SoilData soilPoll();




#endif