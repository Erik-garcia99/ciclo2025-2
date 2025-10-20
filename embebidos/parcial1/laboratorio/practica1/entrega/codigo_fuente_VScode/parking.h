
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
//#define VEINTE_PESO_OUT 26 //este ni sirve, nucna dara mas de 20 en cambio y aparte no esta

//GPIO DE SALIDA, ERROR, INTRODUJO MAS DE LO PERMITIDO
#define DEBOUNCE_TIME 200000 
#define TICKET_LED 25

//GPIO aguja - estado vehiculo 
//salida aguha 

#define _DOWN_ 33
#define _MIDDLE_ 32
#define _UP_ 23

//entradas 
#define init_vehi 4
#define pass_vehi 26


//estados 

typedef enum {
    STATE_INITIAL=0,
    STATE_COLLECTING,
    STATE_PAID,
    STATE_OVERPAID,
    STATE_GIVING_CHANGE,
    STATE_RECEIPT
} vending_state_t;



typedef enum{
    BARRIER_WAITING = 0, //esperando vehiculo
    BARRIER_RAISING,     //subiendo abuja
    BARRIER_UP,          //abuja completamente arriba
    BARRIER_LOWERING,    //bajando abuja
    BARRIER_INTERRUPTED  //aguja interrumpida
}barrier_state_t;

typedef enum{

    STATE_DOWN=0,
    STATE_MIDDLE,
    STATE_UP
}state_needle_t;


//#########################################
//comuncion entre tareas, por medio de las colas 
QueueHandle_t handlerQueue, moneyQueue,vehicleQueue;

extern vending_state_t current_state;
extern barrier_state_t current_barrier_state;
extern bool pyment_completed; //bandera que indica que el pago se completo
extern bool vehicle_waiting; //bandera que me indica que le vehiculo esta esperadno 


extern state_needle_t curren_state_needle;



//funciones 

//funcion que incia-configura los GPIO
void init_GPIO(void);

void vending_machine(int money_acc);
void give_change(void);
//funcion del efecto de que levante 




//manejador de las interupciones
void gpio_isr_handler(void *args);

void TYPE_INT(void);

//tareas
void PROCESS_INTR(void *params);

void MONEY_MANAGER(void *params);




//funcion que procesa la tarea 

//funcion que trata lo que se manda del manejador de interrupciones. 
void STEP_VEHI(void *params);

//implementacion de la maquina de estados
void barrier_state_machine(int vehi_pos);

//funcion que actualiza los GPIO dependeindo del estado en que se encuentren 
    
void update_needle_leds(state_needle_t needle_state, bool is_raising);

//tarea que se encarga de levantar la aguja 

void raise_needle_task(void *params);
void lower_needle_task(void *params);

void interrupted_sequence(void *params);


#endif


