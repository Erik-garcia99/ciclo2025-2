#ifndef UART_LIB_H
#define UART_LIB_H 

//funciones para UART 
#include<driver/uart.h>

//macros 

#define BUFF 1024

/**
 * 
 * @brief estabelcer los parametros de UART 
 * 
 * 
 * @param uart_num -> numero del usart a usar 
 * @param data_frame -> de cunatos bits sera cada frame a trasmitir o recibir 
 * @param party_bits -> la cantidad de bit de paridad o si esta activcado 
 * 
 * @param uart_st_bits -> cunados bits representaran el stop bits 
 * 
*/
void uart_init(uart_port_t uart_num, uart_word_length_t data_frame,uart_parity_t parity_bits, uart_stop_bits_t uart_st_bits, uint8_t TX, uint8_t RX);



#endif