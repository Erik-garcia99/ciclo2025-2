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
QueueHandle_t handlerQueue;
QueueHandle_t moneyQueue;



//definamos nuestros pines de entrada y de salida 
//necesitamos los 4 pines para la entrada de botones

//varibale global que usarmso para llevar el acumulador del dinero ingresado.
static int ACC_MONEY = 0;
static int change_money = 0;
vending_state_t current_state = STATE_INITIAL;


void app_main(void)
{
    //creacion de cola 
    handlerQueue = xQueueCreate(10, sizeof(uint32_t));
    moneyQueue = xQueueCreate(10, sizeof(uint32_t));

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
    xTaskCreate(MONEY_MANAGER, "MONEY_MANAGER", 2048, NULL, 2, NULL);

    
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

    /*pull-up EN
    gpio_pullup_en(UN_PESO_IN);
    gpio_pulldown_dis(UN_PESO_IN);

    gpio_pullup_en(CINCO_PESO_IN);
    gpio_pulldown_dis(CINCO_PESO_IN);

    gpio_pullup_en(DIEZ_PESO_IN);
    gpio_pulldown_dis(DIEZ_PESO_IN);

    gpio_pullup_en(VEINTE_PESO_IN);
    gpio_pulldown_dis(VEINTE_PESO_IN);
    */

    //pull-up DIS
    gpio_pullup_dis(UN_PESO_IN);
    gpio_pulldown_en(UN_PESO_IN);

    gpio_pullup_dis(CINCO_PESO_IN);
    gpio_pulldown_en(CINCO_PESO_IN);

    gpio_pullup_dis(DIEZ_PESO_IN);
    gpio_pulldown_en(DIEZ_PESO_IN);

    gpio_pullup_dis(VEINTE_PESO_IN);
    gpio_pulldown_en(VEINTE_PESO_IN);
    //inicamos los pines de salida << estare jugando para prbarlos, pero deben deestar todos abajo, en una condicion incial>>

    gpio_set_level(UN_PESO_OUT,0);
    gpio_set_level(CINCO_PESO_OUT,0);
    gpio_set_level(DIEZ_PESO_OUT,0);
    gpio_set_level(VEINTE_PESO_OUT,0);
}


void TYPE_INT(void){

    //todos seran en un cunado ocurra un franco ascendente 
    
    //NEG
    // gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_NEGEDGE);
    // gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_NEGEDGE);
    // gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_NEGEDGE);
    // gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_NEGEDGE);


    //POS
    // gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_POSEDGE);


    gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_ANYEDGE);
    gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_ANYEDGE);

    

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

    //verificar que el pin este en alto antes de enviarlo a
    int current_state = gpio_get_level(pin_number);

    switch (pin_number)
    {
        //un delay de 100mS para poder leer el PIN 
        case UN_PESO_IN: {
            if(current_state == 1 && (current_time - LAST_PRESS_UN) >DEBOUNCE_TIME){

                LAST_PRESS_UN = current_time;
                xQueueSendFromISR(handlerQueue, &pin_number, NULL);
            }
        } break;

        case CINCO_PESO_IN: {
            if(current_state ==1 && (current_time - LAST_PRESS_CINCO) > DEBOUNCE_TIME){
                LAST_PRESS_CINCO = current_time;
                xQueueSendFromISR(handlerQueue,&pin_number,NULL);
            }
        }break;


        case DIEZ_PESO_IN :{
            if(current_state == 1 && (current_time - LAST_PRESS_DIEZ) > DEBOUNCE_TIME ){
                LAST_PRESS_DIEZ = current_time;
                xQueueSendFromISR(handlerQueue,&pin_number,NULL);

            }
        }break;

        case VEINTE_PESO_IN:{
            if(current_state == 1 && (current_time - LAST_PRESS_VEINTE) > DEBOUNCE_TIME){
                LAST_PRESS_VEINTE = current_time;
                xQueueSendFromISR(handlerQueue,&pin_number,NULL);
            }
        }break;
    }
}

//esta funcion es la que esta acomulando el dinero, es la funcion << PRINCIPAL >> si lo queremos ver asi. en funcionamiento del programa
void PROCESS_INTR(void *params){

    uint32_t pin_number;
    uint8_t money_acc =0;

    //toda tarea debe de tener un ciclo infinito, cuna asi hayan 2, 3, 4 todas deben de tener su ciclo infinito. 
    while (true)
    {
        /**
         * tercer parametro, hace referencia al tiempo maximo de expera, esto es que la tarea estara bloqueda esperando que llegue un elemento a la cola. << recordando que las tareas que comunican entre ellas por medio de la colas >>, pueden ser 0, que no espera, 1 - reresenta tocks a esperas
         * 
         * o portMAX_delay, esn este caso se bloquea hasta que llegue algo.
         */
        if(xQueueReceive(handlerQueue, &pin_number, portMAX_DELAY)){

            switch(pin_number){
                case UN_PESO_IN:
                    printf("1 peso insertado\n");
                    money_acc =1;
                    break;
                case CINCO_PESO_IN:
                    printf("5 pesos insertados\n");
                    money_acc =5;
                    break;

                case DIEZ_PESO_IN:
                    printf("10 pesos insertados\n");
                    money_acc =10;
                    break;

                case VEINTE_PESO_IN:
                    printf("20 pesos insertados\n");
                    money_acc =20;
                    break;
            }

            xQueueSend(moneyQueue,&money_acc,0);
        }
        
    }
    
}

void MONEY_MANAGER(void *params)
{
    
    uint8_t received_money;
    uint8_t total_money = 0;

    printf("COSTO $15 pesos");

    while(true){
        if(xQueueReceive(moneyQueue, &received_money, portMAX_DELAY)){
            
            
            //total_money += received_money;
            printf("dinero recibido : %d \n", total_money);
            vending_machine(received_money);
            
        }
    }
}



//varibale globla de acumulacion ACC_MONEY

void vending_machine(int coin_value){

    //debugin 

    printf("estado actual %d", current_state);

    switch (current_state)
    {
        
        case STATE_INITIAL:
            //aun no se ha ingresado dinero. 
            ACC_MONEY += coin_value;   
            printf("dinero acumulado $%d pesos", ACC_MONEY);

            //ahora debemos averiguar hacia que estado ahora se dirigira  

            if(ACC_MONEY == 15){
                current_state = STATE_PAID; //SE COMPLETO EL APGO
                printf("pago completado\n");
                //printf("imprimiendo boleto!\n Buen viaje");
                
            }
            else if(ACC_MONEY > 15){
                current_state = STATE_OVERPAID;
                change_money = ACC_MONEY - 15; 
                printf("cambio $%d pesos\n", change_money);
            }
            else{
                //falta dinero
                current_state = STATE_COLLECTING;
                printf("FALTAN $%d pesos", 15 - ACC_MONEY);
            }break;

        case STATE_COLLECTING:
            ACC_MONEY += coin_value;
            printf("dinero acumulado $%d pesos", ACC_MONEY);

            if(ACC_MONEY == 15){
                current_state = STATE_PAID; //SE COMPLETO EL APGO
                printf("pago completado\n");
                //printf("imprimiendo boleto!\n Buen viaje");
                
            }
            else if(ACC_MONEY > 15){
                current_state = STATE_OVERPAID;
                change_money = ACC_MONEY - 15; 
                printf("cambio $%d pesos\n", change_money);
            }
            else{
                //falta dinero
                current_state = STATE_COLLECTING;
                printf("FALTAN $%d pesos", 15 - ACC_MONEY);
            }break;

        case STATE_PAID: 
            //eastado donde ya se pago completo, sin cambio
            printf("imprimiendo boleto!\n");
            //prendemos el LED 25 por cerca de 3 segundos, hacinedo que "imprime el boleto"
            gpio_set_level(TICKET_LED,1);
            vTaskDelay(pdMS_TO_TICKS(3000));
            gpio_set_level(TICKET_LED,0);
            current_state= STATE_RECEIPT;
            printf("dinermo acumulado $%d pesos", ACC_MONEY);
            break;

        case STATE_OVERPAID:
            //CUNADO DI DE MAS ENTONCES NECESITO CAMBIO
            printf("preparando el cambio de $%d pesos", change_money);
            current_state= STATE_GIVING_CHANGE;
            break;
        case STATE_GIVING_CHANGE:

            //vamos a la funcion que
            give_change();
            current_state = STATE_PAID;
            break;

        case STATE_RECEIPT:
            //completado 
            printf("TRNASACION COMPLETADO\nBuen viaje");
            ACC_MONEY = 0;
            change_money = 0;
            current_state = STATE_INITIAL;
            break;
    }

    printf("estado actual %d, dinero acumulado %d\n\n", current_state, ACC_MONEY);
}

void give_change(void){

    int acc_change = acc_change;

    while(acc_change >= 10){
        //debug
        printf("10 pesos LED %d\n", DIEZ_PESO_OUT);

        gpio_set_level(DIEZ_PESO_OUT,1);
        vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
        gpio_set_level(DIEZ_PESO_OUT,0);
        vTaskDelay(pdMS_TO_TICKS(2000));
        acc_change -= 10;
    }

    while(acc_change >= 5){
        //debug
        printf("5 pesos LED %d\n", CINCO_PESO_OUT);
        gpio_set_level(CINCO_PESO_OUT,1);
        vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
        gpio_set_level(CINCO_PESO_OUT,0);
        vTaskDelay(pdMS_TO_TICKS(2000));
        acc_change -= 5;
    }  


    while(acc_change >= 1){
        //debug
        printf("1 pesos LED %d\n", UN_PESO_OUT);
        gpio_set_level(UN_PESO_OUT,1);
        vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
        gpio_set_level(UN_PESO_OUT,0);
        vTaskDelay(pdMS_TO_TICKS(2000));
        acc_change -= 1;
    }  


}








