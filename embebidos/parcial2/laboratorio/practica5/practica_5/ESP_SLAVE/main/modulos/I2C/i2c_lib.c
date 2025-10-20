
#include"modulos/I2C/i2c_lib.h"
// #include<driver/i2c.h>
// #include<driver/i2c_master.h>
// #include<driver/i2c_slave.h>
#include<esp_log.h>
#include<driver/i2c.h>
#include <driver/i2c_types.h>

static const char *TAG ="I2C SLAVE";

#define I2C_SLAVE_RX_BUF_LEN 256
#define I2C_SLAVE_TX_BUF_LEN 256

void i2c_master_init(i2c_port_num_t num_i2c, gpio_num_t pin_sda, gpio_num_t pin_scl, uint32_t speed) {

    i2c_config_t conf_master = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = pin_sda,
        .scl_io_num = pin_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = speed,
    };

    esp_err_t ret = i2c_param_config(num_i2c, &conf_master);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C master param config failed: %s", esp_err_to_name(ret));
    }

    ret = i2c_driver_install(num_i2c, conf_master.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C master driver install failed: %s", esp_err_to_name(ret));
    }

    ESP_LOGI(TAG, "Bus I2C maestro para sensor inicializado correctamente");
}


void i2C_slave_init(i2c_port_num_t num_i2c, gpio_num_t pin_sda, gpio_num_t pin_scl, uint8_t slave_addr){
    
    i2c_config_t conf_slave = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = pin_sda,
        .scl_io_num = pin_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = slave_addr,
    };

    esp_err_t ret = i2c_param_config(num_i2c, &conf_slave);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C slave param config failed: %s", esp_err_to_name(ret));
    }

    ret = i2c_driver_install(num_i2c, conf_slave.mode, I2C_SLAVE_RX_BUF_LEN, I2C_SLAVE_TX_BUF_LEN, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C slave driver install failed: %s", esp_err_to_name(ret));
    }
    
    ESP_LOGI(TAG, "I2C esclavo inicializado en dirección 0x%02X", slave_addr);
}


