#include <stdio.h>
#include<esp_log.h>
#include<driver/i2c.h>


static const char *TAG = "MASTER";

#define I2C_MASTER_SCL_IO 22
#define I2C_MASTER_SDA_IO 21
#define I2C_MASTER_NUM 0
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TIMEOUT_MS 1000
#define I2C_SLAVE_ADDR 0x04


static esp_err_t i2c_master_init(void){

    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf ={
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en =GPIO_PULLUP_ENABLE,
        .master.clk_speed= I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE,0);

}


static esp_err_t device_read(uint8_t *data, size_t len){
    return i2c_master_read_from_device(I2C_MASTER_NUM,I2C_SLAVE_ADDR, data, len,I2C_MASTER_TIMEOUT_MS/portTICK_PERIOD_MS);
}




void app_main(void)
{

    uint8_t data;
    esp_err_t ret;
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized sccessfully");


    while(1){
        if((ret = device_read(&data, 1)) == ESP_OK){
            ESP_LOGI(TAG, "data read = %X", data);
        }
        else{
            ESP_LOGI(TAG, "read error : %X", ret);
        }

        vTaskDelay(100/portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C unitialized successfully");

}