#ifndef I2C_LIB_H
#define I2C_LIB_H
/**
 * 
 * @brief ESTA LIBERBEIA SE ENCARGA DE TODO SOBRE i2c 
 * PERIMRO lo haremos con la version vieja
*/
// #include<driver/i2c.h>
#include<driver/i2c_master.h>
#include<driver/i2c_types.h>

//macros

//VARIABLES 

//funciones 
/**
 * @brief funcion que incia el pericferico de I2C  
 * 
 * @return -> ESP_OK si se relzo con exico 
 * -> ESP_FAIL -> si ubo algun errore 
*/
//esp_err_t i2c_master_init(i2c_port_t port_num,int sda_pin, int scl_pin, uint32_t speed);

//funcion para mandar al slave 

//esp_err_t i2c_master_send(uint8_t *data, uint8_t len, uint16_t salve_addr, i2c_port_t port_num);



///---------> las mismas funciones pero ahora con la nueva API 

esp_err_t i2c_master_init(i2c_port_t port_num,int sda_pin, int scl_pin, uint32_t speed, i2c_master_bus_handle_t *i2c_handler_master, i2c_master_dev_handle_t *i2c_slave_handler, uint16_t slave_addr);




//tareas




#endif