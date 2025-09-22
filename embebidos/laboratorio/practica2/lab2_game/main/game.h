#ifndef GAME_H
#define GAME_H

//definamos nuestors pines de entrada y de salida <esto es lo mas sencillo por lo generlsa>

//entrada
#define _UP_ 18
#define _DOWN_ 19
#define _FINISH_ 21 
#define _INIT_ 22

//salidas
#define _LED1_ 13
#define _LED2_ 14
#define _LED3_ 27
#define _LED4_ 26
#define _LED5_ 25

//macros que nos ayudaran a controlar el antirebote de los botones 
#define DEBOUNCE_TIME 200000 
#define WIDTH_SCRE 50
#define HEIGTH_SCRE 10 //el alto de la "pantalla" del juego, esto para fromar un vector para poder representar le juego 

//creo que debemos de definir como los carriles por los cuales aparecerena los obstaculos y en los que nuestro carro o objeto va a aparecer, los uclaes seran 3 carriles 

//digamops que esto es el centro de mi carril, en estas mismas posiciones es solamente en donde se va a apoder aparecer los obstaculos 
#define LANE_TOP 1      // Carril superior
#define LANE_CENTER 4   // Carril central
#define LANE_BOTTOM 7   // Carril inferior

#define SPAWN_OBJ 5 //el spawn de nuestro vehiculo
#define SPACE_OBJ 6 //el esapcio minimo que debe de haber entre objetos 


extern volatile bool inicio;
//mi carro iniciara en la posicion 5 de nuestro canvas 




void init_gpio(void);

//funcion que controla la interrupcion de cunado se preciona un boton 
void type_intr(void);
void gpio_isr_handler(void *args);

//tareas 
void task_input(void *params);
//esta esta la tarea que controlara la visulaiuacion y el juego principal
void task_game_main(void *params);
void reset_game_state();
//tarea que utiliza el grupo de eventos

void task_timer(void *params); //tarea que mocstrara por pantalla los segundos transcurridos
void task_timer_gpio(void *params);


//funcion que genera los obstaculos de manera aleatorio
void generate_obstacule(void);
void update_lanes(void);
void display_games(void);
//funcion para reocrrer el vechiulo


//funcion que checa que se cumplan la condicion de insertar un obstaculo

int check_space_availabe(int row,int col);

void move_vehicle(void);
int check_collision(void);


void delimiter(void);
void init_matrices(void);

//################################
// void debug_stats_task(void *params);


#endif
