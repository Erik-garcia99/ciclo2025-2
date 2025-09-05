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
\
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

    //todos seran en un cunado ocurra un franco ascendente 
    
    //NEG
    // gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_NEGEDGE);
    // gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_NEGEDGE);
    // gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_NEGEDGE);
    // gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_NEGEDGE);


    //POS
    gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_POSEDGE);

    
    // gpio_set_intr_type(UN_PESO_IN, GPIO_INTR_ANYEDGE);
    // gpio_set_intr_type(CINCO_PESO_IN, GPIO_INTR_ANYEDGE);
    // gpio_set_intr_type(DIEZ_PESO_IN, GPIO_INTR_ANYEDGE);
    // gpio_set_intr_type(VEINTE_PESO_IN, GPIO_INTR_ANYEDGE);
    
    //DEBEMOS AGREGAR PARA LOS NUEVOS BOTOSONES 

    gpio_set_intr_type(init_vehi,GPIO_INTR_POSEDGE);
    gpio_set_intr_type(pass_vehi,GPIO_INTR_POSEDGE);
    

    //instlar el servicio de interrupciones 

    gpio_install_isr_service(0);

    //agregar manejador de GPIO

    gpio_isr_handler_add(UN_PESO_IN,gpio_isr_handler, (void *)UN_PESO_IN);
    gpio_isr_handler_add(DIEZ_PESO_IN,gpio_isr_handler, (void *)DIEZ_PESO_IN);
    gpio_isr_handler_add(CINCO_PESO_IN,gpio_isr_handler, (void *)CINCO_PESO_IN);
    gpio_isr_handler_add(VEINTE_PESO_IN,gpio_isr_handler, (void *)VEINTE_PESO_IN);

    //###################################################################
    //INTERRUPCION pt2, creo que para esta mejor mandemosla a otra funcion a aprte para tener separados estos 2 grupos de botones. 

    gpio_isr_handler_add(init_vehi,gpio_isr_handler,(void *)init_vehi);
    gpio_isr_handler_add(pass_vehi,gpio_isr_handler,(void *)pass_vehi);
//###################################################################

}

//esta funcion esta manejando la interrupcion, pero usamos un case para mapear a cual GPIO es el que se precionado. 
void IRAM_ATTR gpio_isr_handler(void *args){

    uint32_t pin_number=(uint32_t)args;
    uint64_t current_time = esp_timer_get_time();

    static uint64_t last_isr_time = 0; // Tiempo de la última ISR

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

            //######################################################
            //los GPIO que escogi ya los habia configurado para le primera parte, por lo que no cuentan, debo de cambiarlos, pero ahoria no me acuerdo que pines estan libres en mi ESP
            //estamos enviando pero ahora a otra cola 
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

    //debugin 
    //##########3
    /*
    varibale local que indica si esta preparado para salir 
    como solo necesitamos saber si ya se completo el proceso por el cual le da salida al vehiculo solo tomaremos un valor booleano, true o false 
    */

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
                //printf("imprimiendo boleto!\n Buen viaje");
                //vending_machine(0);//recurdivo para procesar el pago y no tener que precionar un x boton de nuevo para qu eprocese el pago.
                //return;
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


//################### FUNCIONES PT2
/*
-> primero necesitamos la funcion que controlara la interrupcion para el grupo de vehiculoa.
en esta funcion se recibira que boton es el que se esta leyendo

*/




//funcion PRINCIPAL como queramos verla en la cual se realziaran la transicion entre los diferentes estados para el vehiculo 
void STEP_VEHI(void *params){

    //va a recibir cual es el boton que se preciono 

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
            //si estoy esperando, pero solo entrara solo se porgra hacer un proceso siempre y cunado se haya precionado el boton y se haya completado el pago 

            if(vehi_pos == init_vehi){

                //se completo el pago
                if(pyment_completed){

                    printf("vehiculo detectado - inicacion elevacion de barrea\n");
                    vehicle_waiting = true;
                    current_barrier_state = BARRIER_RAISING;
                    xTaskCreate(raise_needle_task,"RAISE_NEEDLE", 2048, NULL, 4, NULL);
                }
                else{
                    //se preciono peor no se completo el pago 
                    printf("no se ha completado el pago \n barrera cerrada\n");
                    //tarea que levantara la aguja
                   
                }
                
            }
            break;

        case BARRIER_RAISING: {

            printf("Elevando... \n");
            //barrier_state_machine(current_barrier_state);
            //return;
            /*
            if(vehi_pos == pass_vehi){
                //esperamos hastaq ue se precione el boton que indica que esta pasando el vehiculo
                //pero recordando que este proceso puede ser interrumpido
                printf("vehiculo pasadno - elevacion interrumpida");
                current_barrier_state = BARRIER_INTERRUPTED;
                vehicle_waiting = false; 
                return;
            }*/
            //si termina de salir cambia a BARRIE_UP
        }break;

        case BARRIER_UP:{
            if(vehi_pos == pass_vehi){
                printf("vehiculo ha pasdo - inciando decenso\n");
                vTaskDelay(2000/portTICK_PERIOD_MS); //esperando
                
                vehicle_waiting=false;
                current_barrier_state= BARRIER_LOWERING;
                xTaskCreate(lower_needle_task,"LOWER_NEEDLE",2048,NULL,4,NULL);
            }
            break;
        }



        case BARRIER_LOWERING:{

            printf("Bajando... \n");
            /*if(vehi_pos == pass_vehi){
                printf("nuevo vehiculo detectado - interrumpiedo decenso\n");
                current_barrier_state=BARRIER_INTERRUPTED;
                vehicle_waiting=true;

            }*/
            //si se termina de bajar cambias a BARRIER_WAITING
        }break;

        case BARRIER_INTERRUPTED:{

            if(vehi_pos == init_vehi && !vehicle_waiting){
                printf("reanudando elevacion desde interrumpcion\n");
                vehicle_waiting=true;
                current_barrier_state=BARRIER_RAISING;
                xTaskCreate(raise_needle_task,"RAISE_NEEDLE", 2048, NULL, 4, NULL);
            }
            else if(vehi_pos == pass_vehi && vehicle_waiting){
                printf("vehiculo pasndo contnuando decenso\n");
                vehicle_waiting=false;
                current_barrier_state=BARRIER_LOWERING;
                xTaskCreate(lower_needle_task,"LOWER_NEEDLE",2048,NULL,4,NULL);
            }

        }break;


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

    /*
    la seceuncia es DOWN -> MIDDLE -> UP

    pero tiene la codnicion de que esta operacion puede ser interrumpida, en este caso como se encuentra elevando en cualquier momento de los 3 estaods puede ser interrumpido y en este caso la aguja empezara a bajar. 

    para poder lograr esto, necesitamos verificar que la doncicion no se ha interrumpido, se interrumpe cunado se presiona el boton que indica que hay un vehiculo esperando el GPIO 15 << init_vehi >>
    
    */

    printf("inciando elevacion\n");

    if(current_barrier_state == BARRIER_RAISING && curren_state_needle== STATE_DOWN ){
        //prendemos el led que indica la posicion abajo
        //DOWN -> MIDDLE

        curren_state_needle = STATE_MIDDLE;
        update_needle_leds(STATE_MIDDLE,true);
        printf("posicion media\n");
        vTaskDelay(2000/portTICK_PERIOD_MS); //espera 2 segundos

    }

    if(current_barrier_state == BARRIER_RAISING && curren_state_needle==STATE_MIDDLE){
            //MIDDLE -> UP
            curren_state_needle = STATE_UP;
            update_needle_leds(STATE_UP,true);
            printf("aguja arriba \n");
            current_barrier_state =  BARRIER_UP; //barrera completamente arriba
            vTaskDelay(2000/portTICK_PERIOD_MS); //espera 2 segundos 
    
    }
    vTaskDelete(NULL); //elimina tarea
}


void lower_needle_task(void *params){
    
    printf("iniciando secuencia de decenso\n");

    // UP -> MIDDLE
    if(current_barrier_state == BARRIER_LOWERING && curren_state_needle== STATE_UP){
        curren_state_needle= STATE_MIDDLE;
        update_needle_leds(STATE_MIDDLE,false); //decenso
        printf("aguja posicion media - LOWER\n");
        vTaskDelay(2000/portTICK_PERIOD_MS);    
    }

    if(current_barrier_state == BARRIER_LOWERING && curren_state_needle==STATE_MIDDLE){
        // MIDDLE -> DOWN
        curren_state_needle = STATE_DOWN;
        update_needle_leds(STATE_DOWN,false);
        printf("aguja posicion BAJO - LOWER\n");
        pyment_completed=false;
        vehicle_waiting=false;
        current_barrier_state= BARRIER_WAITING;
        vTaskDelay(2000/portTICK_PERIOD_MS);

    }

    
    vTaskDelete(NULL);
}






