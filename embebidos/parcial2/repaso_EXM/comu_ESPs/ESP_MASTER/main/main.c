#include <stdio.h>
#include "modulos/I2C/i2c_lib.h"
#include"modulos/UART/uart_lib.h"
#include<stdlib.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>

//tal vez enviemos numeros de 1 a 10 al salve y que lo imprima, no se que podemos hcer jjjk 


//macros
//I2C 
#define ESP_SALVE 0x40 
#define I2C_SL I2C_NUM_0
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define SPEED 100000

//UART
#define UART_PORT UART_NUM_0
//si usamos otro uart podemos estbalecer los pines pero si es el 0 por lo genreal no se cambia ya que es con este con elq ue se comunica con la PC 
// #define TX_PIN 
// #define RX_PIN 

//genreales


//funciones 

//tareas 


void app_main(void)
{

    uart_init(UART_PORT,UART_DATA_8_BITS, UART_PARITY_DISABLE, UART_STOP_BITS_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    init_I2C_master(I2C_SL,I2C_SDA_PIN, I2C_SCL_PIN, SPEED);

    
}