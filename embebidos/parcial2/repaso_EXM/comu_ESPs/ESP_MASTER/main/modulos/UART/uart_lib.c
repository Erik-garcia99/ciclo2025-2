#include "uart_lib.h"
#include<driver/uart.h>
#include<freertos/queue.h>
#include<stdio.h>

QueueHandle_t event_uart;






void uart_init(uart_port_t uart_num, uart_word_length_t data_frame,uart_parity_t parity_bits, uart_stop_bits_t uart_st_bits, uint8_t TX, uint8_t RX){

    uart_config_t config ={
        .baud_rate = 115200,
        .data_bits = data_frame,
        .parity = parity_bits,
        .stop_bits = uart_st_bits,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(uart_num, &config));

    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUFF * 2, BUFF *2, 20, event_uart,NULL));

    ESP_ERROR_CHECK(uart_set_pin(uart_num,TX, RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

}