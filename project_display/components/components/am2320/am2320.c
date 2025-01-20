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
 * @file am2320.c
 *
 * ESP-IDF driver for humidty/temperature sensors AM2320
 *
 * Copyright (c) 2023 Ruslan V. Uss <unclerus@gmail.com>
 *
 * BSD Licensed as described in the file LICENSE
 */
#include "am2320.h"
#include <math.h>
#include <esp_log.h>
#include <string.h>
#include "driver/i2c.h"

#define I2C_FREQ_HZ (100000) // 100kHz

static const char *TAG = "am2320";

#define MODBUS_READ (0x03)

#define REG_RH_H     (0x00)
#define REG_T_H      (0x02)
#define REG_MODEL_H  (0x08)
#define REG_VER      (0x0a)
#define REG_DEV_ID_H (0x0b)

//#define DELAY_T1_US (800 + 100) // minimum delay + extra
//#define DELAY_T2_US (1500 + 100)

#define DELAY_T1 (2)
#define DELAY_T2 (3)

#define CONFIG_I2CDEV_TIMEOUT 1000 //ms

#define CHECK(x) do { esp_err_t __; if ((__ = x) != ESP_OK) return __; } while (0)
#define CHECK_ARG(VAL) do { if (!(VAL)) return ESP_ERR_INVALID_ARG; } while (0)

static uint16_t crc16(uint8_t *data, size_t len)
{
    uint16_t crc = 0xffff;
    while (len--)
    {
        crc ^= *data++;
        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x01)
            {
                crc >>= 1;
                crc ^= 0xa001;
            } else crc >>= 1;
        }
    }
    return crc;
}

//------------------------------------
esp_err_t i2c_dev_probe(const i2c_dev_t *dev, i2c_dev_type_t operation_type)
{
    if (!dev) return ESP_ERR_INVALID_ARG;

    //esp_err_t res = i2c_setup_port(dev);
    esp_err_t res = ESP_OK;
    if (res == ESP_OK)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, dev->addr << 1 | (operation_type == I2C_DEV_READ ? 1 : 0), true);
        i2c_master_stop(cmd);

        res = i2c_master_cmd_begin(dev->port, cmd, pdMS_TO_TICKS(CONFIG_I2CDEV_TIMEOUT));

        i2c_cmd_link_delete(cmd);
    }

    return res;
}

esp_err_t i2c_dev_read(const i2c_dev_t *dev, const void *out_data, size_t out_size, void *in_data, size_t in_size)
{
    if (!dev || !in_data || !in_size) return ESP_ERR_INVALID_ARG;

    //esp_err_t res = i2c_setup_port(dev);
    esp_err_t res = ESP_OK;
    if (res == ESP_OK)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (out_data && out_size)
        {
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, dev->addr << 1, true);
            i2c_master_write(cmd, (void *)out_data, out_size, true);
        }
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (dev->addr << 1) | 1, true);
        i2c_master_read(cmd, in_data, in_size, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);

        res = i2c_master_cmd_begin(dev->port, cmd, pdMS_TO_TICKS(CONFIG_I2CDEV_TIMEOUT));
        if (res != ESP_OK)
            ESP_LOGE(TAG, "Could not read from device [0x%02x at %d]: %d (%s)", dev->addr, dev->port, res, esp_err_to_name(res));

        i2c_cmd_link_delete(cmd);
    }

    return res;
}

esp_err_t i2c_dev_write(const i2c_dev_t *dev, const void *out_reg, size_t out_reg_size, const void *out_data, size_t out_size)
{
    if (!dev || !out_data || !out_size) return ESP_ERR_INVALID_ARG;

    //esp_err_t res = i2c_setup_port(dev);
    esp_err_t res = ESP_OK;
    if (res == ESP_OK)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, dev->addr << 1, true);
        if (out_reg && out_reg_size)
            i2c_master_write(cmd, (void *)out_reg, out_reg_size, true);
        i2c_master_write(cmd, (void *)out_data, out_size, true);
        i2c_master_stop(cmd);
        res = i2c_master_cmd_begin(dev->port, cmd, pdMS_TO_TICKS(CONFIG_I2CDEV_TIMEOUT));
        if (res != ESP_OK)
            ESP_LOGE(TAG, "Could not write to device [0x%02x at %d]: %d (%s)", dev->addr, dev->port, res, esp_err_to_name(res));
        i2c_cmd_link_delete(cmd);
    }
    return res;
}

static void delay_ms(int ms)
{
    vTaskDelay((ms) / portTICK_PERIOD_MS);
}

//------------------------------------
/*
 * Request: [3 bytes] CMD, START_REG, BYTES
 * Response: [BYTES + 4] CMD, BYTES, DATA0, ... DATAn, CRC16_LOW, CRC16_HIGH
 */
static esp_err_t read_reg_modbus(i2c_dev_t *dev, uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t req[] = { MODBUS_READ, reg, len };
    uint8_t resp[len + 4];
    esp_err_t err = ESP_FAIL;

    /* Wake up the sensor. See 8.2.4 I2C Communication Timing */
    err = i2c_dev_probe(dev, I2C_DEV_READ);
    if (err == ESP_FAIL)
    {
        /* the sensor does not send ACK for wakeup command, ignore the error
         */
        ESP_LOGD(TAG, "i2c_dev_probe(): %s", esp_err_to_name(err));
    }
    delay_ms(DELAY_T1);

    err =  i2c_dev_write(dev, NULL, 0, req, sizeof(req));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2c_dev_write(): %s", esp_err_to_name(err));
        goto fail;
    }
    delay_ms(DELAY_T2);

    err = i2c_dev_read(dev, NULL, 0, resp, sizeof(resp));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "i2c_dev_read(): %s", esp_err_to_name(err));
        goto fail;
    }

    if (resp[0] != MODBUS_READ)
    {
        ESP_LOGE(TAG, "Invalid MODBUS reply (%d != 0x03)", resp[0]);
        err = ESP_ERR_INVALID_RESPONSE;
        goto fail;
    }
    if (resp[1] != len)
    {
        ESP_LOGE(TAG, "Invalid MODBUS reply length (%d != %d)", resp[1], len);
        err = ESP_ERR_INVALID_RESPONSE;
        goto fail;
    }

    /* CRC16 in little endian */
    if (crc16(resp, len + 2) != ((uint16_t)resp[len + 3] << 8) + resp[len + 2])
    {
        ESP_LOGE(TAG, "Invalid CRC in MODBUS reply");
        err = ESP_ERR_INVALID_CRC;
        goto fail;
    }
    memcpy(buf, resp + 2, len);

fail:
    return err;
}

static float convert_temperature(uint16_t raw)
{
    if (raw == 0xffff)
        return NAN;
    float res = raw & 0x8000
        ? (float)-(int16_t)(raw & 0x7fff)
        : (float)(raw);
    return res / 10.0f;
}

static inline float convert_humidity(uint16_t raw)
{
    return raw != 0xffff
        ? (float)raw / 10.0f
        : NAN;
}

///////////////////////////////////////////////////////////////////////////////

esp_err_t am2320_shared_i2c_init(i2c_dev_t *dev, i2c_port_t port)
{
    dev->port = port;
    dev->addr = AM2320_I2C_ADDR;
    dev->timeout_ticks = pdMS_TO_TICKS(CONFIG_I2CDEV_TIMEOUT);
    return ESP_OK;
}

esp_err_t am2320_get_rht(i2c_dev_t *dev, float *temperature, float *humidity)
{
    CHECK_ARG(dev && (temperature || humidity));

    if (temperature && humidity)
    {
        // read both values
        uint8_t buf[4] = { 0xff, 0xff, 0xff, 0xff };
        CHECK(read_reg_modbus(dev, REG_RH_H, 4, buf));
        *humidity = convert_humidity(((uint16_t)buf[0] << 8) + buf[1]);
        *temperature = convert_temperature(((uint16_t)buf[2] << 8) + buf[3]);
    }
    else if (temperature)
    {
        // read only temperature
        uint8_t buf[2] = { 0xff, 0xff };
        CHECK(read_reg_modbus(dev, REG_T_H, 2, buf));
        *temperature = convert_temperature(((uint16_t)buf[0] << 8) + buf[1]);
    }
    else
    {
        // read only humidity
        uint8_t buf[2] = { 0xff, 0xff };
        CHECK(read_reg_modbus(dev, REG_RH_H, 2, buf));
        *humidity = convert_humidity(((uint16_t)buf[0] << 8) + buf[1]);
    }

    return ESP_OK;
}

esp_err_t am2320_get_model(i2c_dev_t *dev, uint16_t *model)
{
    CHECK_ARG(dev && model);

    uint8_t buf[2] = { 0 };
    CHECK(read_reg_modbus(dev, REG_T_H, 2, buf));
    *model = ((uint16_t)buf[0] << 8) + buf[1];

    return ESP_OK;
}

esp_err_t am2320_get_version(i2c_dev_t *dev, uint8_t *version)
{
    CHECK_ARG(dev && version);

    CHECK(read_reg_modbus(dev, REG_VER, 1, version));

    return ESP_OK;
}

esp_err_t am2320_get_device_id(i2c_dev_t *dev, uint32_t *id)
{
    CHECK_ARG(dev && id);

    uint8_t buf[4] = { 0 };
    CHECK(read_reg_modbus(dev, REG_DEV_ID_H, 4, buf));
    *id = ((uint32_t)buf[0] << 24) + ((uint32_t)buf[1] << 16) + ((uint32_t)buf[2] << 8) + buf[3];

    return ESP_OK;
}
