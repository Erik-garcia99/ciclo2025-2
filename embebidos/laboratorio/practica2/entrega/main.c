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

//colas
//debemos de crear una cola en donde enviamos a nuestra tarea pincipal que es la que debe de animar si el objeto va hacia arriba o hacia abajo 
QueueHandle_t handlerQueue, moveObjectQueue;

//creamos el grupo de eventos
EventGroupHandle_t event_group;


volatile bool inicio = false; //para que no la optmice 
const int ready = BIT0;
const int seg = BIT1; // segun yo este sera para cunado pase 1 segundo, 

volatile uint32_t elapsed_seconds = 0; // varibale global para el timepo 
volatile uint64_t last_update_time = 0;




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

    
    xTaskCreate(task_input,"task_input",2048,NULL,10,NULL);
    xTaskCreate(task_game_main,"task_main",4096,NULL,9,NULL); //asigmaos un poco mas de espacio por que vamos a llamar a funciones para que no todo este dentro de ahi

    //tarea de cotrol de timepo 

    xTaskCreate(task_timer,"task_timer",2048,NULL,8,NULL);
    xTaskCreate(task_timer_gpio,"task_timer_gpio",2048,NULL,7,NULL);

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



    gpio_set_intr_type(_UP_, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(_DOWN_, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(_INIT_, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(_FINISH_, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);

    gpio_isr_handler_add(_UP_,gpio_isr_handler,(void *)_UP_);
    gpio_isr_handler_add(_DOWN_,gpio_isr_handler,(void *)_DOWN_);
    gpio_isr_handler_add(_INIT_,gpio_isr_handler,(void *)_INIT_);
    gpio_isr_handler_add(_FINISH_,gpio_isr_handler,(void *)_FINISH_);

}


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

   
    if(xQueueReceive(handlerQueue,&pin_number,portMAX_DELAY)){
      if (pin_number != _INIT_) {
        xQueueSend(moveObjectQueue, &pin_number, pdMS_TO_TICKS(10));
      }
    }   

  }

}


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



            if(xQueueReceive(moveObjectQueue,&step,50)){

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




void generate_obstacule(void){
    //limamos la pantalla, por ahpora vamos a genrear en un inicio 
    memset((void*)canvas,' ',sizeof(canvas)); //limpiamos la pantalla 
    memset((void*)obstacle, 0, sizeof(obstacle)); //limpamos la matriz que nos dice en donde estan los obstaculos
    

    //el carro iniicara en la posicion 5 de nutro canvas en medio 

    for(int x = WIDTH_SCRE - 35; x<WIDTH_SCRE; x+=6){

        if(x < WIDTH_SCRE){

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


int check_space_availabe(int row,int col){
    switch(row){

        case 0:{
          
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




void move_vehicle(void){
    vehicle_x++;
    
    
    if (vehicle_x >= WIDTH_SCRE - 3) { 
        vehicle_x = SPAWN_OBJ; 
        generate_obstacule(); 
    }
}


int check_collision(void) {

    for (int i = 0; i < 3; i++) {
        int check_x = vehicle_x + i; 

    
        if (check_x >= 0 && check_x < WIDTH_SCRE) {
    
            if (obstacle[current_line][check_x] == 1) {
                return 1; 
            }
        }
    }
    
    return 0; 
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
