#ifndef I2C_LIB_H
#define I2C_LIB_H
// #include<driver/i2c_master.h>
// #include<driver/i2c_slave.h>
#include<driver/i2c.h>
#include<driver/i2c_types.h>

/**
 * @author erik garcia chavez 
 * @author erik lerma 
 * 
 * @brief general 
 * 
 * 
 * 
 * @attention la libreria solo permite direcciones de 7 bits
 * 
 * < en este momento no acepta de 10 bits > 
 */



//macros 


//varibales

//funciones


/**
 * 
 * @brief inicalizacion del I2C
 * 
 * @param num_i2c -> numero del I2C a utulizar (I2C_NUM_0 - I2C_NUM_1)
 * @param pin_sda -> pin GPIO para los datos 
 * @param pin_scl -> pin GPIO para el reloj 
 * @param slave_addr -> direccion del dispositovo esclavo 
 * @param bus_handle -> salida, controlador de bus I2C MASTER
 * @param dev_handle -> identificador del salve 
 * 
 * 
 */


void i2c_master_init(i2c_port_num_t num_i2c, gpio_num_t pin_sda, gpio_num_t pin_scl, uint32_t speed);




/**
 * 
 * @brief inicinado I2C slave 
 * @param num_i2c -> numero del I2C a utulizar (I2C_NUM_0 - I2C_NUM_1)
 * @param pin_sda -> pin GPIO para los datos 
 * @param pin_scl -> pin GPIO para el reloj 
 * @param slave_addr -> direccion del dispositovo esclavo 
 * @param slave_handle  -> bus de identificacion de slave 
 * @param buff_tx -> tamanio del buffer circular para enviar datos 
 * @param buff_rx -> tamanio del buffer del software para recepcion 
 * 
 * 
 * 
 * 
 */

void i2C_slave_init(i2c_port_num_t num_i2c, gpio_num_t pin_sda, gpio_num_t pin_scl, uint8_t slave_addr);

//tareas 





#endif
