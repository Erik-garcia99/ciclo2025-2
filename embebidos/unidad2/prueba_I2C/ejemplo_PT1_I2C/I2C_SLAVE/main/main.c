#include <stdio.h>
#include <string.h>
#include<esp_log.h>
#include<driver/i2c.h>
#include<driver/gpio.h>
#include <portmacro.h>

static const char *TAG = "slave";

#define LED_PIN GPIO_NUM_2

#define I2C_SLAVE_SCL 22
#define I2C_SLAVE_SDA 21
#define I2C_SLAVE_ADRESS 0x1C
#define I2C_SLAVE_RX_BUF_LEN 1024

#define LED_ON_CMD  "ON"
#define LED_OFF_CDM "OFF"



esp_err_t i2c_slave_init(void){

    i2c_config_t i2c_slave_conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SLAVE_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SLAVE_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,

        .slave.addr_10bit_en = 0,
        .slave.slave_addr =I2C_SLAVE_ADRESS,
    };

    esp_err_t error = i2c_param_config(I2C_NUM_1, &i2c_slave_conf);

    if( error != ESP_OK){
        return error;
    }

    return i2c_driver_install(I2C_NUM_1 , i2c_slave_conf.mode, I2C_SLAVE_RX_BUF_LEN, 0, 0);

}


void app_main(void)
{

    uint8_t rx_buffer[I2C_SLAVE_RX_BUF_LEN] = {0};

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    ESP_ERROR_CHECK(i2c_slave_init());
    ESP_LOGI(TAG, "I2C inicializado");

    while(1){

        if(i2c_slave_read_buffer(I2C_NUM_1, rx_buffer, I2C_SLAVE_RX_BUF_LEN, 100/portTICK_PERIOD_MS) > 0){
           i2c_reset_rx_fifo(I2C_NUM_1);

           if(!strncmp((const char*)rx_buffer, LED_ON_CMD, sizeof(LED_ON_CMD))){
            gpio_set_level(LED_PIN, 1 );
            ESP_LOGI(TAG, "LED ON");
           }


           else if(!strncmp((const char*)rx_buffer, LED_OFF_CDM, sizeof(LED_OFF_CDM))){
                gpio_set_level(LED_PIN, 0 );
                ESP_LOGI(TAG, "LED OFF");
           }

           memset(rx_buffer,0, I2C_SLAVE_RX_BUF_LEN);
        }


    }


}