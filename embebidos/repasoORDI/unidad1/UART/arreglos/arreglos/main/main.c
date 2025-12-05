#include <stdio.h>
#include<string.h>
#include<stdlib.h>

//librerias freertos 
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#include<freertos/event_groups.h>
//drivers 
#include<driver/uart.h>


/**
 * 
 * el proeblma del primer examen fue que se ingresan 2 cadenas por uarrt y ver cunatas veces 
 * necsitaba desplzarse hacia la izquierda para poder ser la misma, 
 * 
 * y ese total imprmirlo por leds que serian 5 leds 
 * 
 * 
 */


//libreria propias
#include"modulos/UART/uart_lib.h"
#include"modulos/GPIO/gpio_lib.h"

QueueHandle_t uart_queue, data_queue;

static const char lf[]="\n";


//funciones 

int desplazamineto(char *cad1, char *cad2);

//tareas 

void main_task(void *params);

void app_main(void)
{
    // uart_queue = xQueueCreate(10, sizeof(uint8_t));
    data_queue = xQueueCreate(10, sizeof(char *));

    init_uart(UART_SEL, UART_DATA_8_BITS, UART_STOP_BITS_1, UART_PARITY_DISABLE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);

    init_gpio();

    xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL);
    xTaskCreate(main_task,"main_task", 2048, NULL, 9, NULL);
}

void main_task(void *params){
    
    char dato_1[]="ingrese una cadena:\n";
    char dato_2[]="ingrese una cadena 2:\n";

    char *cadena1=NULL;
    char *cadena2=NULL;
    
    
    while(1){

        //se va a quedar esperando a que se ingrese el primer datos 
        uart_write_bytes(UART_SEL, dato_1, strlen(dato_1));

        if(xQueueReceive(data_queue, (void *)&cadena1, portMAX_DELAY)){
            char exito[]="cadena 1 recibida\n";
            uart_write_bytes(UART_SEL, exito, strlen(exito));
            uart_write_bytes(UART_SEL, cadena1, strlen(cadena1));
            uart_write_bytes(UART_SEL,lf,strlen(lf));
            
        }


        uart_write_bytes(UART_SEL, dato_2, strlen(dato_2));
        if(xQueueReceive(data_queue, (void *)&cadena2, portMAX_DELAY)){

            char exito[]="cadena 2 recibida\n";
            uart_write_bytes(UART_SEL, exito, strlen(exito));
            uart_write_bytes(UART_SEL, cadena2, strlen(cadena2));
            uart_write_bytes(UART_SEL,lf,strlen(lf));
        }

        if(cadena1 !=NULL && cadena2 !=NULL){

            char buffer[64];
            
            //una vez aqui ya se tienen las 2 cadenas., las madare a la funcion que servirara para poder calcular ese proceso y regresara el total o -1 si es que falla 
            int total_desplazamineto = desplazamineto(cadena1,cadena2); 


            if(total_desplazamineto < 0){
                //ocurrio un error las cadenas no son iguales o no son las masa 
                char error[]="las cadenas no son iguales o no son del mismo tamanio\n";
                uart_write_bytes(UART_SEL, error, strlen(error));
            }

            else if(total_desplazamineto ==0){
                char cadenas_iguales_init[]="las cadenas ya son las mismas\n";
                uart_write_bytes(UART_SEL, cadenas_iguales_init, strlen(cadenas_iguales_init));
            }

            else if(total_desplazamineto >0){

                //ahora necesito mostrar por leds el total que se ncesitaron 

                // x x x x x 
                
                bool state_LD1 = (total_desplazamineto & 1);
                bool state_LD2 = (total_desplazamineto & 2);
                bool state_LD3 = (total_desplazamineto & 4);
                bool state_LD4 = (total_desplazamineto & 8);
                bool state_LD5 = (total_desplazamineto & 16);   

                gpio_set_level(GPIO1, state_LD1);
                gpio_set_level(GPIO2, state_LD2);
                gpio_set_level(GPIO3, state_LD3);
                gpio_set_level(GPIO4, state_LD4);
                gpio_set_level(GPIO5, state_LD5);

                sprintf(buffer, "total desplazamineto: %d", total_desplazamineto);
                // char total[]="el total de desplazamieot fue de:";
                // uart_write_bytes(UART_SEL, total,strlen(total)); 
                uart_write_bytes(UART_SEL, buffer,strlen(buffer)); 
                uart_write_bytes(UART_SEL,lf,strlen(lf));

            }

        }
    }


}



int desplazamineto(char *cad1, char *cad2){

    //antes que nada para que las cadenas puedan hacer el proceso del desplzamiento, deben de estar del mismo tamanio ya que sin este no importa cuanto se roten nunca quedaran igual 
    int tam_dato1= strlen(cad1);
    int tam_dato2=strlen(cad2);
    if(tam_dato1 != tam_dato2){
        return -1; //hubo un error 
    }
    else if(strcmp(cad1, cad2) == 0){
        return 0; //inidcando que ya son iguales por lo que no es necesario hacer este proceso 
    }
    else{

        //muy bien entonces tiene el mimso tamanio,
        //llevara el conteo de las reotaciones necesarios, pero este no debe de exceder el tamanio que nos dio strlen para que no este aqui ciclado, porque si eso pasa quiere decir que en efecto hay 2 cadenas del mismo tamanio pero no son la misma desplazada, es otra cadena solo que con la misma cantidad de caracteres 
        // int count=0; 
        

        for(int i=0; i < tam_dato1 -1; i++){

            //como desplazo la cadena, seria sacar lo que esta a la izquierda, desplzar todo una posicion y ingresarlo por la derecha 
            char temp= cad2[0]; //tomo lo que hay en le extremo mas a la izquierda para al final agregarlo al extremo mas a la derecha 

            for(int j=0; j<tam_dato2 -1; j++){
                cad2[j]= cad2[j+1]; //el que esta una posicion mas  ala derecha lo traigno a una posicion mas a la izquiwrda 
            }

            cad2[tam_dato2 -1] = temp;
            
            if(strcmp(cad1,cad2) ==0){
                return i+1;
            }
        }
    }


    return -1; //las cadenas pueden que sean del mimso tamanio pero no son la misma 
}