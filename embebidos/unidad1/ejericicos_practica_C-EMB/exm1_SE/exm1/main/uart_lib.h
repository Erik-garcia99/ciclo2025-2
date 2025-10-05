#ifndef UART_LIB_H
#define UART_LIB_H

//macos
#define UART_SEL UART_NUM_0
#define BUFF 1024


//varibaes globales 
extern bool str1, str2;
char cad1[BUFF] = {0}, cad2[BUFF] = {0};


//define tareas
void uart_task(void *params); //se supone que  esta tarea es la que estara leyendo lo que recibe el UART 
void task_main(void *params); //tarea princopal 

//funciones
void init_uart();

//dentor de esta funcion vamos a limpiar el buffer cunado se ingrese '\n' o 'r', es la procesar lo que se ingresa 
void process_input(uint8_t *input, uint8_t size);


#endif