#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>
#include<freertos/queue.h>
#include<freertos/event_groups.h>
#include<string.h>

#include "uart_user.h"




void app_main(void)
{
    init_uarts();
    xTaskCreate(task_uart_input,"task_user_input", 2048, NULL, 10, NULL);
    vTaskDelete(NULL);
}





void init_uarts(){
    
    uart_config_t uart_config_user = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    ESP_ERROR_CHECK(uart_driver_install(UART_USER, BUF_SIZE * 2,0,0,NULL,0));
    ESP_ERROR_CHECK(uart_param_config(UART_USER, &uart_config_user));


    uart_config_t uart_config_comm = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(UART_ESP_COMM, BUF_SIZE * 2,0,0,NULL,0));
    ESP_ERROR_CHECK(uart_param_config(UART_ESP_COMM, &uart_config_comm));
    ESP_ERROR_CHECK(uart_set_pin(UART_ESP_COMM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));


}



void task_uart_input(void *params){

    uint8_t *data = (uint8_t*)malloc(BUF_SIZE);
    char input_buffer[MAX_INPUT+1];
    int len=0;

    const char* welcome = "introduce el texto y preciona enter:\r\n";
    uart_write_bytes(UART_USER,welcome,strlen(welcome));


    while(1){


        int rx_bytes= uart_read_bytes(UART_USER, data, 1, portMAX_DELAY);

        //leyo algo
        if(rx_bytes > 0){
            char received_char= data[0];

            //recibi el dato, pero necesito saber que recibi en dado caso que se ingrese '\n' indicando el pin de la cade a

            if(received_char == '\r'){
                input_buffer[len] = '\0';
                //salto de liena a la terminal 

                uart_write_bytes(UART_USER, "\r\n", 2);

                //envamos la cadena la otro ESP

                uart_write_bytes(UART_ESP_COMM, input_buffer, len);
                printf("[USER] Enviando a UART2: %s\n", input_buffer);

                //reseteamos la posiciones 
                len=0;
                uart_write_bytes(UART_USER,"> ",2);
            }
            else if(received_char == '\b' || received_char == 127){

                if(len > 0){
                    len--;
                    uart_write_bytes(UART_USER,"\b \b", 3);
                }

            }
            else if(len < MAX_INPUT){
                if((received_char >= 'A' && received_char <= 'Z') || (received_char >= 'a' && received_char <= 'z') ||
                (received_char>= '0' && received_char <= '9') ||
                (received_char == '!') || (received_char == '.') || (received_char == ',') || (received_char == '+') || (received_char == '-') || (received_char == ' ')){

                    input_buffer[len++] = received_char;
                    uart_write_bytes(UART_USER, (const char*)&received_char, 1);

                }
            }
        } 

    }
    free(data);

}