#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <string.h>
#include <driver/gpio.h>
#include <time.h>
#include <esp_timer.h>
#include <freertos/event_groups.h>
#include "game.h" 

//matriz que define la posicion de los obstaculos 
//volatile int obstacle[]][];
//las filas estan declaradas en el primer corcehte y las columnas en el segundo corchete 
//volatile int obstacle[3][WIDTH_SCRE];
//matriz princola 
//volatile int obstacle[HEIGTH_SCRE][3];
//volatile char canvas[WIDTH_SCRE][HEIGTH_SCRE];
//volatile char canvas[HEIGTH_SCRE][WIDTH_SCRE];
//###########################
//debemos de cambiar donde manerajamos los vectores de una manera inversa 
//array[FILAS_Y][COLUMLAS_X]
// FILAS COORDENADA Y -> priemr corcehte 
//COLUMNAS COORDENAAS X -> segundo crocehte 

//MATRICES PRINCIPALES 
volatile int obstacle[3][WIDTH_SCRE];
volatile char canvas[HEIGTH_SCRE][WIDTH_SCRE];


//variables para controlar el antirebote 
static uint64_t LAST_PRESS_UP_ = 0;
static uint64_t LAST_PRESS_DOWN_ = 0;
static uint64_t LAST_PRESS_INIT_ = 0; //aunque este solo se precionara 1 vez en todo el programa 
static uint64_t LAST_PRESS_FINISH_ = 0;

volatile int current_line = 1; //la direccion en donde estara
static int game_speed = 50; //VELOCIDAD del juego 
static int vehicle_x = SPAWN_OBJ; //donde aparecera en un inicio

//volatile int current_line=1; //estabelcemos que 0 - arriba - 1 -centro y 2 - abajo  <pero debemos de establecer alguna condicionan en donde por quejemplo si le da varias veces al arriba o abajo no ceremente mas de lo que deberia si no que se quede donde mismo. >, el voaltin porque su valor va a estar cambiando entonces C no lo optimiza. 
//static int final_scree =0; //varibale que me indica que el vechiculo llego al final de al "pantalla"
//static int init_game = 1; //esta vriable es solamente cunado inicia el juego y piueda imrpiemr, solo el el primer aranque 


//static int game_speed = 100; // Velocidad del juego en ms 
//static int vehicle_x = SPAWN_OBJ;


//colas
//debemos de crear una cola en donde enviamos a nuestra tarea pincipal que es la que debe de animar si el objeto va hacia arriba o hacia abajo 
QueueHandle_t handlerQueue, moveObjectQueue;

//creamos el grupo de eventos
EventGroupHandle_t event_group;


//varibales globales 
//esta variable es para indicar que el juego inicio, esto es pirque si genero numeros randoms puede que gener muy cerca del carro entonces con esta condicion verificamos que la primera aparicion de los objetos aparezca a una distancia considerable de otra forma seria como incio y perder. 

//despues ya tiene que aparecen con condiciones en los cuales deben de estar a una distancia minima tanto de altura como en anchura para que el vehiculo o el objeto a jugar pueda pasar sin probelma 
volatile bool inicio = false; //para que no la optmice 
const int ready = BIT0;
const int seg = BIT1; // segun yo este sera para cunado pase 1 segundo, 

volatile uint32_t elapsed_seconds = 0; // varibale global para el timepo 
volatile uint64_t last_update_time = 0;

//#############################################
// volatile uint32_t debug_isr_count_up = 0;
// volatile uint32_t debug_isr_count_down = 0;
// volatile uint32_t debug_isr_count_init = 0;
// volatile uint32_t debug_isr_count_finish = 0;
// volatile uint32_t debug_last_pin = 0;
// volatile uint32_t debug_pin_state = 0;


// void reset_debug_counters(void){
//     debug_isr_count_up = 0;
//     debug_isr_count_down = 0;
//     debug_isr_count_init = 0;
//     debug_isr_count_finish = 0;
// }


//###########################################




void app_main(void)
{
    printf("configUSE 16 bits TICK %d\n",configUSE_16_BIT_TICKS);
        //esta cola maneja los botones 

        //creamos el eventos de grupos 

    event_group = xEventGroupCreate();
        
    handlerQueue= xQueueCreate(10, sizeof(uint32_t));
    moveObjectQueue = xQueueCreate(10,sizeof(uint32_t));

    srand(time(NULL)); //inicamos la semilla

    //incializacion de funciones 

    init_matrices(); 
    init_gpio();
    type_intr();

    //init_matrices();  
    //init_gpio();
    //type_intr();
        
        
        //tarea que maneja que boton es el que se preciono, ese lo recibe de la ISR
        //la de mayor prioridad es la que controla los botoens de entrada y la de una de menos seria lo que se muestra en pantalla y la de menos prioridad seria la que muestra el tiempo en LEDs, con esos no hya tanto aguite si funciona al millon. 
    xTaskCreate(task_input,"task_input",2048,NULL,10,NULL);
    xTaskCreate(task_game_main,"task_main",4096,NULL,9,NULL); //asigmaos un poco mas de espacio por que vamos a llamar a funciones para que no todo este dentro de ahi

    //tarea de cotrol de timepo 

    xTaskCreate(task_timer,"task_timer",2048,NULL,8,NULL);
    xTaskCreate(task_timer_gpio,"task_timer_gpio",2048,NULL,7,NULL);

    //xTaskCreate(debug_stats_task, "task",2048,NULL,8,NULL);
        //minitarea que se encarga de inicar el juego 
        //xTaskCreate(task_init_game,"task_init_game",2048,NULL,5,NULL);
    delimiter();
    printf("\t\t\n\n\n*************** PRESIONE EL BOTON PARA INICIAR EL JUEGO ***************\n\n\n");
    delimiter();
    
}



void init_gpio(void){

    //entrada
    gpio_reset_pin(_UP_);
    gpio_reset_pin(_DOWN_);
    gpio_reset_pin(_INIT_);
    gpio_reset_pin(_FINISH_);
    //salida
    gpio_reset_pin(_LED1_);
    gpio_reset_pin(_LED2_);
    gpio_reset_pin(_LED3_);
    gpio_reset_pin(_LED4_);
    gpio_reset_pin(_LED5_);


    gpio_set_direction(_UP_,GPIO_MODE_INPUT);
    gpio_set_direction(_DOWN_,GPIO_MODE_INPUT);
    gpio_set_direction(_INIT_,GPIO_MODE_INPUT);
    gpio_set_direction(_FINISH_,GPIO_MODE_INPUT);
    //salidas
    gpio_set_direction(_LED1_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED2_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED3_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED4_,GPIO_MODE_OUTPUT);
    gpio_set_direction(_LED5_,GPIO_MODE_OUTPUT);

    //para leer el boton usaremos la misma funcion y las mismas macros, funcion y mejor ni le movamos

    // gpio_pullup_dis(_UP_);
    // gpio_pulldown_en(_UP_);

    // gpio_pullup_dis(_DOWN_);
    // gpio_pulldown_en(_DOWN_);

    // gpio_pullup_dis(_INIT_);
    // gpio_pulldown_en(_INIT_);

    // gpio_pullup_dis(_FINISH_);
    // gpio_pulldown_en(_FINISH_);


    gpio_pullup_en(_UP_);
    gpio_pulldown_dis(_UP_);
    
    gpio_pullup_en(_DOWN_);
    gpio_pulldown_dis(_DOWN_);
    
    gpio_pullup_en(_INIT_);
    gpio_pulldown_dis(_INIT_);
    
    gpio_pullup_en(_FINISH_);
    gpio_pulldown_dis(_FINISH_);
    

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
    // gpio_set_intr_type(_UP_, GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(_DOWN_, GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(_INIT_,GPIO_INTR_POSEDGE);
    // gpio_set_intr_type(_FINISH_,GPIO_INTR_POSEDGE);

    gpio_set_intr_type(_UP_, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(_DOWN_, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(_INIT_, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(_FINISH_, GPIO_INTR_NEGEDGE);

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
    gpio_isr_handler_add(_FINISH_,gpio_isr_handler,(void *)_FINISH_);

}


// void IRAM_ATTR gpio_isr_handler(void *args){    
//     uint32_t pin_number = (uint32_t)args;
//     uint64_t current_time = esp_timer_get_time();
    
  
//     switch(pin_number) {
//         case _UP_:
//             if((current_time - LAST_PRESS_UP_) > DEBOUNCE_TIME){
//                 LAST_PRESS_UP_ = current_time;
//                 xQueueSendFromISR(handlerQueue, &pin_number, NULL);
//             }
//             break;
            
//         case _DOWN_:
//             if((current_time - LAST_PRESS_DOWN_) > DEBOUNCE_TIME){
//                 LAST_PRESS_DOWN_ = current_time;
//                 xQueueSendFromISR(handlerQueue, &pin_number, NULL);
//             }
//             break;
            
//         case _FINISH_:
//             if((current_time - LAST_PRESS_FINISH_) > DEBOUNCE_TIME){
//                 LAST_PRESS_FINISH_ = current_time;
//                 xQueueSendFromISR(handlerQueue, &pin_number, NULL);
//             }
//             break;
            
//         case _INIT_:
//             if ((current_time - LAST_PRESS_INIT_) > DEBOUNCE_TIME) {
//                 LAST_PRESS_INIT_ = current_time;
//                 if (inicio == false) {
//                     inicio = true;
//                     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//                     xEventGroupSetBitsFromISR(event_group, ready, &xHigherPriorityTaskWoken);
//                     xQueueSendFromISR(handlerQueue, &pin_number, NULL);
//                     if (xHigherPriorityTaskWoken) {
//                         portYIELD_FROM_ISR();
//                     }
//                 }
//             }
//             break;
//     }
// }

void IRAM_ATTR gpio_isr_handler(void *args){    
  uint32_t pin_number = (uint32_t)args;
  uint64_t current_time = esp_timer_get_time();
  
  switch(pin_number) {
        case _UP_:{
            if((current_time - LAST_PRESS_UP_) > DEBOUNCE_TIME){

                if(gpio_get_level(pin_number) == 0){ 
                    LAST_PRESS_UP_ = current_time;
                    xQueueSendFromISR(handlerQueue, &pin_number, NULL);
                }
            }
        }break;

        case _DOWN_:{
            if((current_time - LAST_PRESS_DOWN_) > DEBOUNCE_TIME){
                if(gpio_get_level(pin_number) == 0){
                    LAST_PRESS_DOWN_ = current_time;
                    xQueueSendFromISR(handlerQueue, &pin_number, NULL);
                }
            }
        }break;

        case _FINISH_:{
            if((current_time - LAST_PRESS_FINISH_) > DEBOUNCE_TIME){
                if(gpio_get_level(pin_number) == 0){
                    LAST_PRESS_FINISH_ = current_time;
                    xQueueSendFromISR(handlerQueue, &pin_number, NULL);
                }
            }
        }break;

        case _INIT_:
          if ((current_time - LAST_PRESS_INIT_) > DEBOUNCE_TIME) {
            if(gpio_get_level(pin_number) == 0){
                LAST_PRESS_INIT_ = current_time;
                if (inicio == false) {
                    inicio = true;
                    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                    xEventGroupSetBitsFromISR(event_group, ready, &xHigherPriorityTaskWoken);
                    xQueueSendFromISR(handlerQueue, &pin_number, NULL);
                    if (xHigherPriorityTaskWoken) {
                        portYIELD_FROM_ISR();
                    }
                }
            }
        }break;
    }
}



void task_input(void *params){

  int pin_number;

  while(true){

    //YO planeaba que no se bloqueara pero esto esta ocacionando bugs y errores en el programa con als tareas, por lo que lo mejor es bloquear hasta que reciba algo lam cola 
    if(xQueueReceive(handlerQueue,&pin_number,portMAX_DELAY)){

            //aquie enviamos por cola a la tarea principal
            // le estamos siicento lo mismo, que envie el pin pero no espere si no hay espacio, creo que el bloquear una tarea para un juego no es lo ideal 
            //xQueueSend(moveObjectQueue,&pin_number, pdMS_TO_TICKS(10));
      if (pin_number != _INIT_) {
        xQueueSend(moveObjectQueue, &pin_number, pdMS_TO_TICKS(10));
      }
    }   

  }

}

// void task_input(void *params){
//     int pin_number;

//     while(true){
//         if(xQueueReceive(handlerQueue, &pin_number, portMAX_DELAY)){
            
//             // Debug seguro fuera de ISR
//             printf("Recibido pin: %d, estado actual: %d\n", pin_number, gpio_get_level(pin_number));
            
//             if (pin_number != _INIT_) {
//                 if(xQueueSend(moveObjectQueue, &pin_number, pdMS_TO_TICKS(10)) == pdTRUE) {
//                     printf("Pin %d enviado a juego correctamente\n", pin_number);
//                 } else {
//                     printf("ERROR: No se pudo enviar pin %d a juego\n", pin_number);
//                 }
//             }
//         }   
//     }
// }




//tarea principal que controla el juego
void task_game_main(void *params){

    while(1){


        //aqui vamos a esperar a que el evento en que recibe la senial de active 
        xEventGroupWaitBits(event_group, ready, false, true,portMAX_DELAY);

        // Esperar un momento para que termine la salida del ESP-IDF
        //vTaskDelay(2000 / portTICK_PERIOD_MS); // 2 segundos
        int step;
        //con esto estamdos idnicnado que vamos a querer que la tarea se ejecuter siempre y cunado no se pierta
        bool game_over=false;  
        
        // Limpiar completamente la terminal
        printf("\033[2J\033[H\033[3J"); // Limpia pantalla y buffer
        printf("\033[?25l");      

        vehicle_x = SPAWN_OBJ;

        generate_obstacule();


        while(!game_over){



            //por aqui necesitmaos inicar obstaculos pero aun sin mostrar, estos se tiene que generar de manera random pero tiene que tener una distancia minima a mi vehiculo de almenos << mi vehiculo constara de 3 caracteres { <=> } obstaculos { # } de 6 a 5 espacios esto podria ser random? (por ahora solo lo pondere en 6 estatico despues veo si lo puedo genrear de una manera random) 

            //queiro que la tarea se ano bloquedante si hay espacio lo ingresa 
            //lo que estamos recibiedno es que boton se esta precionado, entonces, antes que nada debemos de verificar que se haya inicado el juego 
                
            //generate_obstacule();

            if(xQueueReceive(moveObjectQueue,&step,50)){

            //el juego ya inicio, la logica para que inice antes esta establecida < tal ves podamos usar el mismo botonn para inciio y fin, pero por ahora usarmeos 2 > 

            //haremos una prieba solo que imprima sin nada mas 
            //entonces esta funcion lo que hara es verificar que boton es el que se recibicio
            switch(step){

                        //de esta manera nos aseugramos que no reste o sume mas de lo que deberia porque estara saliendo de las lineas del juego 
                case _UP_:{
                    if(current_line > 0){
                    current_line--;
                                
                    }
                }break;

                case _DOWN_:{
                    if(current_line < 2){
                    current_line++;
                                
                    }
                }break;

                case _FINISH_:{
                    printf("\n\n=== HASTA LUEGO ===\n");
                    game_over =true;
                    break;
                }

            }   
        }


                //funcion que checa si se colisiono. 

                //depsues de ingresar un moviento debemos de verificar que no haya chocado nuestra nave 
                //para saber esto, primero necesitamos saber como ese supone que se sidtribuyen los obstaculos 
                

            move_vehicle();

            if(check_collision()){
                printf("\n\n=== ¡suerte a la proxima===\n");
                game_over = true;
            }
                    
            update_lanes();

            display_games();

            //SOLO PARA VERLO MEJOR 
            vTaskDelay(game_speed/portTICK_PERIOD_MS);
                
        }
        reset_game_state();

    }
}


void task_timer(void *params){

    static uint64_t last_seg=0;
    while(1){
        //solo se inica cunado el juego haya inciado 
        if(inicio){

            uint64_t current_time = esp_timer_get_time();

            //la funcion nos devuelte el timepi en uS porque lo que para 1Seg seria 1M 
            if((current_time - last_update_time) >= 1000000){
                elapsed_seconds++;
                last_update_time = current_time;

                //activamos el bit en el vgrupo de eventos 
                //indicamos que ya se acrivo el segundo
                xEventGroupSetBits(event_group,seg);

            }

        }
        else{
            last_update_time = esp_timer_get_time();
        }


        vTaskDelay(100/portTICK_PERIOD_MS);
    }


}



void task_timer_gpio(void *params){

    while(1){


        xEventGroupWaitBits(event_group,seg,true,true,portMAX_DELAY);

        uint32_t current_sec = elapsed_seconds;

        bool state_LD1 = ( current_sec & 1); 
        bool state_LD2 = (current_sec & 2);  
        bool state_LD3 = (current_sec & 4);  
        bool state_LD4 = (current_sec & 8);  
        bool state_LD5 = (current_sec & 16); 


        gpio_set_level(_LED1_,state_LD1);
        gpio_set_level(_LED2_,state_LD2);
        gpio_set_level(_LED3_,state_LD3);
        gpio_set_level(_LED4_,state_LD4);
        gpio_set_level(_LED5_,state_LD5);

    }
}




void reset_game_state() {
    vehicle_x = SPAWN_OBJ;
    current_line = 1;
    inicio = false;
    elapsed_seconds = 0;
    init_matrices();
    xEventGroupClearBits(event_group, ready);
    xQueueReset(moveObjectQueue);
    delimiter();
    printf("\t\t\n\n\n*************** PRESIONE EL BOTON PARA INICIAR DE NUEVO ***************\n\n\n");
    delimiter();
}

// void reset_game_state() {
//     vehicle_x = SPAWN_OBJ;
//     current_line = 1;
//     inicio = false;
//     elapsed_seconds = 0;
//     init_matrices();
//     xEventGroupClearBits(event_group, ready);
//     xQueueReset(moveObjectQueue);
    
//     // Limpiar estadísticas de debug
//     reset_debug_counters();
    
//     delimiter();
//     printf("\t\t\n\n\n*************** PRESIONE EL BOTON PARA INICIAR DE NUEVO ***************\n\n\n");
//     delimiter();
// }




//funcion terminada 
//esta funcion establece en que posicion dentro de nuestro liezno aparecera los obstaculos 
void generate_obstacule(void){
    //limamos la pantalla, por ahpora vamos a genrear en un inicio 
    memset((void*)canvas,' ',sizeof(canvas)); //limpiamos la pantalla 
    memset((void*)obstacle, 0, sizeof(obstacle)); //limpamos la matriz que nos dice en donde estan los obstaculos
    

    //el carro iniicara en la posicion 5 de nutro canvas en medio 

    for(int x = WIDTH_SCRE - 35; x<WIDTH_SCRE; x+=6){

        if(x < WIDTH_SCRE){

            /* algortimos que dice en donde va a aparecer el objstaculo: 
                1-verificar que se enceuntre dentro del ancho y alto de la pantall 
                2-genrear un nuermo random de 0 - 2 que seran los 3 posibles carriles de nuestro juego 
                3-verificar que tanto arriba como abajo si es la opcion haya esapcio donde nuestro vechiculo pueda pasar
                3.1 -> si el rand() nos da 0 verificar que abajo haya el espacio establecido de al menos (SPACE_OBJ) en este caso seria de 5 esto podriamos cambiarlos 
                3.2 -> si el rand() nos da 1 que arriba o abajo haya esos 5 espacios pueden haber arriba o abajo uno mas juntito pero al menos uno de los 2 debe de tener sufueinete esapcio para suibir 
                3.3 -> si rand() nos da 2 debe de haber espacio arriba donde nuestro vehiculo pueda pasar. 
            */
            int lane = rand()%3; // an algunso   de los 3 paosicones 0 - 1 - 2 de manera random 
            //aqui la condicion en donde debemos revisar tanto arriba como aabjo porque si no habra en todos lados. 
            //antes de poner el obstaculo debo de vericiar que en los proximos 5 bloques al menso este el espacio si no esta pasamos este rand

            if(check_space_availabe(lane,x) == 1){
                obstacle[lane][x] = 1;
            }
        }
    }
}






void update_lanes(){

    //creo que la condicion deberia de ir aqui y recibimos la posicion, 
    memset((void*)canvas, ' ', sizeof(canvas));

    char time_buffer[25];
    sprintf(time_buffer, "Tiempo: %lu s", elapsed_seconds);
    memcpy(&canvas[0][1], time_buffer, strlen(time_buffer)); 

    for(int x =0; x< WIDTH_SCRE; x++){

        if(obstacle[0][x]) canvas[LANE_TOP][x]= 'X';
        if(obstacle[1][x]) canvas[LANE_CENTER][x]= 'X';
        if(obstacle[2][x]) canvas[LANE_BOTTOM][x]= 'X';
    }


    int vehicle_lane;
    switch(current_line) {
        case 0: vehicle_lane = LANE_TOP; break;
        case 1: vehicle_lane = LANE_CENTER; break;  
        case 2: vehicle_lane = LANE_BOTTOM; break;
        default: vehicle_lane = LANE_CENTER;
    }
    
    // Verificar que el vehículo esté dentro de los límites antes de colocarlo
    if (vehicle_x >= 0 && vehicle_x + 2 < WIDTH_SCRE) {
        canvas[vehicle_lane][vehicle_x] = '<';
        canvas[vehicle_lane][vehicle_x + 1] = '=';
        canvas[vehicle_lane][vehicle_x + 2] = '>';
    }

}




//funcion terminadas
void display_games(void){

    printf("\033[2J\033[H");


    for(int y= 0; y < HEIGTH_SCRE; y++){

        for(int x = 0; x< WIDTH_SCRE; x++){
            putchar(canvas[y][x]);
        }
        putchar('\n');
    }
    delimiter();

    fflush(stdout);


}


//############funcion terminada 

//esta funcion recibira en <row> -> 0 - 1 - 3
//en el parametro < ROW > vamos a pasar una de las 3 macros y verificaremos con un switch 
//regresa 1 o 0 
//0 para NO insertarlo 
//1 para SI insertarlo 
int check_space_availabe(int row,int col){

    //debemos de considerar para el final is hay espacios para ponder el obstaculo 
    /*
    los que estoy buscadno en el EJE X -> columnas 
    
    */
    switch(row){

        case 0:{
            //porque menor que el ancho de la pantalal?, porque realmente no se en que parte aparecera nuestro, pero aun no se si ponde otra variable para que esta salga o solo un break
            
            // x < col + space_needed && x < WIDTH_SCRE <- con esta condicion es la buena solo se va a ciclar los 5 espacios que son necesariospara decidiir si si va o no va. 
            for(int x = col; x < col + SPACE_OBJ && x < WIDTH_SCRE;x++){
                //en la matriz de obstaculo
                //ahora esta recorriendo en el eje x en las columnas busncado si en el esapcio de abajo 
                //verifica si en ese espacio ya hay algun obstaculo entre ese rango si no es asi puede ser proecesado 
                if(obstacle[1][x] == 1){
                    return 0;
                }

            }
            return 1;
        }break;


        case 1:{

            //aqui debemos de verificar tanto arriba como abajo, almenos unos de los 2 debe de cumplir con la condicion 
            bool top_free= true;
            bool bottom_free = true;

            for(int x = col; x < col + SPACE_OBJ && x < WIDTH_SCRE;x++){
                //en la matriz de obstaculo

                if(obstacle[2][x] == 1){
                    bottom_free = false;
                }   
                if(obstacle[0][x]== 1){
                    top_free = false;
                }
            }

            return (bottom_free || top_free)? 1 : 0;
        }break;

        case 2:{

            for(int x = col; x < col + SPACE_OBJ && x < WIDTH_SCRE;x++){
                //en la matriz de obstaculo
                if(obstacle[1][x] ==1 ){
                    return 0;
                }
            }
            return 1;
        }break; 
    }
    return 0; //devuelve que no para que no se corrupta la pantalla del jeugo 
}



// Función para mover el vehículo automáticamente
void move_vehicle(void){
    vehicle_x++;
    
    // Cuando el vehículo llega al final de la pantalla
    if (vehicle_x >= WIDTH_SCRE - 3) { // -3 porque el vehículo ocupa 3 espacios
        vehicle_x = SPAWN_OBJ; // Reiniciar posición
        generate_obstacule(); // Generar nuevos obstáculos
    }
}


// int check_collision(void) {
//     int vehicle_lane;
    
//     switch(current_line) {
//         case 0: vehicle_lane = LANE_TOP; break;
//         case 1: vehicle_lane = LANE_CENTER; break;
//         case 2: vehicle_lane = LANE_BOTTOM; break;
//         default: return 0;
//     }
    
//     // Verificar colisión en las 3 posiciones del vehículo
//     for(int i = 0; i < 3; i++) {
//         int check_x = vehicle_x + i;
//         if (check_x >= 0 && check_x < WIDTH_SCRE) {
//             // Verificar si hay obstáculo en esa posición
//             int lane_index = (vehicle_lane == LANE_TOP) ? 0 : 
//                             (vehicle_lane == LANE_CENTER) ? 1 : 2;
//             if (obstacle[lane_index][check_x] == 1) {
//                 return 1; // Colisión detectada
//             }
//         }
//     }
    
//     return 0; // No hay colisión
// }

// main.c -> Reemplaza la función antigua con esta
int check_collision(void) {
    // La variable 'current_line' (0, 1, o 2) ya es el índice de fila
    // que necesitamos para la matriz 'obstacle'. No se necesitan más conversiones.

    // Recorremos las 3 partes del vehículo
    for (int i = 0; i < 3; i++) {
        int check_x = vehicle_x + i; // La coordenada X de cada parte del vehículo

        // Nos aseguramos de no revisar fuera de los límites de la pantalla
        if (check_x >= 0 && check_x < WIDTH_SCRE) {
            // Verificamos directamente si en la matriz de obstáculos hay un '1'
            // en nuestra línea actual y en la posición X del vehículo.
            if (obstacle[current_line][check_x] == 1) {
                return 1; // ¡Colisión encontrada!
            }
        }
    }
    
    return 0; // Si el bucle termina, no hubo colisión.
}



//funcion de apoyo
void delimiter(void){

    for(int i=0; i<WIDTH_SCRE ; i++){
        putchar('=');
    }
    putchar('\n');

}


void init_matrices(void) {
    // Inicializar obstacle en 0
    memset((void*)obstacle, 0, sizeof(obstacle));
    
    // Inicializar canvas en 0 
    memset((void*)canvas, ' ', sizeof(canvas));
}



//########################################
// void debug_stats_task(void *params){
//     uint32_t last_up = 0, last_down = 0, last_init = 0, last_finish = 0;
    
//     while(1) {
//         // Mostrar contadores de ISR
//         printf("=== ESTADISTICAS ISR ===\n");
//         printf("UP: %lu (+%lu)  DOWN: %lu (+%lu)\n", 
//                debug_isr_count_up, debug_isr_count_up - last_up,
//                debug_isr_count_down, debug_isr_count_down - last_down);
//         printf("INIT: %lu (+%lu)  FINISH: %lu (+%lu)\n",
//                debug_isr_count_init, debug_isr_count_init - last_init,
//                debug_isr_count_finish, debug_isr_count_finish - last_finish);
//         printf("Ultimo pin ISR: %lu, Estado: %lu\n", debug_last_pin, debug_pin_state);
        
//         // Mostrar estado actual de pines
//         printf("Estados actuales - UP:%d DOWN:%d INIT:%d FINISH:%d\n",
//                gpio_get_level(_UP_), gpio_get_level(_DOWN_),
//                gpio_get_level(_INIT_), gpio_get_level(_FINISH_));
//         printf("========================\n");
        
//         // Guardar valores para calcular incrementos
//         last_up = debug_isr_count_up;
//         last_down = debug_isr_count_down;
//         last_init = debug_isr_count_init;
//         last_finish = debug_isr_count_finish;
        
//         vTaskDelay(3000 / portTICK_PERIOD_MS); // Cada 3 segundos
//     }
// }

