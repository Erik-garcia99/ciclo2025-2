#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
#include<freertos/event_groups.h>
#include<driver/gpio.h>
#include<driver/uart.h>

//definiciones 

#define BUFFER 1024
#define TX_PIN 3
#define RX_PIN 1

//definicion de funciones 
void UART_DEFINE();
//tarea principal;
void UART_ARRAY(void *parms);


//varibales globales 
//tag 
static const char *tag="uart_event";
//grupo de eventos para GPIO que indica cunado se complete el arreglo - se complete el ordenamineto - esta listo para la salida 
EventGroupHandle_t event_group;
const int ready = BIT0;
//cola de eventos


int INPUT_RX[];




void app_main(void)
{

    //definimos los eventos de grupo     

    event_group = xEventGroupCreate();
    //primero definimos la configuracion de nuestro UART, esto para establecerlo en un inicio o no estbalecerlos en cada proceso de la tarea 
    UART_DEFINE();


    //creacion de tareas 
    xTaskCreate(UART_ARRAY,"uart_array",2048,NULL,10,NULL);


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



    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0,&uart_config));


}


void UART_ARRAY(void *parms){


}