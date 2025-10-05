#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/gpio.h>
#include<driver/uart.h>


#define BUF_SIZE 1024
#define UART_RX_PIN 3
#define UART_TX_PIN 1


void echo_task(void *params);




void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", 3072,NULL,10,NULL);
}


void echo_task(void *params){
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };


    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0,UART_TX_PIN,UART_RX_PIN,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));

    //instalar los drivers 

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0,BUF_SIZE*2, BUF_SIZE * 2, 0, NULL, 0));

    //un buffer temporal, importante 

    uint8_t *data= (uint8_t *)malloc(BUF_SIZE);

    while(1){
        //read info from UART

        int len = uart_read_bytes(UART_NUM_0,data,(BUF_SIZE -1), 20/portTICK_PERIOD_MS);

        uart_write_bytes(UART_NUM_0, (const char *)data, len);
    }

}


