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
#define CINCO_PESO_IN 18
#define DIEZ_PESO_IN 18
#define VEINTE_PESO_IN 18


//GPIO de cambio
//SALIDAS
#define UN_PESO_OUT 13
#define CINCO_PESO_OUT 14
#define DIEZ_PESO_OUT 27
#define VEINTE_PESO_OUT 26 

//GPIO DE SALIDA, ERROR, INTRODUJO MAS DE LO PERMITIDO

#define ERR_EXCESS_MONEY 33


//funciones 

//funcion que incia-configura los GPIO

void init_GPIO(void);






#endif


