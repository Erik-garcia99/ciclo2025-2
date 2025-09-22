#ifndef DISPLAY_H
#define DISPLAY_H

#define TX_PIN 4
#define RX_PIN 2
#define BUF_SIZE 1024


//los numeros los marcamos para poder leer de una sola tanda, que se separen por coma para saber en donde temrina y empieza 2





void uart_init();
//tarea prinicipal: 
void task_quick(void *params);
void task_read_uart(void *params);




#endif