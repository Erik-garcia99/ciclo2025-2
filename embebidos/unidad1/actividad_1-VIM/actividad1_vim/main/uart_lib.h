#ifndef UART_LIB_H
#define UART_LIB_H

//en el caso de ser otro uart
#define RX 2
#define TX 4

#define UART_SEL UART_NUM_0
#define BUF 1024


//lo que es esto es un buffer en el cual ingresaremos lo que se ingreso por UART, esto, porque al inicio no sabemos de que tamanio sera el arreglo pero necesitamos algoq ue guarde los datos temporlment, estanran en buffer pero este valor se pierde si se esta ingresando algo de nuevo por UART y cambia antes de que pueda procesar por lo que utilizamos este arreglo de apoyo
char input_buffer[256];
int input_index=0;


uint8_t *array;
uint8_t size_array=0;






QueueHandle_t uart_evet;


//funciones 
void init_uart(int num_uart, int pin_tx, int pin_rx);


//tareas
void recive_uart_task(void *params);



#endif
