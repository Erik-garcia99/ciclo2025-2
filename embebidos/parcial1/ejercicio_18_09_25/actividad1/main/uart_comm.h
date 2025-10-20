#ifndef UART_COMM_H
#define UART_COMM_H


#define UART_SEL UART_NUM_0 
#define BUFFER 1024
#define TX_PIN 2 //estos me funcionaron 
#define RX_PIN 4

//ocupamos 8 pines para sacar la informacion

#define BIT_0 13
#define BIT_1 14
#define BIT_2 16
#define BIT_3 17
#define BIT_4 18
#define BIT_5 19
#define BIT_6 21
#define BIT_7 22








//lo que esta pasndo es que el BUFFER circular es necesario para recibir los datos o enviar dado pro UART mas no mantiene guardado los datos, por lo que neceistmos una vairbale que guarde los datos para poder manjerarlo y poder operar con ellos, el buffer cirucar necesario para UART pierde los datos porque no sabemos si se envio nuevos datos entonces se estarina borrando, por lo que es necesario este buffer. 
static char intput_buffer[256];
static int input_index = 0; 

//este es el array que va a contener los datos que se van a orndea,r como no sabemos de que tamanio seria, por eso sera un apuntador
static uint8_t *array;
static uint8_t size_array=0; //el tamanio del arreglo 

//una condicion indicando que ya se 
static bool decl_size= false;



//definicion de funciones 
void UART_DEFINE();

void process_input(char *line);


//tarea principal;
void UART_ARRAY(void *parms);
//necesitamos la tarea que se encarga de recibir de estar minitorieando lo que esta recibiedno o enviado el UART 

void uart_task(void *params);

void init_gpio();



//funcion que ordena el arreglo
void quick_uart(); //pasamos por apintador
void detale_dup(int *vector); //con esta funcion eliminaremos los duplicados del vector, una vez que ya este ordenado


#endif
