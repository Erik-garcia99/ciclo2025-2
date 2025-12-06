/**
 * 
 * ejercicio recibe por uart una cadena, el cual es el tamanio del arreglo 
 * 
 * recive los datos del arreglo
 * 
 * acomoda el arreglo
 * 
 * lo imprime por uart 
 */

#include <stdio.h>
#include<string.h>

//freertos
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#include<freertos/event_groups.h>


//perifericos
#include<driver/uart.h>

//manejo de errores
#include<esp_err.h>
#include<esp_log.h>

//macros
#define UART_SEL UART_NUM_0
#define BUFF 1024

//varibales 

QueueHandle_t uart_event, data_receive;
EventGroupHandle_t event_status;
static const char *TAG="UART_CADENA";

const int def_array=BIT0;

//funciones 

esp_err_t uart_init();


uint8_t *ingreso_arreglo(int tam_arreglo);
uint8_t *sort_array(uint8_t *array, int tam_arreglo);


//tareas
//tarea que recibe datos por usart 

void task_receive_uart(void *params);

void main_task(void *params);


void app_main(void)
{

    data_receive = xQueueCreate(20, sizeof(uint8_t*));
    event_status= xEventGroupCreate();


    esp_err_t ret;

    ret= uart_init();

    if(ret !=ESP_OK){

        ESP_LOGE(TAG, "error al inicar uart %s", esp_err_to_name(ret));
    }


    xTaskCreate(task_receive_uart, "task_uart", 2048, NULL, 10, NULL);
    xTaskCreate(main_task, "main_task", 4098, NULL, 9,NULL);
}


esp_err_t uart_init(){

    uart_config_t config={

        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity=UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t ret;

    ret= uart_param_config(UART_SEL, &config);

    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "error al configurar parametro: %s", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ret = uart_set_pin(UART_SEL, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "error: %s", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ret = uart_driver_install(UART_SEL, BUFF*2, BUFF*2, 20, &uart_event, 0);

    if(ret!=ESP_OK){
        ESP_LOGE(TAG, "error: %s", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    //si todo salio bien entonces mandamos ESP_OK
    return ESP_OK;
}


void task_receive_uart(void *params){

    uart_event_t event;
    uint8_t *buffer =(uint8_t*)malloc(BUFF);
    uint8_t *data =(uint8_t*)malloc(BUFF);
    int index_data=0;

    while(1){

        if(xQueueReceive(uart_event, (void *)&event,portMAX_DELAY)){

            switch(event.type){

                case UART_DATA:{

                    ///ya estamos reciviendo los datos 

                    uart_read_bytes(UART_SEL, buffer, event.size, portMAX_DELAY);

                    //ya renemos los datos recibidos 

                    for(int i=0; i< event.size; i++){
                        uint8_t c = buffer[i];

                        //solo estamos aceptando numeros 
                        if(c >= '0' && c<= '9'){
                            data[index_data++]= c;
                            uart_write_bytes(UART_SEL, (const char *)&c, sizeof(c));
                        }   
                        else if(c == '\b'){
                            index_data--;
                            uart_write_bytes(UART_SEL, "\b \b", 3);
                        }
                        else if(c=='\n' || c== '\r'){
                            data[index_data]='\0';//terminamos 

                            uint8_t *new_msg =(uint8_t*)malloc(strlen((char*)data)+1); 
                            strcpy((char*)new_msg, (char*)data);
                            xQueueSend(data_receive, &new_msg, 0);
                            
                            memset(data, 0, BUFF);
                            index_data= 0;
                            memset(buffer, 0, BUFF);
                        }
                    }


                }break;


                case UART_BUFFER_FULL :{

                    xQueueReset(uart_event);
                    uart_flush(UART_SEL);
                }break;

                case UART_FIFO_OVF:{
                    xQueueReset(uart_event);
                    uart_flush(UART_SEL);
                }break;

                default :{
                    ESP_LOGI(TAG, "type event: %d", event.type);
                }break;

            }

        }

    }

}



void main_task(void *params){


    uint8_t *array=NULL;

    
    //datos recibidos
    uint8_t *tam_array=NULL;
    int tamanio_final_arreglo = 0;

    while(1){

        if(xQueueReceive(data_receive, &tam_array, portMAX_DELAY)){
            tamanio_final_arreglo = atoi((char*)tam_array);
            ESP_LOGI(TAG, "dato: %d", tamanio_final_arreglo);

            
        }

        //ya recibio los datos del arreglo 
        array=ingreso_arreglo(tamanio_final_arreglo);

        array= sort_array(array, tamanio_final_arreglo);

        char buffer[128];

        for(int i=0; i<tamanio_final_arreglo;i++ ){

            char mess[]="dato : ";
            uart_write_bytes(UART_SEL, mess, strlen(mess));
            sprintf(buffer, "dato:%d\n\r", array[i]);

            uart_write_bytes(UART_SEL, buffer, strlen(buffer));

        }


    }

}


uint8_t *ingreso_arreglo(int tam_arreglo){

    uint8_t *array_local =(uint8_t*)malloc(tam_arreglo);

    uint8_t *current_data=NULL;
    
    char mess[] = "ingrese el dato: ";
    for(int i=0 ; i<= tam_arreglo; i++){
        uart_write_bytes(UART_SEL,mess,strlen(mess));
        if(xQueueReceive(data_receive, &current_data, portMAX_DELAY)){

            int convert_dato = atoi((char*)current_data);

            array_local[i]=(uint8_t)convert_dato;
        }
    }

    return array_local;
}


uint8_t *sort_array(uint8_t *array, int tam_arreglo){


    uint8_t *aux= array; 
    //este va  llegar hasta el penuntumo elementos del arreglo.
    for(int i=0; i < tam_arreglo-1; i++){
        //este irea un elementos mas
        for(int j=i+1;j < tam_arreglo ; j++){
            uint8_t elm_i = aux[i];
            uint8_t elm_j = aux[j];

            if(elm_i > elm_j){
                aux[j]= elm_i;
                aux[i]=elm_j;
            }
            else if(elm_i == elm_j){
                continue;
            }
        }
    }

    return aux;

}