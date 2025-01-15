/*
 * Copyright (c) 2023 Ruslan V. Uss <unclerus@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of itscontributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file am2320.h
 * @defgroup am2320 am2320
 * @{
 *
 * ESP-IDF driver for humidty/temperature sensors AM2320
 *
 * Copyright (c) 2023 Ruslan V. Uss <unclerus@gmail.com>
 *
 * BSD Licensed as described in the file LICENSE
 */
#ifndef __AM2320_H__
#define __AM2320_H__

#include "driver/i2c.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AM2320_I2C_ADDR (0x5c)

/**
 * I2C device descriptor
 */
typedef struct
{
    i2c_port_t port;         //!< I2C port number
    uint8_t addr;            //!< Unshifted address
    uint32_t timeout_ticks;  /*!< HW I2C bus timeout (stretch time), in ticks. 80MHz APB clock
                                  ticks for ESP-IDF, CPU ticks for ESP8266.
                                  When this value is 0, I2CDEV_MAX_STRETCH_TIME will be used */
} i2c_dev_t;

/**
 * I2C transaction type
 */
typedef enum {
    I2C_DEV_WRITE = 0, /**< Write operation */
    I2C_DEV_READ       /**< Read operation */
} i2c_dev_type_t;

/**
 * @brief Initialize the I2C device descriptor in shared mode
 *
 * @param dev      Device descriptor
 * @param port     I2C port
 * @return `ESP_OK` on success
 */
esp_err_t am2320_shared_i2c_init(i2c_dev_t *dev, i2c_port_t port);

/**
 * @brief Get temperature and relative humidity
 *
 * @param dev Device descriptor
 * @param[out] temperature Temperature, degrees Celsius
 * @param[out] humidity    Relative humidity, percents
 * @return `ESP_OK` on success
 */
esp_err_t am2320_get_rht(i2c_dev_t *dev, float *temperature, float *humidity);

/**
 * @brief Get device model ID
 *
 * @param dev Device descriptor
 * @param[out] model Device model ID
 * @return `ESP_OK` on success
 */
esp_err_t am2320_get_model(i2c_dev_t *dev, uint16_t *model);

/**
 * @brief Get device version
 *
 * @param dev Device descriptor
 * @param[out] version Device version
 * @return `ESP_OK` on success
 */
esp_err_t am2320_get_version(i2c_dev_t *dev, uint8_t *version);

/**
 * @brief Get device ID
 *
 * @param dev Device descriptor
 * @param[out] id Device ID
 * @return `ESP_OK` on success
 */
esp_err_t am2320_get_device_id(i2c_dev_t *dev, uint32_t *id);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif /* __AM2320_H__ */
