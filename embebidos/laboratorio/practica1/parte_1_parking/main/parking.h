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
#define DEBOUNCE_TIME 50 


//funciones 

//funcion que incia-configura los GPIO
void init_GPIO(void);


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





#endif


