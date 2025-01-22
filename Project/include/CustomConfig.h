#ifndef CUSTOM_CONFIG
#define CUSTOM_CONFIG

#define I2C_NUM    0
#define SDA_PIN    2
#define SCL_PIN    3
#define I2C_SPEED   100000  // 100kHz speed
#define I2C_MASTER_FREQ_HZ 75000 //Reduce it to 50000 if the temperature/umidity sensor fails
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_SDA_GPIO 2
#define I2C_MASTER_SCL_GPIO 3

#define ERROR_TAG "ERROR"

// Consumers
#define DISPLAY_ACTIVE
#define DASHBOARD_ACTIVE
#define RGB_ACTIVE

// Producers
#define LIGHT_SENSOR_ACTIVE
#define SOIL_SENSOR_ACTIVE
#define AIR_SENSOR_ACTIVE

//Polling Rate
#define STANDARD_POLLING 20000
#define FAST_POLLING 20000
#define SLOW_POLLING 60000

// sensor polling delays
#define AIR_SENSOR_POLLING_DELAY pdMS_TO_TICKS(SLOW_POLLING)
#define SOIL_SENSOR_POLLING_DELAY pdMS_TO_TICKS(SLOW_POLLING)
#define LIGHT_SENSOR_POLLING_DELAY pdMS_TO_TICKS(SLOW_POLLING)

//vTaskDelays
#define ONE_SEC  pdMS_TO_TICKS(1000)
#define TWO_SECS pdMS_TO_TICKS(2000)
#define TEN_SECS pdMS_TO_TICKS(10000)

//Operation Mode
#define LONG_DELAYS
// #define SHORT_DELAYS

#ifdef LONG_DELAYS
#define DELAY TWO_SECS
#endif
#ifdef SHORT_DELAYS
#define DELAY ONE_SEC
#endif



#endif