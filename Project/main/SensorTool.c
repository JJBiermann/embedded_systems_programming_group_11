#include "SensorTool.h"



void setupSensors() {
    ESP_ERROR_CHECK(am2320_shared_i2c_init(&dev, I2C_NUM));
    ESP_ERROR_CHECK(adafruit_stemma_soil_sensor_shared_i2c_init());
}

struct SoilData soilPoll() {
    esp_err_t ret = ESP_OK;
    uint16_t moist = 0;
    float temp = 0;
    bool valid = true;

    ret = adafruit_stemma_soil_sensor_read_moisture(I2C_NUM, &moist);
    if (ret != ESP_OK) {
        valid = false;
    }

    ret = adafruit_stemma_soil_sensor_read_temperature(I2C_NUM, &temp);
    if (ret != ESP_OK) {
        valid = false;
    }
    
    struct SoilData data = {
        .valid = valid,
        .moist = moist,
        .temp = temp
    };
    return data;
    
}

int lightPoll() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); //ADC1_CHANNEL_0 is on GPIO0 (GPIOzero)

    int light = adc1_get_raw(ADC1_CHANNEL_0);
    return light;
}   

struct AirData tempHumidPoll() {
    float temp, humid;
    bool valid = true;

    esp_err_t res = am2320_get_rht(&dev, &temp, &humid);
    if (res != ESP_OK) {
        valid = false;
    }

    struct AirData data = {
        .valid = valid,
        .temp = temp,
        .humid = humid
    };
    return data;
}