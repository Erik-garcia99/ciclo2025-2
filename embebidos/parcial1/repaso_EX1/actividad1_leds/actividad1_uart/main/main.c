#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>
#include<driver/gpio.h>
#include<freertos/queue.h>

//macros 

#define BUF 1024
#define TX 1
#define RX 3
#define UART UART_NUM_0


static const char *TAG = "UART";

static QueueHandle_t uart_queue;


//funciones 

void init_uart();



//funciones tarea 



void app_main(void)
{

    init_uart();

}



void init_uart(){

    uart_config_t uart_confi ={
        .baud_rate=115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl=UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };


    ESP_ERROR_CHECK(uart_param_config(UART,&uart_confi));
    ESP_ERROR_CHECK(uart_set_pin(UART,TX,RX,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_driver_install(UART,BUF*2,BUF*2,20,&uart_queue,0));
}
