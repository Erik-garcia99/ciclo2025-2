#ifndef I2C_LIB_H
#define I2C_LIB_H

#include<driver/i2c_types.h>
#include"esp_log.h"
#include "esp_err.h"

//macros 
#define ACK_CHECK_EN 0x01
#define I2C_RX_BUFF 256
#define I2C_TX_BUFF 256


//varibles 


//funciones 
/**
 * 
 * @brief configuracion del slave
 * 
 * @param port_i2c -> que puerto de I2C se quiere configurar como maestro 
 * @param sda_pin -> pin del buffer de datos 
 * @param scl_pin -> pin de reloj comun 
 * @param slave_addr -> direccion del salve 
 * 
*/

void init_I2C_slave(i2c_port_t port_i2c,int sda_pin, int scl_pin, uint32_t slave_addr);


#endif I2C_LIB_H 