#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>
#include<freertos/queue.h>
#include<freertos/event_groups.h>
#include<string.h>
#include<ctype.h>

// #include "uart_com_esp.h"



#define UART_USER UART_NUM_0 //comunicacion de PC -> ESP
#define UART_ESP_COMM   UART_NUM_2 //comunicacion ESP->ESP ->PC


// #define TX_PIN 17
// #define RX_PIN 16
#define TX_PIN 18
#define RX_PIN 19

#define MAX_INPUT 25
#define BUF_SIZE 1024

#define HEIGHT 5 
#define WIDTH 6 // se le agrega +1 para el final de la linea 


typedef struct{
    char character;
    const char lines[HEIGHT][WIDTH];
}asciiChar;


void init_uarts();

const asciiChar* find_char(char c);

void task_banner(void *params);



//definicion de las fuente asquii

const asciiChar ASCII[]={

    {'A',{" ### ", "#    #", "#####", "#   #", "#   #"}},
    {'B', {"#### ", "#   #", "#### ", "#   #", "#### "}},
    {'C', {" ####", "#    ", "#    ", "#    ", " ####"}},
    {'D', {"#### ", "#   #", "#   #", "#   #", "#### "}},
    {'E', {"#####", "#    ", "###  ", "#    ", "#####"}},
    {'F', {"#####", "#    ", "###  ", "#    ", "#    "}},
    {'H', {"#   #", "#   #", "#####", "#   #", "#   #"}},
    {'I', {"#####", "  #  ", "  #  ", "  #  ", "#####"}},
    {'J', {"#####", "   # ", "   # ", "#  # ", " ### "}},
    {'K', {"#   #", "#  # ", "###  ", "#  # ", "#   #"}},
    {'L', {"#    ", "#    ", "#    ", "#    ", "#####"}},
    {'M', {"#   #", "## ##", "# # #", "#   #", "#   #"}},
    {'N', {"#   #", "##  #", "# # #", "#  ##", "#   #"}},
    {'O', {" ### ", "#   #", "#   #", "#   #", " ### "}},
    {'P', {"#### ", "#   #", "#### ", "#    ", "#    "}},
    {'Q', {" ### ", "#   #", "# # #", "#  ##", " ####"}},
    {'R', {"#### ", "#   #", "#### ", "#  # ", "#   #"}},
    {'S', {" ####", "#    ", " ### ", "    #", "#### "}},
    {'T', {"#####", "  #  ", "  #  ", "  #  ", "  #  "}},
    {'U', {"#   #", "#   #", "#   #", "#   #", " ### "}},
    {'V', {"#   #", "#   #", "#   #", " # # ", "  #  "}},
    {'W', {"#   #", "#   #", "# # #", "# # #", " # # "}},
    {'X', {"#   #", " # # ", "  #  ", " # # ", "#   #"}},
    {'Y', {"#   #", " # # ", "  #  ", "  #  ", "  #  "}},
    {'Z', {"#####", "   # ", "  #  ", " #   ", "#####"}},
    {' ', {"     ", "     ", "     ", "     ", "     "}},
    {'!', {"  #  ", "  #  ", "  #  ", "     ", "  #  "}},
    {'.', {"     ", "     ", "     ", "     ", "  #  "}},
    {' ', {"     ", "     ", "     ", "     ", "     "}},



};


//calculamos el numero de elemntos que tensmos en nuestro arreglo. 
const int FONT_SIZE = sizeof(ASCII) / sizeof(asciiChar);



void app_main(void)
{
    init_uarts();
    xTaskCreate(task_banner, "task_banner", 4096, NULL, 10, NULL);
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
    
    ESP_ERROR_CHECK(uart_driver_install(UART_USER,BUF_SIZE * 2,0,0,NULL,0));
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


const asciiChar* find_char(char c){

    for(int i=0; i< FONT_SIZE; i++){
        if(ASCII[i].character == c || ASCII[i].character == toupper(c)){
                return &ASCII[i];
        }
    }
    return NULL;
}



void task_banner(void *params){

    uint8_t *data = (uint8_t*)malloc(BUF_SIZE);

    while(1){

        int rx_bytes = uart_read_bytes(UART_ESP_COMM, data, BUF_SIZE, portMAX_DELAY);

        if(rx_bytes >0){
            data[rx_bytes]= '\0';
            printf("[COMM] Recibido por UART2: %s\n", data);
            for(int line=0; line< HEIGHT; line++){
                for(int i=0; i< rx_bytes; i++){
                    const asciiChar* c = find_char(data[i]);
                    if(c){
                        uart_write_bytes(UART_USER, c->lines[line], strlen(c->lines[line]));
                        uart_write_bytes(UART_USER," ",1);
                    }
                }
                uart_write_bytes(UART_USER, "\r\n", 2);
            }
            uart_write_bytes(UART_USER,"\r\n", 2);
        }

    }
    free(data);

}