// MASTER

#include <stdio.h>
#include<string.h>
#include<esp_log.h>
#include<driver/i2c.h>


static const char *TAG ="master";

#define I2C_MASTER_SCL 22
#define I2C_MASTER_SDA 21
#define I2C_SLAVE_ADDRESS 0x1C
#define ACK_CHECK_EN 0x1



esp_err_t i2c_master_init(void){

    i2c_config_t i2c_master_conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 400000,
    };

    esp_err_t error = i2c_param_config(I2C_NUM_0,&i2c_master_conf);

    if(error != ESP_OK){
        return error;
    }

    return i2c_driver_install(I2C_NUM_0,i2c_master_conf.mode, 0,0,0);
}


esp_err_t i2c_master_send(uint8_t *data, uint8_t len){

    esp_err_t res;

    //crea un enlace de comando I2C retoran un puntero a la estrucutra que va a contener os comandos I2C a ejecutar 
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    
    //manejador creador con i2c_cmd_link_create
    i2c_master_start(cmd_handle );

    //escribe un byte
    i2c_master_write_byte(cmd_handle, //manejador creador con i2c
        I2C_SLAVE_ADDRESS<< 1 | I2C_MASTER_WRITE, //byte a escribir 
          ACK_CHECK_EN // si se debe esperar un ACK del esclavo);
    );
    i2c_master_write(cmd_handle, data, len, ACK_CHECK_EN);

    i2c_master_stop(cmd_handle);

    //send all queue commands 

    res = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle,1000/portTICK_PERIOD_MS);

    i2c_cmd_link_delete(cmd_handle);
    return res;
}


void app_main(void)
{
    uint8_t led_on[] = "ON";
    uint8_t led_off[] = "OFF";

    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C inicializado");


    while(1){

        i2c_master_send(led_on, sizeof(led_on));
        ESP_LOGI(TAG, "LED on request");
        vTaskDelay(500/portTICK_PERIOD_MS);
        i2c_master_send(led_off, sizeof(led_off));
        ESP_LOGI(TAG, "LED off request");
        vTaskDelay(500/portTICK_PERIOD_MS);


    }
}