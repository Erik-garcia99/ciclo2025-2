#ifndef I2C_LIB_H
#define I2C_LIB_H
/**
 * 
 * @brief ESTA LIBERBEIA SE ENCARGA DE TODO SOBRE i2c 
 * PERIMRO lo haremos con la version vieja
*/
// #include<driver/i2c.h>
#include<driver/i2c_slave.h>
#include<driver/i2c_types.h>

//macros
#define RX_BUFFER 512
#define TX_BUFFER 512

#define time_out 1000

//VARIABLES 

extern uint8_t i2c_rx_buffer[RX_BUFFER];
extern size_t i2c_rx_bytes;
extern bool i2c_data_ready;



//funciones 
/**
 * @brief funcion que incia el pericferico de I2C  
 * 
 * @return -> ESP_OK si se relzo con exico 
 * -> ESP_FAIL -> si ubo algun errore 
*/
esp_err_t i2c_slave_init(i2c_port_t port_num,int sda_pin, int scl_pin, uint16_t slave_addr, i2c_slave_dev_handle_t *i2c_slave_handler);

//funcion para mandar al slave 

// esp_err_t i2c_master_send(uint8_t *data, uint8_t len, uint16_t salve_addr, i2c_port_t port_num);

//tareas




#endif