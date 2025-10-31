#ifndef GPIO_LIB_H
#define GPIO_LIB_H
#include<driver/gpio.h>
#include<esp_log.h>
#include<freertos/queue.h>

//cola que manjara el envio de GPIO 
extern QueueHandle_t xQueueGPIO;



/**
 *  @brief funcion para establecer los GPIO
 * 
 * 
 */

void gpio_init(void);

/**
 * 
 * @brief habilitamos los driver para el uso de interrupciones del GPIO en este caso en cambio de flanco habilitada la interrupcion 
 * 
 */
void type_int(void);


/**
 * @brief manjeador de interrupciones 
 * 
*/

void IRAM_ATTR gpio_isr_handler(void *args); 


/**
 * 
 * @brief Establece el handle de la cola para que la ISR pueda enviar eventos.
 * @param queue Handle de la cola.
 */
void gpio_set_queue_handle(QueueHandle_t queue);

#endif