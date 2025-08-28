#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "parking.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_timer.h"

//varibales para controlar el entirebote
static uint64_t LAST_PRESS_UN=0;
static uint64_t LAST_PRESS_CINCO=0;
static uint64_t LAST_PRESS_DIEZ=0;
static uint64_t LAST_PRESS_VEINTE=0;


//comuncion entre tareas, por medio de las colas 
QueueHandle_t handlerQueue;

//definamos nuestros pines de entrada y de salida 
//necesitamos los 4 pines para la entrada de botones

//varibale global que usarmso para llevar el acumulador del dinero ingresado.
static uint8_t ACC_MONEY = 0;

void app_main(void)
{
    //creacion de cola 
    handlerQueue = xQueueCreate(10, sizeof(uint32_t));

    init_GPIO();
    TYPE_INT();

    //parametros
    /*
    parametros: 
    1-puntero a la funcion que contendra el codigo de la tarea. 
    2- un nomsbre descriptivo, este es un nombre que identifica a la tarea, puede ser cualquiera que nosotros queramos 
    3-un tamanio de de stack, este funciona para guardar vairbales, valores, etc. lo que vamos a ocupar, lo mas normal para una tarea simple puede de 1024 Bytes a 2048 bytes, mas completas puede ir a 4096 bytes o mas.
    4-este es un puntero a datos que se pasan a la tarea, como estrucutras con datos que se necesiten pasar, (en mi caso estare usarnado una varibale global - por ahora ya vere si lo cambio)
    5- prioridad de la tarea, con esto de determina que tarea se ejecuta priemro, dentro de la cola de prioridades
    6-handle: puntero donde se guarda la referencia de la tarea creada. 
    
    
    */
    xTaskCreate(PROCESS_INTR, "ISR_PROCESSOR", 2048, NULL, 1, NULL);

    while (true)
    {

    }
    
    
}


void init_GPIO(void){

    //configuramos el PIN como GPIO
    //INPUT
    gpio_reset_pin(UN_PESO_IN);
    gpio_reset_pin(CINCO_PESO_IN);
    gpio_reset_pin(DIEZ_PESO_IN);
    gpio_reset_pin(VEINTE_PESO_IN);

    //OUTPUT
    gpio_reset_pin(UN_PESO_OUT);
    gpio_reset_pin(CINCO_PESO_OUT);
    gpio_reset_pin(DIEZ_PESO_OUT);
    gpio_reset_pin(VEINTE_PESO_OUT);
    //configuracion como salida 
    gpio_set_direction(UN_PESO_OUT,GPIO_MODE_OUTPUT);
    gpio_set_direction(CINCO_PESO_OUT,GPIO_MODE_OUTPUT);
    gpio_set_direction(DIEZ_PESO_OUT,GPIO_MODE_OUTPUT);
    gpio_set_direction(VEINTE_PESO_OUT,GPIO_MODE_OUTPUT);

    //configuracion como entrada 
    //la entrada es el que ocupa el pull-up / pull-down

    gpio_set_direction(UN_PESO_IN,GPIO_MODE_INPUT);
    gpio_set_direction(CINCO_PESO_IN,GPIO_MODE_INPUT);
    gpio_set_direction(DIEZ_PESO_IN,GPIO_MODE_INPUT);
    gpio_set_direction(VEINTE_PESO_IN,GPIO_MODE_INPUT);

    //trabajemos por pull-up enable 

    gpio_pullup_dis(UN_PESO_IN);
    gpio_pulldown_en(UN_PESO_IN);

    gpio_pullup_dis(CINCO_PESO_IN);
    gpio_pulldown_en(CINCO_PESO_IN);

    gpio_pullup_dis(DIEZ_PESO_IN);
    gpio_pulldown_en(DIEZ_PESO_IN);

    gpio_pullup_dis(VEINTE_PESO_IN);
    gpio_pulldown_en(VEINTE_PESO_IN);


    //inicamos los pines de salida << estare jugando para prbarlos, pero deben deestar todos abajo, en una condicion incial>>

    gpio_set_level(UN_PESO_OUT,1);
    gpio_set_level(CINCO_PESO_OUT,0);
    gpio_set_level(DIEZ_PESO_OUT,1);
    gpio_set_level(VEINTE_PESO_OUT,1);

}


void TYPE_INT(void){

    //todos seran en un cunado ocurra un franco ascendente 
    
    gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_POSEDGE);

    //instlar el servicio de interrupciones 

    gpio_install_isr_service(0);

    //agregar manejador de GPIO

    gpio_isr_handler_add(UN_PESO_IN,gpio_isr_handler, (void *)UN_PESO_IN);
    gpio_isr_handler_add(DIEZ_PESO_IN,gpio_isr_handler, (void *)DIEZ_PESO_IN);
    gpio_isr_handler_add(CINCO_PESO_IN,gpio_isr_handler, (void *)CINCO_PESO_IN);
    gpio_isr_handler_add(VEINTE_PESO_IN,gpio_isr_handler, (void *)VEINTE_PESO_IN);


}

//esta funcion esta manejando la interrupcion, pero usamos un case para mapear a cual GPIO es el que se precionado. 
void IRAM_ATTR gpio_isr_handler(void *args){

    uint32_t pin_number=(uint32_t)args;
    uint64_t current_time = esp_timer_get_time();

    switch (pin_number)
    {
        //un delay de 100mS para poder leer el PIN 
        case UN_PESO_IN: {
            if(current_time - LAST_PRESS_UN >(DEBOUNCE_TIME *2000)){

                LAST_PRESS_UN = current_time;
                xQueueSendFromISR(handlerQueue, &pin_number, NULL);
            }
        } break;

        case CINCO_PESO_IN: {
            if(current_time - LAST_PRESS_CINCO > (DEBOUNCE_TIME * 2000)){
                LAST_PRESS_CINCO = current_time;
                xQueueSendFromISR(handlerQueue,&pin_number,NULL);
            }
        }break;


        case DIEZ_PESO_IN :{
            if(current_time - LAST_PRESS_DIEZ> (DEBOUNCE_TIME * 2000)){
                LAST_PRESS_DIEZ = current_time;
                xQueueSendFromISR(handlerQueue,&pin_number,NULL);

            }
        }break;

        case VEINTE_PESO_IN:{
            if(current_time - LAST_PRESS_VEINTE > (DEBOUNCE_TIME * 2000)){
                LAST_PRESS_VEINTE = current_time;
                xQueueSendFromISR(handlerQueue,&pin_number,NULL);
            }
        }break;
    }
}


void PROCESS_INTR(void *params){

    uint32_t pin_number;

    //toda tarea debe de tener un ciclo infinito, cuna asi hayan 2, 3, 4 todas deben de tener su ciclo infinito. 
    while (true)
    {
        /**
         * tercer parametro, hace referencia al tiempo maximo de expera, esto es que la tarea estara bloqueda esperando que llegue un elemento a la cola. << recordando 
         */
        if(xQueueReceive(handlerQueue, &pin_number, portMAX_DELAY)){

        }
        
    }
    
}












