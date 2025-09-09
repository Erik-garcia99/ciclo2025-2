#ifndef GAME_H
#define GAME_H

//definamos nuestors pines de entrada y de salida <esto es lo mas sencillo por lo generlsa>

//entrada
#define _UP_ 13
#define _DOWN_ 14
#define _FINISH_ 27
#define _INIT_ 26

//salidas
#define _LED1_ 4
#define _LED2_ 18
#define _LED3_ 19
#define _LED4_ 21
#define _LED5_ 22

//macros que nos ayudaran a controlar el antirebote de los botones 
#define DEBOUNCE_TIME 200000
#define WIDTH_SCRE 90
//colas 

QueueHandle_t handlerQueue;



void init_gpio(void);

//funcion que controla la interrupcion de cunado se preciona un boton 
void type_intr(void);
void gpio_isr_handler(void *args);


//funcion que solo imprimre lineas demilimintado el juego 

void delimiter(void);
#endif