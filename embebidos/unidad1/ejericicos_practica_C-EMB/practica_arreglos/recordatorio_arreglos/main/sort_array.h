#ifndef SORT_ARRAY_H
#define SORT_ARRAY_H

//si usamos el UART0 para recibir los datos deberia de sersin cambias 



#define UART_SEL UART_NUM_0
#define BUFF 1024

uint8_t *sort_array;
uint8_t size_array;


//tareas 
void task_event_uart(void *params);

//funciones 
void init_uart();

void process_input(uint8_t *input, uint8_t size);

#endif