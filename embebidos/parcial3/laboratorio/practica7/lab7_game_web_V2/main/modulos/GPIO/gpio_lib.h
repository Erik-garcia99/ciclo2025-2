#ifndef GPIO_LIB_H
#define GPIO_LIB_H
#include <stdint.h>
#include "esp_attr.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

// extern QueueHandle_t button_queue;
#define PLAYER_1 17
#define PLAYER_2 16

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


/**
 * @brief recibe la cola desde el main  
 * 
 * @param queue -> la cola a utilizar para mandar el GPIO utilizado
 * 
*/

void get_queue(QueueHandle_t *queue);

#endif