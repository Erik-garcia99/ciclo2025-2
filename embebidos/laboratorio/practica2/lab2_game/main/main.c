#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
//casi simepre estas seras las bibliotecas basicas de todo programa para el ESP32

#include <driver/gpio.h>
#include <time.h>
#include <esp_timer.h>
#include <freertos/event_groups.h>
#include "game.h" //mi libreria 

//variables para controlar el antirebote 

static uint64_t LAST_PRESS_UP_ = 0;
static uint64_t LAST_PRESS_DOWN_ = 0;

QueueHandle_t handlerQueue;


//creamos el gru´po de eventos
EventGroupHandle_t event_group;




void app_main(void)
{
    printf("configUSE 16 bits TICK %d\n",configUSE_16_BIT_TICKS);
    handlerQueue= xQueueCreate(10, sizeof(uint32_t));
    delimiter();
}



void init_gpio(void){

    //entrada
    gpio_reset_pin(_UP_);
    gpio_reset_pin(_DOWN_);
    gpio_reset_pin(_INIT_);
    //salida
    gpio_reset_pin(_LED1_);
    gpio_reset_pin(_LED2_);
    gpio_reset_pin(_LED3_);
    gpio_reset_pin(_LED4_);
    gpio_reset_pin(_LED5_);


    gpio_set_direction(_UP_,GPIO_MODE_INPUT);
    gpio_set_direction(_DOWN_,GPIO_MODE_INPUT);
    gpio_set_direction(_INIT_,GPIO_MODE_OUTPUT);
    //salidas
    gpio_set_direction(_LED1_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED2_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED3_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED4_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED5_,GPIO_MODE_OUTPUT);

    //para leer el boton usaremos la misma funcion y las mismas macros, funcion y mejor ni le movamos

    gpio_pullup_dis(_UP_);
    gpio_pulldown_en(_UP_);

    gpio_pullup_dis(_DOWN_);
    gpio_pulldown_en(_DOWN_);

    gpio_pullup_dis(_INIT_);
    gpio_pulldown_en(_INIT_);

    //seteamos los leds en 0

    gpio_set_level(_LED1_,0);
    gpio_set_level(_LED2_,0);
    gpio_set_level(_LED3_,0);
    gpio_set_level(_LED4_,0);
    gpio_set_level(_LED5_,0);


}

//las funciones estas para detectar los pulsos seran las mismas 


void type_intr(void){

    //indicamos que la interrupcion sera cunado detecte un flanco asendente 
    gpio_set_intr_type(_UP_, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(_DOWN_, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(_INIT_,GPIO_INTR_POSEDGE);

    //instalamos el servico de interrupciones 
    /*
        esto permite que el servicio de interrcupciones este habilitados por los GPIO

        esta funcion es incopatible con gpio_isr_register(), esta funcion registra un ISR global unico para todas las interrucpioesn de GPIO. Si se utiliza esta función, el servicio ISR proporciona un GPIO ISR global y los manipuladores individuales de pin se registran a través de la función gpio-isr-handler.add
        
        el parametro < 0 > indica que se esta aceptando la configuracion por defecta para la instaacion del servicio de interrupciones. 
    */

    gpio_install_isr_service(0);


    //menjo de interrupcions del GPIO, la funcion uqe tendera la interrupciones, recordando que un majedaro de interrupciones debe ser un codigo corto porque debe entrar y salir en el menor tiempo posible tan solo es para tomar decisiones 

    gpio_isr_handler_add(_UP_,gpio_isr_handler,(void *)_UP_);
    gpio_isr_handler_add(_DOWN_,gpio_isr_handler,(void *)_DOWN_);
    gpio_isr_handler_add(_INIT_,gpio_isr_handler,(void *)_INIT_);

}

//gardamos la funcion en RAM para que el proceso de ingresar sea mucho mas rapido
void IRAM_ATTR gpio_isr_handler(void *args){    

    uint32_t pin_number =(uint32_t)args;

    uint64_t current_time = esp_timer_get_time(); //creo qeu este devuelve el timepo en uS

    

    int current_state = gpio_get_level(pin_number); //esto lo puse para ser mas escritos en el antirebote

    

    if(gpio_get_level(pin_number) == 1){

        switch(pin_number){

            case _UP_:{
                if(current_state == 1 && (current_time - LAST_PRESS_UP_) > DEBOUNCE_TIME){


                    LAST_PRESS_UP_ = current_time;
                    xQueueSendFromISR(handlerQueue, &pin_number,NULL);

                }
            }break;

            case _DOWN_:{

                if(current_state == 1 && (current_time - LAST_PRESS_DOWN_) > DEBOUNCE_TIME){

                    LAST_PRESS_DOWN_ = current_state;
                    //el ultimo paremtro es la prioridad para enviar este dato, pero si le pone NULL esta bien dado que que un parametro opcional solo si queremos darle prioridad al envio de x dato por la cola. 
                    xQueueSendFromISR(handlerQueue,&pin_number,NULL);

                }

            }


        }

    }
}



void delimiter(void){

    for(int i=0; i<WIDTH_SCRE ; i++){
        putchar('=');
    }
    putchar('\n');

}
