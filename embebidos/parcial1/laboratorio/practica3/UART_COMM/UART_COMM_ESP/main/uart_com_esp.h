#ifndef UART_COM_ESP_H
#define UART_COM_ESP_H

#define UART_USER UART_NUM_0 //comunicacion de PC -> ESP
#define UART_ESP_COMM   UART_NUM_2 //comunicacion ESP->ESP ->PC


// #define TX_PIN 17
// #define RX_PIN 16
#define TX_PIN 18
#define RX_PIN 19

#define MAX_INPUT 25
#define BUF_SIZE 1024

#define HEIGHT 5 
#define WIDTH 6 // se le agrega +1 para el final de la linea 


typedef struct{
    char character;
    const char lines[HEIGHT][WIDTH];
}asciiChar;


void init_uarts();

const asciiChar* find_char(char c);

void task_banner(void *params);


#endif
