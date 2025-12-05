


#include <stdio.h>
#include<string.h>

//librerias freertos
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#include<freertos/event_groups.h>


//drivers 
#include<driver/i2c.h>
#include<driver/i2c_types.h>
#include<driver/uart.h>

//controlador de erroes
#include<esp_err.h>
#include<esp_log.h>
#include <portmacro.h>


//I2C
#define SDA 22
#define SCL 21
#define SPEED 100000
#define ADDR_LM75AB 0x48
#define NUM_I2C I2C_NUM_0

#define ACK 0x1

//uart
#define UART_SEL UART_NUM_0
#define BUFFER 1024

static const char *TAG ="ESP-I2C";

esp_err_t init_i2c_master(i2c_port_t i2c, int sda_num, int scl_num,uint32_t speed);

typedef struct{
    int8_t integer;
    uint8_t decimal;
}temperatur_t;


bool read_sens_tmp(temperatur_t *tmp);

//tarea 

void main_task(void *params);

void app_main(void)
{
    esp_err_t ret;

    ret = init_i2c_master(NUM_I2C, SDA, SCL, SPEED);

    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "error al inicar master");
    }

    xTaskCreate(main_task,"main_task", 2048, NULL, 10, NULL);


}


void main_task(void *params){

    temperatur_t tmep;

    while(1){

        if(read_sens_tmp(&tmep)){

            uint8_t buff[10];
            sprintf(buff, "%d", tmep.decimal);
            

        }

    }

}

esp_err_t init_i2c_master(i2c_port_t i2c, int sda_num, int scl_num,uint32_t speed){

    i2c_config_t config ={

        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_num,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl_num,
        .scl_pullup_en=GPIO_PULLUP_ENABLE,
        .master.clk_speed=speed,
        .clk_flags=0,
    };
    esp_err_t ret;
    ret = i2c_param_config(i2c, &config);
    if(ret!=ESP_OK){

        ESP_LOGE(TAG, "error configurar los parametros: %s", esp_err_to_name(ret));
    }

    ret = i2c_driver_install(i2c, config.mode, 0,0,0);
    
    if(ret!=ESP_OK){

        ESP_LOGE(TAG, "error al instalar los drivers: %s", esp_err_to_name(ret));
    }
}

bool read_sens_tmp(temperatur_t *tmp){

    uint8_t reg_addr = 0x00;
    uint8_t data[2];
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ADDR_LM75AB << 1 ) | I2C_MASTER_WRITE, ACK);
    i2c_master_write_byte(cmd, reg_addr, ACK);
    //renincianmos para ller 
    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (ADDR_LM75AB << 1) | I2C_MASTER_READ, ACK);
    i2c_master_read_byte(cmd, &data[0], I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &data[1], I2C_MASTER_NACK);

    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(NUM_I2C, cmd, 1000/portTICK_PERIOD_MS);

    if(ret!=ESP_OK){

        ESP_LOGE(TAG, "error al leer el sensor %s", esp_err_to_name(ret));
    }

    // int16_t temperarure = (data[0] << 8) | data[1];
    
    tmp->integer = data[0];
    uint8_t temp_decimal = (data[1] & 0x80) >> 7;

    tmp->decimal = temp_decimal * 5;

}


