#ifndef UART_USER_H
#define UART_USER_H

#define UART_USER UART_NUM_0 //comunicacion de PC -> ESP
#define UART_ESP_COMM   UART_NUM_2 //comunicacion ESP->ESP ->PC

// #define TX_PIN 17
// #define RX_PIN 16

#define TX_PIN 18
#define RX_PIN 19

#define MAX_INPUT 25
#define BUF_SIZE 1024


void init_uarts();

//tarea pincipal 

void task_uart_input(void *params);






#endif