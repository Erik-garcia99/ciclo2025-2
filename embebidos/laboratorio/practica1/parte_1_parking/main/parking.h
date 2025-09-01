//archivo header, donde se tendras la declaraciones de las funciones, estrucutras, TAD para la practica 1 << actualemnte la parte 1 >>
//definidciones del preprocesador. 
#ifndef parking_h
#define parking_h

//definamos nuestros pines de entrada y de salida 
//necesitamos los 4 pines para la entrada de botones
//GPIO's a usar . ENTRADAS - 18,19,21,22
//GPIO salidas: 13,14,27,26

//ENTRADAS
#define UN_PESO_IN 18
#define CINCO_PESO_IN 19
#define DIEZ_PESO_IN 21
#define VEINTE_PESO_IN 22


//GPIO de cambio
//SALIDAS
#define UN_PESO_OUT 13
#define CINCO_PESO_OUT 14
#define DIEZ_PESO_OUT 27
#define VEINTE_PESO_OUT 26 

//GPIO DE SALIDA, ERROR, INTRODUJO MAS DE LO PERMITIDO
#define ERR_EXCESS_MONEY 33
#define DEBOUNCE_TIME 200000 
#define TICKET_LED 25

//estados 

typedef enum {
    STATE_INITIAL=0,
    STATE_COLLECTING,
    STATE_PAID,
    STATE_OVERPAID,
    STATE_GIVING_CHANGE,
    STATE_RECEIPT
} vending_state_t;


//parte 2
//maquina de estados de la barrera 
/*
-STOPPED_STATE -> estado donde el carro esta detenido esperando a que se levante la barrera. 

*/



//comuncion entre tareas, por medio de las colas 
QueueHandle_t handlerQueue, moneyQueue;
extern vending_state_t current_state;

//funciones 

//funcion que incia-configura los GPIO
void init_GPIO(void);

void vending_machine(int money_acc);
void give_change(void);
//funcion del efecto de que levante 




//funcion de creacion de interrupcioens por GPIO-IN
/*static void IRAM_ATTR gpio_UN_isr_handler(void *arg);
static void IRAM_ATTR gpio_CINCO_isr_handler(void *arg);
static void IRAM_ATTR gpio_DIEZ_isr_handler(void *arg);
static void IRAM_ATTR gpio_VEINTE_isr_handler(void *arg);
*/
//manejador de las interupciones
void gpio_isr_handler(void *args);

void TYPE_INT(void);

//tareas
void PROCESS_INTR(void *params);

void MONEY_MANAGER(void *params);





#endif


