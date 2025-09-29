#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#include<esp_log.h>
#include<driver/gpio.h>
#include<driver/uart.h>
#include "uart_comm.h"
#include<string.h>
#include<freertos/event_groups.h>

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
static const char *TAG="UART_ARRAY";


//grupo de eventos para GPIO que indica cunado se complete el arreglo - se complete el ordenamineto - esta listo para la salida < por ahorita solo se me ocurren colas, aun no grupo de eventos >

//cola de eventos
static QueueHandle_t uart_queue, queue_data_input;

EventGroupHandle_t event_group;

const int INIT_VEC = BIT0;
const int VEC_RD = BIT1; 

static const char lf[]= "\n";



//#######################################################################################
//TAREAS

void app_main(void)
{

    queue_data_input= xQueueCreate(10,sizeof(int8_t));
    event_group =  xEventGroupCreate();

    //definimos los eventos de grupo     

    // event_group = xEventGroupCreate();
    //primero definimos la configuracion de nuestro UART, esto para establecerlo en un inicio o no estbalecerlos en cada proceso de la tarea 
    UART_DEFINE();
    init_gpio();

    //creacion de tareas 
    xTaskCreate(uart_task,"uart_task",4098,NULL,9,NULL);
    xTaskCreate(UART_ARRAY,"uart_array",4048,NULL,10,NULL);


}



void UART_ARRAY(void *parms){

    while(1){

        //para que se pueda avanzar se debe de declarar, creo que debo de agregar un grupo de eventos para que no salga a cada rato esa mamada 

        //agregamos un grupo de eventos para indicar que 
        //creo que puedo poner el que espere los bits y mapear los bits que esta esperando a que se activen. 

        ESP_LOGI(TAG,"ingrese el tamanio del arreglo");
        //cunado llegue aqui se va a bloquear la tarea hatsa que se ingrese el valor 
        xEventGroupWaitBits(event_group,INIT_VEC,true,true,portMAX_DELAY);
        ESP_LOGI(TAG,"tamanio del arreglo ingresado %d",size_array);
        //aplicamos un saltyo de linea 
        // uart_write_bytes(UART_SEL, (const char*)lf, 1);

        //ingresando los datos por UART 
        //otro grupo de eventos que me indique cunado ya se termino de ingresar 
        xEventGroupWaitBits(event_group,VEC_RD,true,true,portMAX_DELAY);
        ESP_LOGI(TAG, "termino de ingresar los datos del arreglo -> procede a ordenar de manera asendente - > MENOR - MAYOR");
        ESP_LOGI(TAG,"impresion del arreglo ");
        
        //solo quier ver si si los paso a numeros o como los paso el pinche atoi 
        // char buffer_array_ptr;
        // for(uint8_t i=0; i<size_array; i++){


        //     char item_array = array[i];
        //     item_array += 48; // -> '0'
        //     ESP_LOGI(TAG,"posicion [%d]",i);
        //     uart_write_bytes(UART_SEL,(const char*)&item_array,sizeof(item_array));
        //     uart_write_bytes(UART_SEL, (const char*)&lf,sizeof(lf));
            
        // }

        vTaskDelay(100/portTICK_PERIOD_MS);
        ESP_LOGI(TAG,"ARREGLO ORDENADO SIN REPETICION / impresion BIN LEDS");
        quick_uart();
        for(uint8_t i=0; i<size_array; i++){

            uint8_t aux = array[i];
            if(aux != 255){
                char item_array = aux;
                item_array += 48; // -> '0'
                uart_write_bytes(UART_SEL,(const char*)&item_array,sizeof(item_array));
                uart_write_bytes(UART_SEL, (const char*)&lf,sizeof(lf));

                
                // si tengo 25 -> binario seria 0001 1001

                bool state_LD0 = (aux & 1);
                bool state_LD1 = (aux & 2);
                bool state_LD2 = (aux & 4);
                bool state_LD3 = (aux & 8);
                bool state_LD4 = (aux & 16);
                bool state_LD5 = (aux & 32);
                bool state_LD6 = (aux & 64);
                bool state_LD7 = (aux & 128);

                ESP_LOGI(TAG, "PRENDIENDO LEDS");
                vTaskDelay(1000/portTICK_PERIOD_MS);

                gpio_set_level(BIT_0, state_LD0);
                gpio_set_level(BIT_1, state_LD1);
                gpio_set_level(BIT_2, state_LD2);
                gpio_set_level(BIT_3, state_LD3);
                gpio_set_level(BIT_4, state_LD4);
                gpio_set_level(BIT_5, state_LD5);
                gpio_set_level(BIT_6, state_LD6);
                gpio_set_level(BIT_7, state_LD7);

                vTaskDelay(1000/portTICK_PERIOD_MS);
                ESP_LOGI(TAG, "APAGANDO LEDS");
                gpio_set_level(BIT_0, 0);
                gpio_set_level(BIT_1, 0);
                gpio_set_level(BIT_2, 0);
                gpio_set_level(BIT_3, 0);
                gpio_set_level(BIT_4, 0);
                gpio_set_level(BIT_5, 0);
                gpio_set_level(BIT_6, 0);
                gpio_set_level(BIT_7, 0);
                vTaskDelay(1000/portTICK_PERIOD_MS);

            }

        }



    }
}




//creo que esto funciona mucho mas para cunado se esta leyedo, cunado estasmo escirbiendo son en esapcios muy especificos que nosotros podemos controlar desde el codigo, pero cunado se esta leyendo no es tanto asi, porque no sabemos a que hora se va a mandar datos por UART
void uart_task(void *params){

    //esta se encargara se recibir los datos de uart. 

    uart_event_t event;
    uint8_t *buffer = (uint8_t*)malloc(BUFFER); //si pongo un sizeof no me estara reservando los 1024 bytes que indique que queria

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
                            ESP_LOGI(TAG, "entrada completa %s", intput_buffer);
                            //funacion para tratar loq ue entreo, esto va a corresponder a este ejercicio, 
                            process_input(intput_buffer);
                            input_index=0;
                        }

                    }

                    //cunado es un numero o algo que en verdad acepta 
                    if(c >= '0' && c <='9'){
                        //porqeu aqui, porque si lo ponemso al inicio se estara como que imprimirendo 2 veces en el caso de querer borrar, entonces lo que estoy haceindo aqui es que si se ingreso un dato a UART lo imprimimos al momento de ingresarlo
                        intput_buffer[input_index++] = c;
                        uart_write_bytes(UART_SEL, (const char*)&c, 1); //echo de lo que se escribe
                    }

                    //backspace
                    if(c =='\b' || c == 127){
                        if(c > 0){
                            //intput_buffer[input_index--]= '\0'; //no hay nada
                            input_index--;
                            uart_write_bytes(UART_SEL, "\b \b", 3);
                            // uart_write_bytes(UART_SEL, "/b", 1);
                            // uart_write_bytes(UART_SEL," ",1);
                            // uart_write_bytes(UART_SEL,"\b",1);
                            // uart_write_bytes(UART_SEL," ",1);
                            // uart_write_bytes(UART_SEL," ",1);
                        }
                    }
                    
                }
            }break;

            //en estos 2 casos en donde tanto la lista FIFO o el buffer estan llenos es necesario resetearlos porque algo paso que no los liberero por lo que hay que hacerlo manulamente. 
           case UART_FIFO_OVF:

                ESP_LOGE(TAG, "Hardware FIFO overflow");
                uart_flush_input(UART_SEL);
                xQueueReset(uart_queue);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGE(TAG, "Ring buffer full");
                uart_flush_input(UART_SEL);
                xQueueReset(uart_queue);
                break;

            case UART_BREAK:
                ESP_LOGI(TAG, "UART break signal detected");
                break;

            case UART_PARITY_ERR:
                ESP_LOGE(TAG, "UART parity error");
                break;

            case UART_FRAME_ERR:
                ESP_LOGE(TAG, "UART frame error");
                break;

            // Un default para cualquier otro evento inesperado
            default:
                ESP_LOGI(TAG, "UART event type: %d", event.type);
                break;

            }
        }

    }

}




//##########################################################################
//funciones
//con esta funcion mandar por cola los datos, los numeros 1 a 1, 

//intdouce peirmo el tamanio del arrelog 

//despues introduce 1 a 1 los datos
//no es necesario decir si ya 
void process_input(char *line){

    //esto porque recordar que en el UART le espamos ponenedo que el frame es de 8 bits 
    uint8_t number = (uint8_t)atoi(line);

    static uint8_t index_array =0; //este indica en que posicion del indice esta en este momento el arreglo 

    //el numero que ingreso debe de ser mayor a 0, un numero positivo 
    //de al menos 1 su espaico. 
    if(number > 0){

        //el numero que ingreso debe de ser 

        if(decl_size != true){
            //quiere decri que aun no se ha declado un tamanio al arreglo. 
            array = (uint8_t*)malloc(sizeof(uint8_t) * number);
            //grupo de evemtos 
            size_array = number;
            decl_size = true;
            xEventGroupSetBits(event_group,INIT_VEC); //indicamos que ya se configuro el tamanio

            ESP_LOGI(TAG, "Tamaño del arreglo: %d", number);
            ESP_LOGI(TAG, "ingrese el valor de la posicion [%d]: ", index_array);
            index_array++;
        }

        else{

            if(index_array < size_array){
                array[index_array] = number;
                ESP_LOGI(TAG, "ingrese el valor de la posicion [%d]: ", index_array);
                index_array++;
                
            }
            else if(index_array == size_array){
                ESP_LOGI(TAG,"arreglo completado");
                xEventGroupSetBits(event_group,VEC_RD);
            }
        }
    }


}




void quick_uart(){

    //no podreimos hacer recursividad ya que para un uC es un poco complicado hacer eso, por lo que 
    //podremos hacer el motod del burbuja en el cual estamos intercambiando entre uno y otro 
    //necesito tomar el primero elemeo y compararlo con el sigueinte 
    
    uint8_t swuapped;

    for(int i = 0 ; i< size_array; i++){
        swuapped =0; //aun no esta ordenado
        //este se va a ciclcar simepre y cunado haya intercambios, hasta que ya no haya
        for(int j= 0 ; j< size_array - i - 1; j++){
            //este ciclo es el que se ecnarga de realizar los intercambios

            if(array[j] > array[j+1]){
                uint8_t temp = array[j];
                array[j] = array[j+1];
                array[j+1]= temp;
                swuapped =1;
            }
        }
        if(!swuapped) break;
    }

    //ahora necesitamos intercambiar los datos que esten repetidos, remplazarlos con 1 
    //para solo comparar hasta el penunitmo y el j compraa hasta el ultimo 
     for(int i = 0; i < size_array - 1; i++){
        for(int j = i + 1; j < size_array; j++){
            // Si son iguales y no son -1
            if(array[i] == array[j] && array[i] != 255){
                array[j] = 255;  
            }
        }
    }
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
    
    uart_flush(UART_SEL); //limpinado buffer
}


void init_gpio(){


    gpio_reset_pin(BIT_0);
    gpio_reset_pin(BIT_1);
    gpio_reset_pin(BIT_2);
    gpio_reset_pin(BIT_3);
    gpio_reset_pin(BIT_4);
    gpio_reset_pin(BIT_5);
    gpio_reset_pin(BIT_6);
    gpio_reset_pin(BIT_7);

    gpio_set_direction(BIT_0, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIT_1, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIT_2, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIT_3, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIT_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIT_5, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIT_6, GPIO_MODE_OUTPUT);
    gpio_set_direction(BIT_7, GPIO_MODE_OUTPUT);


    gpio_set_level(BIT_0, 0);
    gpio_set_level(BIT_1, 0);
    gpio_set_level(BIT_2, 0);
    gpio_set_level(BIT_3, 0);
    gpio_set_level(BIT_4, 0);
    gpio_set_level(BIT_5, 0);
    gpio_set_level(BIT_6, 0);
    gpio_set_level(BIT_7, 0);

}
