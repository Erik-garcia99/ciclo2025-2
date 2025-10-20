#include "driver/i2c.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Definiciones faltantes
#define I2C_MASTER_SCL_IO    22      // Pin SCL
#define I2C_MASTER_SDA_IO    21      // Pin SDA  
#define I2C_MASTER_NUM       I2C_NUM_0  // Puerto I2C número 0
#define I2C_MASTER_FREQ_HZ   100000  // Frecuencia de 100 kHz

#define LM75_ADDR   0x48  // Dirección por defecto del LM75
#define TAG         "LM75"

float read_lm75_temperature() {
    uint8_t data[2] = {0};
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Seleccionar registro de temperatura (0x00)
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LM75_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, 0x00, true); // Registro de temperatura

    // Lectura de 2 bytes
    i2c_master_start(cmd); // Repeated start
    i2c_master_write_byte(cmd, (LM75_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error de lectura I2C: %d", ret);
        return -999.0f;
    }

    // Convertir datos a temperatura
    int16_t temp_raw = (data[0] << 8) | data[1];
    temp_raw >>= 7; // El LM75AB usa los 9 bits más significativos
    return (temp_raw * 0.5f); // Cada LSB = 0.5°C
}

void app_main() {
    // Configuración del I2C
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);

    // Leer temperatura continuamente
    while(1) {
        float temp = read_lm75_temperature();
        if (temp != -999.0f) {
            ESP_LOGI(TAG, "Temperatura: %.2f °C", temp);
        } else {
            ESP_LOGE(TAG, "No se pudo leer el sensor");
        }
        
        vTaskDelay(pdMS_TO_TICKS(2000)); // Esperar 2 segundos
    }
}
