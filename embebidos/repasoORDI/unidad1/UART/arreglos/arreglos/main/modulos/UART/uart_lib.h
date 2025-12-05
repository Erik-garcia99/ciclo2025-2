#ifndef UART_LIB_H
#define UART_LIB_H


#include<freertos/FreeRTOS.h>
#include<freertos/queue.h>

#include<driver/uart.h>

#define TX 19
#define RX 18
#define BUFFER 1024
#define UART_SEL UART_NUM_0

extern QueueHandle_t uart_queue, data_queue;

//funcion para definir un uart genericia 

void init_uart(uart_port_t uart_num, uart_word_length_t data_len,uart_stop_bits_t uart_stop_b, uart_parity_t parity_mode, int rx_pin, int tx_pin);


//funcion de tarea para recibir datos por UART 

/**
 * 
 * @brief se encarga de procesar por eventos la entrada de datos por UART 
 * 
 * 
 */
void uart_task(void *params);




#endif