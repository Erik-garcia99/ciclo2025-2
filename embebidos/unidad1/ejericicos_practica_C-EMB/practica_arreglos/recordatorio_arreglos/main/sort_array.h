#ifndef SORT_ARRAY_H
#define SORT_ARRAY_H

//macros


//varuables 
uint8_t *sort_array; //el arreglo aun no sabemos de que tamanio sera 
//el arreglo contendra datos de 8 bits 
uint8_t size_array =0; //el arreglo no sera mayor a 255, el tamanio maximo que soportea una varibale de 8 bits sin signo 

bool init_size=false;


//tareas 
void task_main(void *params);
//para agreasr los daatos y esperar a que no se consuman los recursos tendrian que ser 2 tareas 
void task_input(void *params);


//funciones

void statement_array();



#endif