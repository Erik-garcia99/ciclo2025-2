#ifndef I2C_LIB_H
#define I2C_LIB_H
#include<driver/i2c_types.h>
//librerias 
#define ACK_CHECK_EN 0x01
//varibles 


//funciones 
/**
 * 
 * @brief configuracion del master 
 * 
 * @param port_i2c -> que puerto de I2C se quiere configurar como maestro 
 * @param sda_pin -> pin del buffer de datos 
 * @param scl_pin -> pin de reloj comun 
 * @param speed -> vevlocidad de comunicacion 
 * 
*/

void init_I2C_master(i2c_port_t port_i2c,int sda_pin, int scl_pin, uint32_t speed);

/**
 * 
 * @brief funcion que inica la trasmicion de MASTER a SLAVE 
 * solo eso no recibe nada, indica que quiere leer 
 * 
 * @param *data -> un apuntador a un byte que se mandara por el bus de datos, porque el bus solo puede trasmitir de 1 byte 
 * @param len -> tamanio de los datos a mandar 
 * @param addr_slave -> direccion del esclavo 
 * 
 * 
 * @return 
 * posiblemente puede retornar 
 * 
 * ESP_OK -> se pudo mandar correctamente 
 * 
 * ESP_FAIL -> no se pudo mandar la operacion
 * 
 * 
*/

esp_err_t i2c_master_send(i2c_port_t port_i2c,uint8_t *data, uint8_t len, uint8_t addr_salave);


//tareas 



#endif I2C_LIB_H 