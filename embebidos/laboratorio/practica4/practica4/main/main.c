#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include<driver/uart.h>
#include<driver/gpio.h>
#include "esp_log.h"    
#include "display.h"
#include<freertos/queue.h>




static const char *TAG="UART_USER";

//segunda tarea en donde leemos lo que recibe el UART y por cola mandamos el dato que este esta leyendo 

QueueHandle_t queue_uart0;

//matriz 

uint16_t *data;
int size_data;

//indicamos con uan bandera si ya se definicio de que tramaniop sera el arrelo 
int DEFINITION_DATA_FLAG = false;





void app_main(void)
{


    xTaskCreate(task_quick, "task_quick",4098, NULL,10,NULL);
    xTaskCreate(task_read_uart, "task_read_uart",BUF_SIZE*2, NULL,9,NULL);


}


void uart_init(){

    uart_config_t uart_config = {

        .baud_rate= 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity= UART_PARITY_DISABLE,
        .stop_bits= UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0,BUF_SIZE*2,BUF_SIZE*2, 20, &queue_uart0, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

}


void task_quick(void *params){


    uint8_t *buffer = (uint8_t*)malloc(BUF_SIZE);

    int i=0;
    while(1){

        if(xQueueReceive(queue_uart0,&buffer,portMAX_DELAY)){

            //ahorita mismo ya tengo mis datos dentro de la direccionde buffer.
            // ingrese un elementos el cual debemos de guardar 
            //vamos guardando hasta lo que diga size_data
            
            if(i < size_data ){
                



            }
            else{
                //mandamos a ordenar 
            }




        }
    }


}

task_read_uart(void *params){

    //esta tarea es el que va mandar a task_quick lo que se ingrese por el usuario 


    uart_event_t event;
    //un buffer temproar 
    uint8_t *buffer = (uint8_t*)malloc(BUF_SIZE);

    while(1){
        //este espera que se lea algo desde UART 

        if(xQueueReceive(queue_uart0,(void *)&event,(TickType_t)portMAX_DELAY)){

            memset(buffer, 0, BUF_SIZE);

            switch(event.type){

                case UART_DATA:{
                    uart_read_bytes(UART_NUM_0,buffer,event.size, portMAX_DELAY);

                    if(DEFINITION_DATA_FLAG == false){
                        //traemos lo que tiene
                        int i=0;
                        while(buffer >=0){

                            //debemos de desplzarnos, para saber cual es el numero que ingreso el usuarios 
                            size_data += buffer[i++] - 48;
                        }

                        data = (uint16_t*)malloc(size_data);
                        DEFINITION_DATA_FLAG = true;

                    }
                    else{
                        ESP_LOGI(TAG, "ingrese el elementos: ");
                        xQueueSend(queue_uart0,&buffer,portMAX_DELAY);
                    }            
                }break;


            }




        }

    }


}
