#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
//casi simepre estas seras las bibliotecas basicas de todo programa para el ESP32

#include<string.h>
#include <driver/gpio.h>
#include <time.h>
#include <esp_timer.h>
#include <freertos/event_groups.h>
#include "game.h" //mi libreria 

//matriz que define la posicion de los obstaculos 
//volatile int obstacle[]][];
//las filas estan declaradas en el primer corcehte y las columnas en el segundo corchete 
volatile int obstacle[3][WIDTH_SCRE];
//matriz princola 
//volatile int obstacle[HEIGTH_SCRE][3];
//volatile char canvas[WIDTH_SCRE][HEIGTH_SCRE];

volatile char canvas[HEIGTH_SCRE][WIDTH_SCRE];
//###########################
//debemos de cambiar donde manerajamos los vectores de una manera inversa 
//array[FILAS_Y][COLUMLAS_X]
// FILAS COORDENADA Y -> priemr corcehte 
//COLUMNAS COORDENAAS X -> segundo crocehte 


//###########################



//variables para controlar el antirebote 
static uint64_t LAST_PRESS_UP_ = 0;
static uint64_t LAST_PRESS_DOWN_ = 0;
static uint64_t LAST_PRESS_INIT_ = 0; //aunque este solo se precionara 1 vez en todo el programa 

volatile int current_line=1; //estabelcemos que 0 - arriba - 1 -centro y 2 - abajo  <pero debemos de establecer alguna condicionan en donde por quejemplo si le da varias veces al arriba o abajo no ceremente mas de lo que deberia si no que se quede donde mismo. >, el voaltin porque su valor va a estar cambiando entonces C no lo optimiza. 


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
const int seg = BIT1;

void app_main(void)
{
    printf("configUSE 16 bits TICK %d\n",configUSE_16_BIT_TICKS);
    //esta cola maneja los botones 

    //creamos el eventos de grupos 

    event_group = xEventGroupCreate();



    //inicio del programa, antes de el son coasa que estoy probando 

    handlerQueue= xQueueCreate(10, sizeof(uint32_t));
    moveObjectQueue = xQueueCreate(10,sizeof(uint32_t));

    srand(time(NULL)); //inicamos la semilla

   
    init_matrices(); 
    
    init_gpio();
    type_intr();
    //tarea que maneja que boton es el que se preciono, ese lo recibe de la ISR
    //la de mayor prioridad es la que controla los botoens de entrada y la de una de menos seria lo que se muestra en pantalla y la de menos prioridad seria la que muestra el tiempo en LEDs, con esos no hya tanto aguite si funciona al millon. 
    xTaskCreate(task_input,"task_input",2048,NULL,10,NULL);
    xTaskCreate(task_game_main,"task_main",4096,NULL,9,NULL); //asigmaos un poco mas de espacio por que vamos a llamar a funciones para que no todo este dentro de ahi

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
    //salida
    gpio_reset_pin(_LED1_);
    gpio_reset_pin(_LED2_);
    gpio_reset_pin(_LED3_);
    gpio_reset_pin(_LED4_);
    gpio_reset_pin(_LED5_);


    gpio_set_direction(_UP_,GPIO_MODE_INPUT);
    gpio_set_direction(_DOWN_,GPIO_MODE_INPUT);
    gpio_set_direction(_INIT_,GPIO_MODE_INPUT);
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

                    LAST_PRESS_DOWN_ = current_time;
                    //el ultimo paremtro es la prioridad para enviar este dato, pero si le pone NULL esta bien dado que que un parametro opcional solo si queremos darle prioridad al envio de x dato por la cola. 
                    xQueueSendFromISR(handlerQueue,&pin_number,NULL);

                }

            }break;

            case _INIT_:{

                if((current_time - LAST_PRESS_INIT_) > DEBOUNCE_TIME){

                    //printf("boton incio\n");

                    LAST_PRESS_INIT_ = current_time;

                    if(inicio == false){
                        inicio = true;
                        // Despierta a las tareas que esperan el inicio del juego
                        //xEventGroupSetBits(event_group,ready);
                        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                        xEventGroupSetBitsFromISR(event_group, ready, &xHigherPriorityTaskWoken);
                        //portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                        if(xHigherPriorityTaskWoken){
                            portYIELD_FROM_ISR();
                        }

                    }
                }

            }break;



        }

    }
}



void task_input(void *params){

    int pin_number;

    while(true){

        //YO planeaba que no se bloqueara pero esto esta ocacionando bugs y errores en el programa con als tareas, por lo que lo mejor es bloquear hasta que reciba algo lam cola 
        if(xQueueReceive(handlerQueue,&pin_number,portMAX_DELAY)){

            //aquie enviamos por cola a la tarea principal
            // le estamos siicento lo mismo, que envie el pin pero no espere si no hay espacio, creo que el bloquear una tarea para un juego no es lo ideal 
            xQueueSend(moveObjectQueue,&pin_number,0);

        }   

    }

}



//tarea principal que controla el juego
void task_game_main(void *params){
    int step;
    //con esto estamdos idnicnado que vamos a querer que la tarea se ejecuter siempre y cunado no se pierta
    bool game_over=false;
    int frame=0;


    //aqui vamos a esperar a que el evento en que recibe la senial de active 
    xEventGroupWaitBits(event_group, ready, false, true,portMAX_DELAY);

    // Esperar un momento para que termine la salida del ESP-IDF
    vTaskDelay(2000 / portTICK_PERIOD_MS); // 2 segundos
    
    // Limpiar completamente la terminal
    printf("\033[2J\033[H\033[3J"); // Limpia pantalla y buffer
    printf("\033[?25l");      

    while(!game_over){


        // int position_y = HEIGTH_SCRE / 2 ;
        //     int position_x = 5;

        //     for(int i=0; i< 20; i++){

        //         //limpiamos el lienzo virtual con espacios

        //         memset(canvas,' ', sizeof(canvas));

        //         if(position_x + 3 < WIDTH_SCRE){
        //             canvas[position_y][position_x] = '<';
        //             canvas[position_y][position_x + 1 ] = '<';
        //             canvas[position_y][position_x + 2] = '<';
        //         }

        //         position_x++;
        //         printf("\033[2J\033[H"); // Limpia la terminal

        //         delimiter();

        //         for(int i=0; i<HEIGTH_SCRE;i++){

        //             printf("%.*s\n",WIDTH_SCRE,canvas[i]);

        //         }


        //         delimiter();

        //         fflush(stdout);
        //         vTaskDelay(1000/portTICK_PERIOD_MS);
            


        //     }
        

        //por aqui necesitmaos inicar obstaculos pero aun sin mostrar, estos se tiene que generar de manera random pero tiene que tener una distancia minima a mi vehiculo de almenos << mi vehiculo constara de 3 caracteres { <=> } obstaculos { # } de 6 a 5 espacios esto podria ser random? (por ahora solo lo pondere en 6 estatico despues veo si lo puedo genrear de una manera random) 

        //queiro que la tarea se ano bloquedante si hay espacio lo ingresa 
        //lo que estamos recibiedno es que boton se esta precionado, entonces, antes que nada debemos de verificar que se haya inicado el juego 
        
        generate_obstacule();

        if(xQueueReceive(moveObjectQueue,&step,100)){

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
                }

            }   
        }


        //depsues de ingresar un moviento debemos de verificar que no haya chocado nuestra nave 
        //para saber esto, primero necesitamos saber como ese supone que se sidtribuyen los obstaculos 
        



        update_lanes();
        display_games();

        for(int y = 0; y<3; y++ ){
            for(int x = 0; x< WIDTH_SCRE; x++){
                printf("%d", obstacle[y][x]);
            }
            putchar('\n');
        }

        vTaskDelay(5000/portTICK_PERIOD_MS);
 
    }

}


// void task_timer(void *params){}

//esta funcion establece en que posicion dentro de nuestro liezno aparecera los obstaculos 
// void generate_obstacule(void){

//     //limamos la pantalla, por ahpora vamos a genrear en un inicio 

//     memset(canvas,' ',sizeof(canvas)); //limpiamos la pantalla 
//     memset((void*)obstacle, 0, sizeof(obstacle)); //limpamos la matriz que nos dice en donde estan los obstaculos
    

//     //el carro iniicara en la posicion 5 de nutro canvas en medio 

//     for(int x = WIDTH_SCRE - 35; x<WIDTH_SCRE; x+=6){

//         if(x < WIDTH_SCRE){

//             /* algortimos que dice en donde va a aparecer el objstaculo: 
//                 1-verificar que se enceuntre dentro del ancho y alto de la pantall 
//                 2-genrear un nuermo random de 0 - 2 que seran los 3 posibles carriles de nuestro juego 
//                 3-verificar que tanto arriba como abajo si es la opcion haya esapcio donde nuestro vechiculo pueda pasar
//                 3.1 -> si el rand() nos da 0 verificar que abajo haya el espacio establecido de al menos (SPACE_OBJ) en este caso seria de 5 esto podriamos cambiarlos 
//                 3.2 -> si el rand() nos da 1 que arriba o abajo haya esos 5 espacios pueden haber arriba o abajo uno mas juntito pero al menos uno de los 2 debe de tener sufueinete esapcio para suibir 
//                 3.3 -> si rand() nos da 2 debe de haber espacio arriba donde nuestro vehiculo pueda pasar. 
//             */
//             int lane = rand()%3; // an algunso   de los 3 paosicones 0 - 1 - 2 de manera random 
//             //aqui la condicion en donde debemos revisar tanto arriba como aabjo porque si no habra en todos lados. 
//             //antes de poner el obstaculo debo de vericiar que en los proximos 5 bloques al menso este el espacio si no esta pasamos este rand

//             if(check_space_availabe(lane,x) == 1){
//                 obstacle[lane][x] = 1;
//             }
//         }
//     }
// }

void generate_obstacule(void){
    printf("=== GENERATE_OBSTACULE DEBUG ===\n");
    
    // Limpiar matrices
    memset(canvas, ' ', sizeof(canvas)); 
    memset((void*)obstacle, 0, sizeof(obstacle));
    
    printf("Canvas limpiado, obstacle limpiado\n");
    
    // Generar obstáculos
    for(int x = WIDTH_SCRE - 35; x < WIDTH_SCRE; x += 6){
        if(x < WIDTH_SCRE){
            int lane = rand() % 3;
            printf("Intentando poner obstáculo en lane=%d, x=%d\n", lane, x);
            
            if(check_space_availabe(lane, x) == 1){
                obstacle[lane][x] = 1;
                printf("✓ Obstáculo colocado en lane=%d, x=%d\n", lane, x);
            } else {
                printf("✗ No se pudo colocar obstáculo en lane=%d, x=%d\n", lane, x);
            }
        }
    }
    
    // Verificar qué hay en obstacle después
    printf("\n=== MATRIZ OBSTACLE DESPUÉS ===\n");
    for(int y = 0; y < 3; y++){
        printf("Lane %d: ", y);
        for(int x = 0; x < WIDTH_SCRE; x++){
            if(obstacle[y][x] == 1) printf("[%d] ", x);
        }
        printf("\n");
    }
    printf("=== FIN GENERATE_OBSTACULE ===\n\n");
}




// void update_lanes(){


//     for(int x =0; x< WIDTH_SCRE; x++){

//         if(obstacle[LANE_TOP][x]) canvas[LANE_TOP][x]= 'X';
//         if(obstacle[LANE_CENTER][x]) canvas[LANE_CENTER][x]= 'X';
//         if(obstacle[LANE_BOTTOM][x]) canvas[LANE_BOTTOM][x]= 'X';
//     }
// }

void update_lanes(){
    printf("=== UPDATE_LANES DEBUG ===\n");
    
    int obstacles_found = 0;
    for(int x = 0; x < WIDTH_SCRE; x++){
        if(obstacle[LANE_TOP][x]) {
            canvas[LANE_TOP][x] = 'X';
            printf("Colocando X en canvas[%d][%d]\n", LANE_TOP, x);
            obstacles_found++;
        }
        if(obstacle[LANE_CENTER][x]) {
            canvas[LANE_CENTER][x] = 'X';
            printf("Colocando X en canvas[%d][%d]\n", LANE_CENTER, x);
            obstacles_found++;
        }
        if(obstacle[LANE_BOTTOM][x]) {
            canvas[LANE_BOTTOM][x] = 'X';
            printf("Colocando X en canvas[%d][%d]\n", LANE_BOTTOM, x);
            obstacles_found++;
        }
    }
    
    printf("Total obstáculos colocados en canvas: %d\n", obstacles_found);
    printf("=== FIN UPDATE_LANES ===\n\n");
}





// void display_games(void){

//     printf("\033[2J\033[H"); // Limpiar terminal
//     delimiter();


//     for(int y= 0; y < HEIGTH_SCRE; y++){

//         for(int x = 0; x< WIDTH_SCRE; x++){
//             putchar(canvas[y][x]);
//         }
//         putchar('\n');
//     }
//     delimiter();


// }

// void display_games(void){
//     printf("\033[2J\033[H"); // Limpiar terminal
//     delimiter();

//     for(int y = 0; y < HEIGTH_SCRE; y++){
//         for(int x = 0; x < WIDTH_SCRE; x++){
//             // Asegurar que solo imprimimos caracteres válidos
//             char c = canvas[y][x];
//             if(c == '\0') c = ' '; // Convertir nulos a espacios
//             putchar(c);
//         }
//         putchar('\n');
//     }
//     delimiter();
// }

void display_games(void){
    printf("=== DISPLAY_GAMES DEBUG ===\n");
    
    // Mostrar solo las líneas donde deberían estar los obstáculos
    printf("Solo mostrando las 3 líneas de juego:\n");
    delimiter();
    
    // Línea superior (LANE_TOP = 1)
    printf("TOP   : ");
    for(int x = 0; x < WIDTH_SCRE; x++){
        putchar(canvas[LANE_TOP][x]);
    }
    printf("\n");
    
    // Línea central (LANE_CENTER = 4)  
    printf("CENTER: ");
    for(int x = 0; x < WIDTH_SCRE; x++){
        putchar(canvas[LANE_CENTER][x]);
    }
    printf("\n");
    
    // Línea inferior (LANE_BOTTOM = 7)
    printf("BOTTOM: ");
    for(int x = 0; x < WIDTH_SCRE; x++){
        putchar(canvas[LANE_BOTTOM][x]);
    }
    printf("\n");
    
    delimiter();
    printf("=== FIN DISPLAY_GAMES ===\n\n");
}



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

        case LANE_TOP:{
            //porque menor que el ancho de la pantalal?, porque realmente no se en que parte aparecera nuestro, pero aun no se si ponde otra variable para que esta salga o solo un break
            
            // x < col + space_needed && x < WIDTH_SCRE <- con esta condicion es la buena solo se va a ciclar los 5 espacios que son necesariospara decidiir si si va o no va. 
            for(int x = col; x < col + SPACE_OBJ && x < WIDTH_SCRE;x++){
                //en la matriz de obstaculo
                //ahora esta recorriendo en el eje x en las columnas busncado si en el esapcio de abajo 
                //verifica si en ese espacio ya hay algun obstaculo entre ese rango si no es asi puede ser proecesado 
                if(obstacle[LANE_CENTER][x] == 1){
                    return 0;
                }

            }
            return 1;
        }break;


        case LANE_CENTER:{

            //aqui debemos de verificar tanto arriba como abajo, almenos unos de los 2 debe de cumplir con la condicion 
            bool top_free= true;
            bool bottom_free = true;

            for(int x = col; x < col + SPACE_OBJ && x < WIDTH_SCRE;x++){
                //en la matriz de obstaculo

                if(obstacle[LANE_BOTTOM][x] == 1){
                    bottom_free = false;
                }   
                if(obstacle[LANE_TOP][x]== 1){
                    top_free = false;
                }
            }

            return (bottom_free || top_free)? 1 : 0;
        }break;

        case LANE_BOTTOM:{

            for(int x = col; x < col + SPACE_OBJ && x < WIDTH_SCRE;x++){
                //en la matriz de obstaculo
                if(obstacle[LANE_CENTER][x] ==1 ){
                    return 0;
                }
            }
            return 1;
        }break; 
    }
    return 0; //devuelve que no para que no se corrupta la pantalla del jeugo 
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

