#include "SensorTool.h"



void setupSensors() {
    ESP_ERROR_CHECK(am2320_shared_i2c_init(&dev, I2C_NUM));
}

void soilPoll() {
    int ret = ESP_OK;
    uint16_t moisture_value = 0;
    float temperature_value = 0;

    //Initialize the sensor (shared i2c) only once after boot.
    ESP_ERROR_CHECK(adafruit_stemma_soil_sensor_shared_i2c_init());


    ret = adafruit_stemma_soil_sensor_read_moisture(I2C_NUM, &moisture_value);

    // if (ret == ESP_OK)
    // {
    //     ESP_LOGI(tag, "Adafruit Stemma sensor value: =%u", moisture_value);
    // }

    ret = adafruit_stemma_soil_sensor_read_temperature(I2C_NUM, &temperature_value);


    // if (ret == ESP_OK)
    // {
    //     ESP_LOGI(tag, "Adafruit Stemma sensor value: =%f", temperature_value);
    // }
        
    
}

void lightPoll(int* light) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); //ADC1_CHANNEL_0 is on GPIO0 (GPIOzero)

    *light = adc1_get_raw(ADC1_CHANNEL_0);
}   

void tempHumidPoll(float* temp, float* humid) {
    esp_err_t res = am2320_get_rht(&dev, temp, humid);
    // if (res == ESP_OK)
    //     ESP_LOGI(tag, "Temperature: %.1f°C, Humidity: %.1f%%", temperature, humidity);
    // else
    //     ESP_LOGE(tag, "Error reading data: %d (%s)", res, esp_err_to_name(res));
}