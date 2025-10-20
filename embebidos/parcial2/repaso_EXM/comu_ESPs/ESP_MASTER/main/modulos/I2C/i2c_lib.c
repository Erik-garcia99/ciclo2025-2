#include"i2c_lib.h"
#include<driver/i2c.h>
#include<freertos/FreeRTOS.h>
#include <portmacro.h>

//funcion de configruacion e instalacion de drivers para ESP32 

void init_I2C_master(i2c_port_t port_i2c,int sda_pin, int scl_pin, uint32_t speed){

    i2c_config_t config_master = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl_pin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = speed,
        .clk_flags = 0,
    };

    ESP_ERROR_CHECK(i2c_param_config(port_i2c, &config_master));
    ESP_ERROR_CHECK(i2c_driver_install(port_i2c,config_master.mode, 0,0,0));
}



esp_err_t i2c_master_send(i2c_port_t port_i2c,uint8_t *data, uint8_t len, uint8_t addr_salave){

    //la operacion para crear el proceso de lectrua ya esta establecidda, reacionado con la api vieja

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //inicamos el i2c 
    i2c_master_start(cmd);

    //indicamos en que slave queremos realizar la operacion y que sera, si escritura o lectrua 

    i2c_master_write_byte(cmd, (addr_salave << 1) |I2C_MASTER_WRITE, ACK_CHECK_EN);
    //escribimos la ifnromacion
    i2c_master_write(cmd, data, len, ACK_CHECK_EN);
    i2c_master_stop(cmd); //finaliamos la trasmicion 

    //comandos para ralizar la trasmiscion, la infromacion se queda en el buffer 

    esp_err_t res = i2c_master_cmd_begin(port_i2c, cmd, 1000/ portTICK_PERIOD_MS); 
    
    i2c_cmd_link_delete(cmd);

    return res; 

}






