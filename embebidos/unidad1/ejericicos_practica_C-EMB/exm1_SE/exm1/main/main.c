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

    //para limpiarlo desde la otra funcion y practicar puntereos 
    static uint8_t *input_buffer[BUFF];
    static uint8_t *index_input = 0;


    while(1){

        if(xQueueReceive(uart_event, (void *)&event,portMAX_DELAY)){

            //limpimamso el buffer 
            // memset(buffer, 0, BUFF);

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
        ESP_LOGI(TAG, "cadenas ingresadas");
        
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

    if(!str1 && !str2){
        //si los 2 string aun no han inicalizado entonces lo inicializamos 
        //entonces quiere decri que debemos ingresar el primer string 
        aux = cad1;
        current_str = &str1;
    }
    else if(str1 == true && !str2){
        //en este caso str1 = true y como !(str2 = false) ->  str2= true entonces entrara por lo que 
        aux = cad2;
        current_str = &str2;
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

        else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')){
            //la si quiero converit a -> A solo le resto 32 o de A -> a le sumo 32

            aux[idx++] = c;
            uart_write_bytes(UART_SEL, (const char*)&c, 1);
        }

        else if(c == '\n' || c =='\r'){
            //ponemos el final con un caracter nulo 
            aux[idx] = '\0';
            //reiniciamos idx 
            idx = 0;
            //depende que cadena fue 
            *current_str = true;
            memset((uint8_t *)input,0,BUFF);
        }
    }

    //activamos el grupo de eventos
    if(str1==true && str2 == true){

        xEventGroupSetBits(uart_status, str_ready);
    }




    

}



