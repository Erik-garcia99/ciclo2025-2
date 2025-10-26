#include"i2c_lib.h"

//#include<driver/i2c.h>
#include<driver/i2c_master.h>

#include<freertos/FreeRTOS.h>
#include<driver/i2c_types.h>
#include<esp_log.h>

#define ACK_ENABLE 0x1


const char *TAG = "I2C_ESP_MASTER";

/*
esp_err_t i2c_master_init(i2c_port_t port_num,int sda_pin, int scl_pin, uint32_t speed){

    esp_err_t ret;

    i2c_config_t i2c_master ={

        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl_pin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = speed,
    };

    ret = i2c_param_config(port_num, &i2c_master);

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "erro al configurar los parametros:  %s", esp_err_to_name(ret));
        return ret;
    }
    return i2c_driver_install(port_num,i2c_master.mode,0,0,0);
}

esp_err_t i2c_master_send(uint8_t *data, uint8_t len, uint16_t salve_addr, i2c_port_t port_num){

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, salve_addr << 1 | I2C_MASTER_WRITE, ACK_ENABLE);
    i2c_master_write(cmd, data, len, ACK_ENABLE);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(port_num, cmd, 1000/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;

}*/


///------------------> ahora haremos la misma gata pero con la nueva API, pero creo que va a valer gaber, creo que debo de dejar de hacerme pedenjo y empezar a usar el LCD con I2C, pero primero debemos de aprender a manerjar ambos campos


esp_err_t i2c_master_init(i2c_port_t port_num,int sda_pin, int scl_pin, uint32_t speed, i2c_master_bus_handle_t *i2c_handler_master, i2c_master_dev_handle_t *i2c_slave_handler, uint16_t slave_addr){

    esp_err_t ret;

    i2c_master_bus_config_t i2c_master_cnf ={
        .i2c_port = port_num,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };


    ret = i2c_new_master_bus(&i2c_master_cnf, i2c_handler_master);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "error al agregar el bus %s", esp_err_to_name(ret));
        return ret;
    }


    i2c_device_config_t slave_conf ={
        .dev_addr_length = I2C_ADDR_BIT_7,
        .device_address = slave_addr,
        .scl_speed_hz = speed,
    };

    ret = i2c_master_bus_add_device(i2c_handler_master, &slave_conf,i2c_slave_handler);

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "error al agregar el bus del esclavo %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}




