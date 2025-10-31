#ifndef GPIO_LIB_H
#define GPIO_LIB_H
#include "esp_attr.h"
// #include<freertos/queue.h>





/**
 *  @brief funcion para establecer los GPIO
 *
 *
*/

void gpio_init(void);

/**
 * 
 * @brief instalar con controladores para el manjeador de interrrupciones
 * 
 * 
*/
void type_int(void);

/**
 * 
 * @brief manjeador de interrupciones 
 * 
*/


void IRAM_ATTR gpio_isr_handler(void *args); 

#endif