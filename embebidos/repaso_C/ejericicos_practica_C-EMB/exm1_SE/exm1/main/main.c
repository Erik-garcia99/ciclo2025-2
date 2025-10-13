#include <stdio.h>
#include<stdlib.h>
#include<string.h>
// #include<ctype.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>
#include "uart_lib.h"
#include<esp_log.h>
#include<freertos/queue.h>


// -> estamos en la ultima funcion sobre el rol del vector 



/*
el programa que nos pidio la proce es de 

-> en xamen 1

-> recibir por UART 2 cadenas

-> relizar un algoritmo que diga cunatas roataciones hacia la izquierda haya que hacer para que ambas cadenas sean iguales

-> imprimir por UART cunatas fueron necesarias y las cadenas

-> por leds imprimir la cantidad de esas mismas, con 5 LEDS

-> hcinadolo lo mas eficiente posible. en recursos (CPU, memoria y perifericos )


*/


//####################PRUEBAS - APUNTES
//vamos a ahcer algo, vamos a hacer un buffer de caracteres. y uno de uint8_t, para ver como se comportan y ver cual me conviene mejor 


QueueHandle_t uart_event;
EventGroupHandle_t uart_status;
static const char *TAG = "uart_cmp";
// static const char lf[] = "\n";



bool str1 = false, str2 = false;

const int str_ready = BIT0;

void app_main(void)
{
    init_uart();
    uart_status = xEventGroupCreate();


    xTaskCreate(uart_task, "uart_task", 4098,NULL, 8, NULL);
    xTaskCreate(task_main, "main_task", 4098, NULL, 10, NULL);


}

//#########################################TAREAS

void uart_task(void *params){

    uart_event_t event;

    uint8_t *buffer = malloc(BUFF*2);
    // ESP_LOGI(TAG, "ingresando UART\n");
    //para limpiarlo desde la otra funcion y practicar puntereos 

    while(1){

        if(xQueueReceive(uart_event, (void *)&event,portMAX_DELAY)){

            switch(event.type){

                case UART_DATA :{
                    //he recibido algo 
                    
                    //la funcion que lee por uart lo que devuelve es el tamani0o de lo que leyo
                    int len = uart_read_bytes(UART_SEL, buffer, event.size, portMAX_DELAY);
                    
                    process_input(buffer, len);

                    
                }break;
                default:{
                    const char *mess = "other error";
                    uart_write_bytes(UART_SEL, mess, strlen(mess));

                }break;;

            }
        }

    }

}


void task_main(void *params){


    while (1)
    {
        //un grupo de eventos esperando 
        //esperamos hasta que se ingresen las 2 cadenas 
        xEventGroupWaitBits(uart_status, str_ready, pdTRUE, pdTRUE, portMAX_DELAY);
        
        // ESP_LOGI(TAG, "cadenas ingresadas");
        const char *mess = "\ncadenas ingresadas\n";
        uart_write_bytes(UART_SEL, mess, strlen(mess));
        uart_write_bytes(UART_SEL,(const char *)&cad1, strlen(cad1));
        uart_write_bytes(UART_SEL,"\n", 1);
        uart_write_bytes(UART_SEL,(const char *)&cad2, strlen(cad2));

        //ahora ya tengo las cadenas ingresadas entonces ahora toca el tratar las cadenas y saber cuantas roataciones hacia la izquierda son necesarias para que sean iguales 


        //antes de esto, primero ambas cadenas deben de ser del mismo tamanio, en otro caso dara un error 

        bool pass = str_cmp();

        if(pass == false){

            //algo pasa cunado quiero volver a ingresar algo, que hace cosas raras y todo lo que ingreso lo manda de nuevo aqui, no se espera. ni a que introduzca todo 
            //aqui termina el programa 
            //limpiamos las cadenas s
            str1 = false;
            str2 = false;
            memset((char *)cad1,0,BUFF);
            memset((char *)cad2,0,BUFF);
            xEventGroupClearBits(uart_status, str_ready);
            const char *mess = "\nlas cadenas no son del mismo tamanio\n";
            uart_write_bytes(UART_SEL, mess, strlen(mess));
            //limsoamos los bits que hacen que se active esta part 

        }
        else if (pass == true){
            //entonces si coindicen 
            ESP_LOGI(TAG, "cadenas coindicen!");
            //si las cadenas coinciden entonces podemos hacer el trabajo de rotar las cadenas, 
            //debemos de escoger una de las 2 cadenas para que conicidean, la cual es la cadena numero 2

            //$$$$$$ CRRO QUE PODEMOS ASIGNAR MEMORIA DEPENDO DE LO QUE ENTRA PERO POR AHORA SIGAMOS CON TODO MANUAL
            //ahora debemos de rotar las cadenas, 

            //nueva funcion

            /****
            -> tenemos las 2 cadenas 
            -> puden haber 2 casos 
                --> 1 la cadenas son iguales solo que una desfasada
                --> 2 don del mismo tamanio solo que no son la misma cadena
                
            -> tengo el tamanio de cada uno en una varibale global 
            -> como saber si aunque sean del mismo tamanio no son cadenas iguales, si la cantidad de rotaciones son iguales o mayores a lo que se supone que es el tamanio de la cadena quiere decir que no son iguales y no importa cunatas veces se rotee porque no son una cadena igual, porque vuelve a su estado original 

            -> como tenemos un arreglo de 1024, no podemos sacar el ultimo espacio, debemos de tener indices que me indiquen cual es el rango maximo, sera de 0 - size_cad(1-2)-1 -> esto porque el ultimo es el caracter nulo indicando el final de la cadena. 
            */

            //mandamos a llamar a al funcion que me indique las rotaciones, devolvera el total de rotaciones para mostrar por pantalla 


            //esto ests al final 
            str1 = false;
            str2 = false;
            memset(cad1, 0, BUFF);
            memset(cad2, 0, BUFF);
            xEventGroupClearBits(uart_status, str_ready);



            
        }
        
        
    }
    
}

//########################################FUNCIONES 


void init_uart(){

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity= UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_SEL,&uart_config));

    ESP_ERROR_CHECK(uart_driver_install(UART_SEL,BUFF *2, BUFF *2,20,&uart_event,0));

    ESP_ERROR_CHECK(uart_set_pin(UART_SEL,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));


    //creo que es lo que debo de acrivar para el uart 

    const char *mess = "uart configurado\n";

    uart_write_bytes(UART_SEL, mess,strlen(mess));

}


void process_input(uint8_t *input, uint8_t size){


    //aqui ingresamos a la primera cadena lo que se ingresa. 
    //como saber a que cadena debo de ingresar 

    //bool str1 = false, str2 = false;
    static char *aux= NULL; //a este le pasaremos la direccion en donde estan los arrelgos 
    static uint8_t idx = 0; //el indice en donde va dentro de la cadena
    static bool *current_str = NULL;

    //vairble de ayuda, solo para imprimir 
    static bool print = false;
    //este apuntador va a apuntar a la variable en donde tendran el tamanio de cada cadena real, < DEPSUES VERE COMO PUEDO HACER DE MANERA DINAMICA > 
    static *size =NULL;

    if(!str1 && !str2){
        //si los 2 string aun no han inicalizado entonces lo inicializamos 
        //entonces quiere decri que debemos ingresar el primer string 
        aux = cad1;
        current_str = &str1;
        size = size_cad1;
        if(print == false){
            print =true;

            const char *mess = "\ningresar la cadena 1:\n";
            uart_write_bytes(UART_SEL, mess, strlen(mess));
        }


    }
    else if(str1 == true && !str2){
        //en este caso str1 = true y como !(str2 = false) ->  str2= true entonces entrara por lo que 
        aux = cad2;
        current_str = &str2;
        size= size_cad2;

        if(print == false){
            print =true;

            const char *mess = "\ningresar la cadena 2:\n";
            uart_write_bytes(UART_SEL, mess, strlen(mess));
        }
    }
    for(int i=0 ; i< size; i++){
        uint8_t c=input[i];

        if(c == '\b'){
            if(idx > 0){
                //debio de haber ingreado algo
                idx--;
                uart_write_bytes(UART_SEL,"\b \b",3);
            }
        }

        else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||( c == ' ')){
            //la si quiero converit a -> A solo le resto 32 o de A -> a le sumo 32

            if(idx < BUFF -1){
                aux[idx++] = c;
                uart_write_bytes(UART_SEL, (const char*)&c, 1);
            }
        }

        else if(c == '\n' || c =='\r'){
            if(idx > 0){
                //ponemos el final con un caracter nulo 

                //para poder usar la memoria solo que necetio, aqui tengo como usarlo, tengo aux

                aux[idx] = '\0';
                size = idx; //la cantidad de idx es el ancho de la cadena que se introdujo 
                //reiniciamos idx 
                idx = 0;
                //depende que cadena fue 
                *current_str = true;
                memset(input,0,BUFF);
                print= false; //devolvemos el valor original para poder imprimir el segundo mesnaje
            } 
        }
    }

    //activamos el grupo de eventos
    if(str1==true && str2 == true){
        xEventGroupSetBits(uart_status, str_ready);
    }
}


bool str_cmp(){


    //se va a contar hasta el caracter nulo, si son difrentes entonces son string de tamanio diferentes y por lo tanto nunca quedaran de la misma manera. 
    int len_cad1 = 0, len_cad2 = 0;

    //trabajaremos con puntadores
    char *aux_cad1 = cad1; 
    char *aux_cad2 = cad2;
    

    //primero el primer ciclo 
    int i=0;
    while(aux_cad1[i] != '\0'){
        i++;
        len_cad1++;
    }

    i=0;

    while(aux_cad2[i] != '\0'){
        i++;
        len_cad2++;
    }

    if(len_cad1 != len_cad2){
        return false; //no son iguales por lo que no se pruede procesa r
    }
    else{
        return true;
    }
    return false; //si llega aqui ocurrio un error

}


int rol_vec(){


    //devemos mover los arreglos 

    //lo haremos con el arreglo 2 

    /*
    
    tengo el sigueinte arreglo 
             0    1    2    3    
    cad2 = ['H']['O']['L']['A']
    
    -> las letras salen por 'H'
    -> necesitamos una variables <char> que darmaos el caracter que saldra 
    -> 
    */

    char backup_char; // neceitmaos que nos regresa en que parte estamos porque el arreglo abarca mucho espacio, mas de lo que usara. 
    int acc = 0; //este es el que llevara el contador de cunatas rotaciones se han dado. 

    //ROTACION HACIA LA IZQUIERDA 

    //como podemos recorrer el arrelgo 
    //si estamos recorriendo hacia la izquierda, a cada elemento le restamos una posicion, 
    /*
    POSIBLE ALGORIMTOS 

    -> sacamos el elemnto del indice 0
    -> lo guardamos en una varibale temporal porque es el caracter/elemento que es el que va salir de la lista 
    -> a cada elemento le restamos 1 en su posicion para que el que esta en POS[1] -> POS[0], POS[2] -> POS[1], etc.
    -> al final guardamos el elemnto de la vairbale en la posicon < size_cad(1-2)-1 >
    ########################3
    -> IMPORTANTE RECORDAR NO MOVER EL ELEMNTO ULTIMO DE size_cad(1-2) PORQUE ESTE ES EL TIENE EL CARACTER NULO INDICANDO EL FINAL DE LA CADENA      
    #########################

    -> en cada rotacion comparamos si son iguales, si lo son entonces salidmos del ciclo, si no continuamos 

    -> la cosa esta en que creo que debo de tener 2 indices 
    1-> es el que contarai ahcia adelante 
    2 -> el otro es al que le voy a restar para ingresar el caracter en su nueva posicion. 
    */
    int cmp;
    // int aux_idx = size_cad2-1;


    for(int i=0; i <= size_cad2 -1; i++){

        backup_char = cad2[0];

        for(int j=0; j <= size_cad2 -1 ;i++){
            //porque este segundo bloque porque debo de reocerr 1 todos los elemntos hacia la izquierda y necesito este ciclo de mas 

        }


        //este compara hasta que enutre el caracter nulo 
        // cmp = strcmp();
    }


}


