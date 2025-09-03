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

//GPIO aguja - estado vehiculo 
//salida aguha 

#define _DOWN_ 33
#define _MIDDLE_ 32
#define _UP_ 23

//entradas 
#define init_vehi 22
#define pass_vehi 21


//estados 

typedef enum {
    STATE_INITIAL=0,
    STATE_COLLECTING,
    STATE_PAID,
    STATE_OVERPAID,
    STATE_GIVING_CHANGE,
    STATE_RECEIPT
} vending_state_t;

/*#####################################################################




parte 2
maquina de estados de la barrera 

-> requiere que le agregemos 2 botones < 1 boton - carro en espera a cruzar 

2 boton- el carro termino de cruzar

3 leds - presentado, abajo, en medio y arriba de la aguja.

- esta es otra tarea que se deberia de estar hacinedo?, muy posiblemnete, aunque son procesos que no es necesario que se esnte hacinedo a la par,pero igual no seria mala idea. 

-STOPPED_STATE -> estado donde el carro esta detenido esperando a que se levante la barrera. 

-STATE_TRANSITION -> representa el estado en el cual se esta levantando la aguja < creo que seria buena idea poder interrupitr este proceso y volver al estado inical > 

-STATE_COMPLETELY_UP -> estado donde la aguja esta completamente arriba 

-STATE_FINAL -> estado donde se esta bajando < de igual forma de este estado puede ser interrumpido y volver a subir la aguja volviendo al estado anterior a este > 

-> la aguja tiene 3 estados por los que tiene que pasar, primero de abajo hacia arriba, seria abajo -> en medio -> arriba, el sentido inverso cunado la aguja esta arriba, por loq ue estos son otros 3 estados, con los cuales podemos interrumpir dependinedo si se encuentra en la trnasicion asendente o desendente. 


-> esta maquian de estado seria algo muy parecido a la que ya tenemos, solo que se podra activar, cunado el estado del cobro sea que ya se cobro completo sin cambio o con cambio. 

//##################################################################
*/

//#########################################
//estados de aguja de salida -> PT2
typedef enum{
    STOPPED_STATE = 0,
    STATE_TRANSITION,
    STATE_COMPLETELY_UP,
    STATE_FINAL
}gate_state_t;
//#########################################
//PT2 -> estados de la aguja, esto seria algo importante para poder interrumpir el proceso de una trnaucion tanto asendente como desendente. 

typedef enum{
    STATE_DOWN=0;
    STATE_MIDDLE,
    STATE_UP
}state_needle_t;


//#########################################


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

//#########################################
//funciones de la parte 2 - funcionamiento de la aguja 

/*
que funciones vamos a necesitar?

-> la funcion la que estara procesando los vehiculos debe de esperar a que precione el boton de que esta esperando para que pueda proceder al proceso de elevar la aguja 

-> entonces cunado se procesa el pago  ya sea los estado < 

*/





#endif


