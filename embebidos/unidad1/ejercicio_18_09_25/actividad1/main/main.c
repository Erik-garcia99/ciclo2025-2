#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#include<esp_log.h>
#include<driver/gpio.h>
#include<driver/uart.h>
#include "uart_comm.h"
#include<string.h>


/*
esta actividad era de que recibe por UART 
-primero un numero que es el que representa el numero del arreglo
-despues va ingresando los datos hasta n-1
-despues de eso pasamos a ordenar
    -primero se ordena de manera asendente, de menor a mayor
    -despues si hay numeros repetidos sustituirlos con el -1
-imrpimroos los valores con sus valores binarios que seran desplegados por GPIO en 8 leds 
-> no imprimir los -1. 
*/








//varibales globales 
//tag 
static const char *TAG="uart_event";


//grupo de eventos para GPIO que indica cunado se complete el arreglo - se complete el ordenamineto - esta listo para la salida < por ahorita solo se me ocurren colas, aun no grupo de eventos >

//cola de eventos
static QueueHandle_t uart_queue, queue_data_input;


static const char lf[]= "\n";



//#######################################################################################
//TAREAS

void app_main(void)
{

    queue_data_input= xQueueCreate(10,sizeof(int8_t));


    //definimos los eventos de grupo     

    // event_group = xEventGroupCreate();
    //primero definimos la configuracion de nuestro UART, esto para establecerlo en un inicio o no estbalecerlos en cada proceso de la tarea 
    UART_DEFINE();


    //creacion de tareas 
    xTaskCreate(uart_task,"uart_task",4098,NULL,9,NULL);
    xTaskCreate(UART_ARRAY,"uart_array",4048,NULL,10,NULL);


}



//creo que esto funciona mucho mas para cunado se esta leyedo, cunado estasmo escirbiendo son en esapcios muy especificos que nosotros podemos controlar desde el codigo, pero cunado se esta leyendo no es tanto asi, porque no sabemos a que hora se va a mandar datos por UART
void uart_task(void *params){

    //esta se encargara se recibir los datos de uart. 

    uart_event_t event;
    uint8_t *buffer = (uint8_t*)malloc(sizeof(BUFFER));

    while(1){
        if(xQueueReceive(uart_queue,(void *)&event, (TickType_t)portMAX_DELAY)){
        memset(buffer,0,BUFFER); //esta estableceindo 0 todo el buffer cricular de nustro UART

        switch(event.type){

            case UART_DATA:{
                //hay datos en UART
                //lo que voy a recibir de UART son datos en codigo ASCII, pero lo va a recibir y estran alamcenados de manera numerica, deciamal decimal, por lo que debemos de tratar los numeros para enceontrar el valor real que es el que introdujo el usuario, por ejemplo si introdiuce el 0 numerico dentro de lo que recibe el UART es 48 porque en la tabal ascii '0' = 48
                uart_read_bytes(UART_SEL,buffer,event.size,portMAX_DELAY);

                //lo que esta haceindo ahorita es que UART esta recibiendo caracter por caracter. por lo que debemos de ver hasta cunatos el usuario desea insertar, recordando que lo que recibe es caracter, codigo ASCII 

                for(int i =0; i < event.size; i++){

                    //recorre el buffer 

                    char c = (char)buffer[i];

                    //vemos que es lo que el usuario escribe, incluso vamos a ingresar el backspace

                    //si ya termino, esto cuando se termina es cunado de da enter o retodno de carro (vuelve al inicio)
                    if(c == '\n' || c == '\r'){

                        if(input_index > 0){
                            //indicando que ya se escribio algo, en otro caso no hace nada ni se encia nada porque no se ha escrito nada 

                            intput_buffer[input_index] = '\0'; //marcando el final de lo qie ontrodujo

                            ESP_LOGI(TAG, "entrada completa");

                            //una vez que se introdujo los x cantidad 




                        }

                    }




                }



                ESP_LOGI(TAG,"[UART_DATA]. len %d data: ", event.size);
                uart_write_bytes(UART_SEL,buffer,event.size);
                uart_write_bytes(UART_SEL,lf,sizeof(lf));
            }break;

            default:{
                ESP_LOGI(TAG,"orale");
            };break;

        }


    }

    }

}




void UART_ARRAY(void *parms){

    while(1){





    }
}


//##########################################################################
//funciones
//con esta funcion mandar por cola los datos, los numeros 1 a 1, 

//intdouce peirmo el tamanio del arrelog 

//despues introduce 1 a 1 los datos
//no es necesario decir si ya 
void process_input(char *line){

    int number = atoi(line);

    //el numero que ingreso debe de ser mayor a 0, un numero positivo 
    //de al menos 1 su espaico. 
    if(number > 0){

    }


}




void quick_uart(int *vector){

    //esta funcion ya le pasaremos el vector con todos los datos crudos 
    






}




void UART_DEFINE(){

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };


    ESP_ERROR_CHECK(uart_driver_install(UART_SEL,BUFFER*2, BUFFER*2,20,&uart_queue,0));
    ESP_ERROR_CHECK(uart_param_config(UART_SEL,&uart_config));

    //esot para comunicarme solo conectando el ESP 
    ESP_ERROR_CHECK(uart_set_pin(UART_SEL,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));
    
}



