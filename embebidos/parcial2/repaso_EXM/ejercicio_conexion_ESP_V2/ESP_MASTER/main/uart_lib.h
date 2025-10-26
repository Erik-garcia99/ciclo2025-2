#ifndef UART_LIB_H
#define UART_LIB_H
#include<driver/uart.h>
#include<freertos/event_groups.h>


//macros 
#define BUFF 256

typedef struct{
    uart_port_t num_uart;
    EventGroupHandle_t status_uart_S;

}param_uart_t;

extern QueueHandle_t event_uart;
extern EventGroupHandle_t status_uart;

//verifica que la transicion se relizo nates de poner borrar desde el main el buffer que se ingresa 
#define RX_COMPLETE BIT0 

//porque delcaro com unsigned char, para que el tipo de dato sea compatible con uint8_t que espera el I2C. porque este es un char sin signo que va de 0 - 255 recorando que char es de 1 byte
extern unsigned char input[BUFF];
extern int input_index;

/**
 * @brief funcion para inicar UART
 * 
 * 
*/

void uart_init(uart_port_t uart_num, uart_word_length_t len_frame, uart_parity_t uart_parity, uart_stop_bits_t uart_stop_bits, int TX, int RX);


/**
 * 
 * @brief tarea que se encarga de recibir lo de RX 
 * 
 * 
*/

void receive_task(void *params);


#endif