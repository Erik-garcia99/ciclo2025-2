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
//para el grupo de vehiculos 
static uint64_t LAST_PRESS_INIT_VEHI =0;
static uint64_t LAST_PRESS_PASS_VEHI = 0;


QueueHandle_t handlerQueue;
QueueHandle_t moneyQueue;



//definamos nuestros pines de entrada y de salida 
//necesitamos los 4 pines para la entrada de botones

//varibale global que usarmso para llevar el acumulador del dinero ingresado.
static int ACC_MONEY = 0;
static int change_money = 0;
vending_state_t current_state = STATE_INITIAL;

barrier_state_t current_barrier_state = BARRIER_WAITING;
bool pyment_completed= false;
bool vehicle_waiting= false;



state_needle_t curren_state_needle;

void app_main(void)
{
    //creacion de cola 
    handlerQueue = xQueueCreate(10, sizeof(uint32_t));
    moneyQueue = xQueueCreate(10, sizeof(uint32_t));
    vehicleQueue = xQueueCreate(10, sizeof(uint32_t));

    init_GPIO();
    TYPE_INT();



    xTaskCreate(PROCESS_INTR, "ISR_PROCESSOR", 2048, NULL, 1, NULL);
    xTaskCreate(MONEY_MANAGER, "MONEY_MANAGER", 2048, NULL, 2, NULL);
    //tarea para PT2
    xTaskCreate(STEP_VEHI,"STEP_VEHI",2048,NULL,3,NULL);

    
}


void init_GPIO(void){

    //configuramos el PIN como GPIO
    //INPUT
    gpio_reset_pin(UN_PESO_IN);
    gpio_reset_pin(CINCO_PESO_IN);
    gpio_reset_pin(DIEZ_PESO_IN);
    gpio_reset_pin(VEINTE_PESO_IN);
    gpio_reset_pin(TICKET_LED);
    //INIT PT2 
    gpio_reset_pin(init_vehi);
    gpio_reset_pin(pass_vehi);

    //OUTPUT
    gpio_reset_pin(UN_PESO_OUT);
    gpio_reset_pin(CINCO_PESO_OUT);
    gpio_reset_pin(DIEZ_PESO_OUT);
    
    //salida PT2
    gpio_reset_pin(_DOWN_);
    gpio_reset_pin(_MIDDLE_);
    gpio_reset_pin(_UP_);
    //configuracion como salida 
    gpio_set_direction(UN_PESO_OUT,GPIO_MODE_OUTPUT);
    gpio_set_direction(CINCO_PESO_OUT,GPIO_MODE_OUTPUT);
    gpio_set_direction(DIEZ_PESO_OUT,GPIO_MODE_OUTPUT);
    
    gpio_set_direction(TICKET_LED,GPIO_MODE_OUTPUT);
    //SALIDA PT2
    gpio_set_direction(_DOWN_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_MIDDLE_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_UP_,GPIO_MODE_OUTPUT);

    //configuracion como entrada 
    //la entrada es el que ocupa el pull-up / pull-down

    gpio_set_direction(UN_PESO_IN,GPIO_MODE_INPUT);
    gpio_set_direction(CINCO_PESO_IN,GPIO_MODE_INPUT);
    gpio_set_direction(DIEZ_PESO_IN,GPIO_MODE_INPUT);
    gpio_set_direction(VEINTE_PESO_IN,GPIO_MODE_INPUT);
    //entrada PT2
    gpio_set_direction(init_vehi,GPIO_MODE_INPUT);
    gpio_set_direction(pass_vehi,GPIO_MODE_INPUT);

    //pull-up DIS
    gpio_pullup_dis(UN_PESO_IN);
    gpio_pulldown_en(UN_PESO_IN);

    gpio_pullup_dis(CINCO_PESO_IN);
    gpio_pulldown_en(CINCO_PESO_IN);

    gpio_pullup_dis(DIEZ_PESO_IN);
    gpio_pulldown_en(DIEZ_PESO_IN);

    gpio_pullup_dis(VEINTE_PESO_IN);
    gpio_pulldown_en(VEINTE_PESO_IN);

    gpio_pullup_dis(init_vehi);
    gpio_pulldown_en(init_vehi);

    gpio_pullup_dis(pass_vehi);
    gpio_pulldown_en(pass_vehi);

    //inicamos los pines de salida << estare jugando para prbarlos, pero deben deestar todos abajo, en una condicion incial>>

    gpio_set_level(UN_PESO_OUT,0);
    gpio_set_level(CINCO_PESO_OUT,0);
    gpio_set_level(DIEZ_PESO_OUT,0);
    
    gpio_set_level(TICKET_LED,0);
    //salida inical PT2
    gpio_set_level(_DOWN_,0);
    gpio_set_level(_MIDDLE_,0);
    gpio_set_level(_UP_,0);

    curren_state_needle = STATE_DOWN;
    update_needle_leds(STATE_DOWN,true);//inicamos con el led que indica abajo prendid. y por defual lo marcamos que esta abajo que va a acender la aguja

}


void TYPE_INT(void){

    
    gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_POSEDGE);

    
    

    gpio_set_intr_type(init_vehi,GPIO_INTR_POSEDGE);
    gpio_set_intr_type(pass_vehi,GPIO_INTR_POSEDGE);
    

    //instlar el servicio de interrupciones 

    gpio_install_isr_service(0);

    //agregar manejador de GPIO

    gpio_isr_handler_add(UN_PESO_IN,gpio_isr_handler, (void *)UN_PESO_IN);
    gpio_isr_handler_add(DIEZ_PESO_IN,gpio_isr_handler, (void *)DIEZ_PESO_IN);
    gpio_isr_handler_add(CINCO_PESO_IN,gpio_isr_handler, (void *)CINCO_PESO_IN);
    gpio_isr_handler_add(VEINTE_PESO_IN,gpio_isr_handler, (void *)VEINTE_PESO_IN);
 

    gpio_isr_handler_add(init_vehi,gpio_isr_handler,(void *)init_vehi);
    gpio_isr_handler_add(pass_vehi,gpio_isr_handler,(void *)pass_vehi);

}

//esta funcion esta manejando la interrupcion, pero usamos un case para mapear a cual GPIO es el que se precionado. 
void IRAM_ATTR gpio_isr_handler(void *args){

    uint32_t pin_number=(uint32_t)args;
    uint64_t current_time = esp_timer_get_time();

    static uint64_t last_isr_time = 0; 

    //verificar que el pin este en alto antes de enviarlo a
    int current_state = gpio_get_level(pin_number);

    if (current_time - last_isr_time < DEBOUNCE_TIME) {
        return; // Ignorar si es demasiado pronto
    }
    last_isr_time = current_time;

    if(gpio_get_level(pin_number) == 1){
        
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

           
            case init_vehi:
                if(current_state == 1 && (current_time - LAST_PRESS_INIT_VEHI) > DEBOUNCE_TIME){
                    LAST_PRESS_INIT_VEHI = current_time;
                    xQueueSendFromISR(vehicleQueue, &pin_number,NULL);
                }
                break;
            case pass_vehi:{
                if(current_state == 1 && (current_time - LAST_PRESS_PASS_VEHI) > DEBOUNCE_TIME){
                    LAST_PRESS_PASS_VEHI = current_time;
                    xQueueSendFromISR(vehicleQueue,&pin_number,NULL);
                }
            }break;
        }

    }

}



//esta funcion es la que esta acomulando el dinero, es la funcion << PRINCIPAL >> si lo queremos ver asi. en funcionamiento del programa
void PROCESS_INTR(void *params){

    uint32_t pin_number;
    uint8_t money_acc =0;

    //toda tarea debe de tener un ciclo infinito, cuna asi hayan 2, 3, 4 todas deben de tener su ciclo infinito. 
    while (true)
    {
        
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
    
    printf("COSTO $15 pesos\n");
    while(true){
        if(xQueueReceive(moneyQueue, &received_money, portMAX_DELAY)){

            //total_money += received_money;
            printf("M - dinero recibido : %d \n", received_money);
            vending_machine(received_money);
            
        }
    }
}

//varibale globla de acumulacion ACC_MONEY

void vending_machine(int coin_value){



    printf("D - estado actual %d\n", current_state);

    switch (current_state)
    {
        case STATE_INITIAL:
            //aun no se ha ingresado dinero. 
            ACC_MONEY += coin_value;   
            printf("I - dinero acumulado $%d pesos\n", ACC_MONEY);

            //ahora debemos averiguar hacia que estado ahora se dirigira  

            if(ACC_MONEY == 15){
                current_state = STATE_PAID; //SE COMPLETO EL APGO
                printf("I - pago completado\n");
             
                vending_machine(0);
                return;
                
            }
            else if(ACC_MONEY > 15){
                current_state = STATE_OVERPAID;
                change_money = ACC_MONEY - 15; 
                printf("I - cambio $%d pesos\n", change_money);
                vending_machine(0);
                return;
            }
            else{
                //falta dinero
                current_state = STATE_COLLECTING;
                printf("I - FALTAN $%d pesos\n", 15 - ACC_MONEY);
            }
            break;

        case STATE_COLLECTING:
            ACC_MONEY += coin_value;
            printf("C - dinero acumulado $%d pesos\n", ACC_MONEY);

            if(ACC_MONEY == 15){
                current_state = STATE_PAID; //SE COMPLETO EL APGO
                printf("C - pago completado\n");
                vending_machine(0);
                return;
                
            }
            else if(ACC_MONEY > 15){
                current_state = STATE_OVERPAID;
                change_money = ACC_MONEY - 15; 
                printf("C - cambio $%d pesos\n", change_money);
                vending_machine(0);
                return;
            }
            else{
                //falta dinero
                current_state = STATE_COLLECTING;
                printf("C - FALTAN $%d pesos\n", 15 - ACC_MONEY);
            }break;

        case STATE_PAID: 
            //eastado donde ya se pago completo, sin cambio
            printf("P - imprimiendo boleto!\n");
            //prendemos el LED 25 por cerca de 3 segundos, hacinedo que "imprime el boleto"
            gpio_set_level(TICKET_LED,1);
            vTaskDelay(pdMS_TO_TICKS(3000));
            gpio_set_level(TICKET_LED,0);
            vTaskDelay(pdMS_TO_TICKS(3000));
            current_state= STATE_RECEIPT;
            printf("P - dinermo acumulado $%d pesos\n", ACC_MONEY);
            vending_machine(0);
            return;
            break;

        case STATE_OVERPAID:
            //CUNADO DI DE MAS ENTONCES NECESITO CAMBIO
            printf("O - preparando el cambio de $%d pesos\n", change_money);
            current_state= STATE_GIVING_CHANGE;
            vending_machine(0);
            return;
            break;
        case STATE_GIVING_CHANGE:

            //vamos a la funcion que
            give_change();
            current_state = STATE_PAID;
            vending_machine(0);
            return;
            break;

        case STATE_RECEIPT:
            //completado 
            printf("R - TRNASACION COMPLETADO\nBuen viaje\n");
            ACC_MONEY = 0;
            change_money = 0;
            current_state = STATE_INITIAL;
            //aqui irea la comunicacion con la funcion que proceso a los vehiculos, la cola con la que va a comunicar.
            pyment_completed = true;
            printf("listo para procesar al vehiculo\n");
            break;
    }

    printf("estado actual %d, dinero acumulado %d\n\n", current_state, ACC_MONEY);
}

void give_change(void){

    int acc_change = change_money;
    
    while(acc_change >= 10){
        //debug
        printf("10 pesos LED %d\n", DIEZ_PESO_OUT);

        gpio_set_level(DIEZ_PESO_OUT,1);
        vTaskDelay(2000/portTICK_PERIOD_MS);
        //vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
        gpio_set_level(DIEZ_PESO_OUT,0);
        //vTaskDelay(pdMS_TO_TICKS(2000));
        vTaskDelay(2000/portTICK_PERIOD_MS);
        acc_change -= 10;
    }

    while(acc_change >= 5){
        //debug
        printf("5 pesos LED %d\n", CINCO_PESO_OUT);
        gpio_set_level(CINCO_PESO_OUT,1);
        //vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
        vTaskDelay(2000/portTICK_PERIOD_MS);
        gpio_set_level(CINCO_PESO_OUT,0);
        // vTaskDelay(pdMS_TO_TICKS(2000));
        vTaskDelay(2000/portTICK_PERIOD_MS);
        acc_change -= 5;
    }  


    while(acc_change >= 1){
        //debug
        printf("1 pesos LED %d\n", UN_PESO_OUT);
        gpio_set_level(UN_PESO_OUT,1);
        //vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
        vTaskDelay(2000/portTICK_PERIOD_MS);
        gpio_set_level(UN_PESO_OUT,0);
        // vTaskDelay(pdMS_TO_TICKS(2000));
        vTaskDelay(2000/portTICK_PERIOD_MS);
        acc_change -= 1;
    } 


}




//funcion PRINCIPAL como queramos verla en la cual se realziaran la transicion entre los diferentes estados para el vehiculo 
void STEP_VEHI(void *params){


    int vehi_pos;

    while(true){
        if(xQueueReceive(vehicleQueue,&vehi_pos,portMAX_DELAY)){

            printf("posicion actual del vehiculo\n");

            if(vehi_pos == init_vehi){
                printf("vehiculo en esprea : GPIO %d\n",init_vehi);
            }
            else{
                printf("vehiculo en movimiento: GPIO: %d\n", pass_vehi);
            }
        }
        //de aqui a mandar a otra funcion
        barrier_state_machine(vehi_pos);
    }
}



void barrier_state_machine(int vehi_pos){

    printf("estado de la barrera: %d : GPIO : %d\n",current_barrier_state,vehi_pos);

    switch(current_barrier_state){

        case BARRIER_WAITING:
            
            if(vehi_pos == init_vehi){
                if(pyment_completed){
                    printf("vehiculo detectado - inicacion elevacion de barrea\n");
                    vehicle_waiting = true;
                    current_barrier_state = BARRIER_RAISING;
                    xTaskCreate(raise_needle_task,"RAISE_NEEDLE", 2048, NULL, 4, NULL);
                }
                else{
                    printf("no se ha completado el pago \n barrera cerrada\n");
                }
            }
            break;

        case BARRIER_RAISING:
            printf("elevando\n");
            
      
            if(vehi_pos == pass_vehi){
                printf("vehiculo pasando durante elevacion - interrumpiendo y bajando\n");
                current_barrier_state = BARRIER_INTERRUPTED;
                vehicle_waiting = false;
                
        
                xTaskCreate(lower_needle_task,"LOWER_NEEDLE_INTERRUPT",2048,NULL,4,NULL);
            }
            break;

        case BARRIER_UP:
            if(vehi_pos == pass_vehi){
                printf("vehiculo ha pasado - inciando decenso\n");
                vTaskDelay(2000/portTICK_PERIOD_MS);
                
                vehicle_waiting = false;
                current_barrier_state = BARRIER_LOWERING;
                xTaskCreate(lower_needle_task,"LOWER_NEEDLE",2048,NULL,4,NULL);
            }
            break;

        case BARRIER_LOWERING:
            printf("bajando\n");
            
            
            if(vehi_pos == init_vehi && pyment_completed){
                printf("nuevo vehiculo detectado durante descenso - interrumpiendo y subiendo\n");
                current_barrier_state = BARRIER_INTERRUPTED;
                vehicle_waiting = true;
                
              
                xTaskCreate(raise_needle_task,"RAISE_NEEDLE_INTERRUPT", 2048, NULL, 4, NULL);
            }
            break;

        case BARRIER_INTERRUPTED:
            printf("Estado de interrupción - procesando cambio de direccion\n");
            break;
    }
}


void update_needle_leds(state_needle_t needle_state, bool is_raising){
    if(is_raising){
        switch(needle_state){
            case STATE_DOWN:
                gpio_set_level(_DOWN_, 1);
                gpio_set_level(_MIDDLE_, 0);
                gpio_set_level(_UP_, 0);
                break;
            case STATE_MIDDLE:
                gpio_set_level(_DOWN_, 1);      
                gpio_set_level(_MIDDLE_, 1);    
                gpio_set_level(_UP_, 0);
                break;
            case STATE_UP:
                gpio_set_level(_DOWN_, 1);      
                gpio_set_level(_MIDDLE_, 1);    
                gpio_set_level(_UP_, 1);        
                break;
        }
    }
    else{
        switch(needle_state){
            case STATE_MIDDLE:
                gpio_set_level(_DOWN_, 1);      
                gpio_set_level(_MIDDLE_, 1);    
                gpio_set_level(_UP_, 0);      
                break;
            case STATE_DOWN:
                gpio_set_level(_DOWN_, 1);   
                gpio_set_level(_MIDDLE_, 0); 
                gpio_set_level(_UP_, 0);
                break;
            default:
                break;
        }
    }
}

void raise_needle_task(void *params){
    printf("iniciando elevacion desde estado: %d\n", curren_state_needle);

    // desde DOWN hacia MIDDLE
    if((current_barrier_state == BARRIER_RAISING || current_barrier_state == BARRIER_INTERRUPTED) && curren_state_needle == STATE_DOWN){
        curren_state_needle = STATE_MIDDLE;
        update_needle_leds(STATE_MIDDLE, true);
        printf("posicion media - ELEVANDO\n");
        vTaskDelay(2000/portTICK_PERIOD_MS);
        
        // verificar si fue interrumpido durante el delay
        if(current_barrier_state == BARRIER_INTERRUPTED && !vehicle_waiting){
            printf("Elevacion interrumpida en posicion MIDDLE - cambiando a descenso\n");
            current_barrier_state = BARRIER_LOWERING;
            xTaskCreate(lower_needle_task,"LOWER_NEEDLE_FROM_INTERRUPT",2048,NULL,4,NULL);
            vTaskDelete(NULL);
            return;
        }
    }

    // desde MIDDLE hacia UP
    if((current_barrier_state == BARRIER_RAISING || current_barrier_state == BARRIER_INTERRUPTED) && curren_state_needle == STATE_MIDDLE){
        curren_state_needle = STATE_UP;
        update_needle_leds(STATE_UP, true);
        printf("aguja arriba - elevacion completa\n");
        current_barrier_state = BARRIER_UP;
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
    
    vTaskDelete(NULL);
}

void lower_needle_task(void *params){
    printf("iniciando descenso desde estado: %d\n", curren_state_needle);

    // desde UP hacia MIDDLE
    if((current_barrier_state == BARRIER_LOWERING || current_barrier_state == BARRIER_INTERRUPTED) && curren_state_needle == STATE_UP){
        curren_state_needle = STATE_MIDDLE;
        update_needle_leds(STATE_MIDDLE, false);
        printf("posicion media - BAJANDO\n");
        vTaskDelay(2000/portTICK_PERIOD_MS);
        
        // verificar si fue interrumpido durante el delay
        if(current_barrier_state == BARRIER_INTERRUPTED && vehicle_waiting){
            printf("Descenso interrumpido en posicion MIDDLE - cambiando a elevacion\n");
            current_barrier_state = BARRIER_RAISING;
            xTaskCreate(raise_needle_task,"RAISE_NEEDLE_FROM_INTERRUPT", 2048, NULL, 4, NULL);
            vTaskDelete(NULL);
            return;
        }
    }

    // Desde MIDDLE hacia DOWN
    if((current_barrier_state == BARRIER_LOWERING || current_barrier_state == BARRIER_INTERRUPTED) && curren_state_needle == STATE_MIDDLE){
        curren_state_needle = STATE_DOWN;
        update_needle_leds(STATE_DOWN, false);
        printf("aguja posicion BAJO - DESCENSO COMPLETO\n");
        
        // Solo resetear variables si llegamos completamente abajo
        pyment_completed = false;
        vehicle_waiting = false;
        current_barrier_state = BARRIER_WAITING;
        vTaskDelay(2000/portTICK_PERIOD_MS);
    }
    
    vTaskDelete(NULL);
}





