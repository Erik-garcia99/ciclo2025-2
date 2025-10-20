#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>
#include<string.h>
#include<ctype.h>
#include "esp_log.h"





#define UART_USER UART_NUM_0 //comunicacion de PC -> ESP
#define UART_ESP_COMM   UART_NUM_2 //comunicacion ESP->ESP ->PC


// #define TX_PIN 16 
// #define RX_PIN 17 

#define TX_PIN 2   // GPIO2 - va al RX del emisor
#define RX_PIN 4


// #define TX_PIN 19  // Cambiado: TX del receptor va al RX del emisor
// #define RX_PIN 18 

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

static const char* TAG = "ESP_RECEPTOR";

//definicion de las fuente asquii
const asciiChar ASCII[]={
    {'A', {" ### ", "#   #", "#####", "#   #", "#   #"}},
    {'B', {"#### ", "#   #", "#### ", "#   #", "#### "}},
    {'C', {" ####", "#    ", "#    ", "#    ", " ####"}},
    {'D', {"#### ", "#   #", "#   #", "#   #", "#### "}},
    {'E', {"#####", "#    ", "###  ", "#    ", "#####"}},
    {'F', {"#####", "#    ", "###  ", "#    ", "#    "}},
    {'G', {" ####", "#    ", "# ###", "#   #", " ####"}},
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
    {'0', {" ### ", "#   #", "#   #", "#   #", " ### "}},
    {'1', {"  #  ", " ##  ", "  #  ", "  #  ", "#####"}},
    {'2', {" ### ", "#   #", "  ## ", " #   ", "#####"}},
    {'3', {" ### ", "#   #", "  ## ", "#   #", " ### "}},
    {'4', {"#   #", "#   #", "#####", "    #", "    #"}},
    {'5', {"#####", "#    ", "#### ", "    #", "#### "}},
    {'6', {" ### ", "#    ", "#### ", "#   #", " ### "}},
    {'7', {"#####", "    #", "   # ", "  #  ", " #   "}},
    {'8', {" ### ", "#   #", " ### ", "#   #", " ### "}},
    {'9', {" ### ", "#   #", " ####", "    #", " ### "}},
    {' ', {"     ", "     ", "     ", "     ", "     "}},
    {'!', {"  #  ", "  #  ", "  #  ", "     ", "  #  "}},
    {'.', {"     ", "     ", "     ", "     ", "  #  "}},
    {',', {"     ", "     ", "     ", "  #  ", " #   "}},
    {'+', {"     ", "  #  ", " ### ", "  #  ", "     "}},
    {'-', {"     ", "     ", " ### ", "     ", "     "}},
};



//calculamos el numero de elemntos que tensmos en nuestro arreglo. 
const int FONT_SIZE = sizeof(ASCII) / sizeof(asciiChar);



void app_main(void)
{
    ESP_LOGI(TAG, "iniciando ESP Receptor...");
    init_uarts();
    xTaskCreate(task_banner, "task_banner", 4096, NULL, 10, NULL);
    vTaskDelete(NULL);


}


void init_uarts(){
    ESP_LOGI(TAG, "configurando UARTs...");
    
    uart_config_t uart_config_user = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    ESP_ERROR_CHECK(uart_driver_install(UART_USER, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_USER, &uart_config_user));


    uart_config_t uart_config_comm = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    ESP_ERROR_CHECK(uart_driver_install(UART_ESP_COMM, BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_ESP_COMM, &uart_config_comm));
    ESP_ERROR_CHECK(uart_set_pin(UART_ESP_COMM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uart_flush(UART_ESP_COMM);
    uart_flush_input(UART_ESP_COMM);

    ESP_LOGI(TAG, "UARTS configurados. UART_ESP_COMM en TX:%d, RX:%d", TX_PIN, RX_PIN);

}   


const asciiChar* find_char(char c){

    for(int i=0; i< FONT_SIZE; i++){
        if(ASCII[i].character == c || ASCII[i].character == toupper(c)){
                return &ASCII[i];
        }
    }

    for(int i=0; i< FONT_SIZE; i++){
        if(ASCII[i].character == ' '){
                return &ASCII[i];
        }
    }
    return NULL;
}



void task_banner(void *params){
    uint8_t *data = (uint8_t*)malloc(BUF_SIZE);
    ESP_LOGI(TAG, "Tarea banner iniciada, esperando datos en UART2...");

    while(1){
        // Limpiar buffer antes de cada lectura
        memset(data, 0, BUF_SIZE);

        // Esperar con timeout pero sin mostrar mensaje cada vez
        int rx_bytes = uart_read_bytes(UART_ESP_COMM, data, MAX_INPUT, pdMS_TO_TICKS(1000));

        if(rx_bytes > 0){
            data[rx_bytes] = '\0';
            ESP_LOGI(TAG, "¡DATOS RECIBIDOS! Bytes: %d, Contenido: '%s'", rx_bytes, (char*)data);
            

           // ESP_LOGI(TAG, "Datos en HEX: %s", hex_str);

            bool valid_data = true;
            for(int i = 0; i < rx_bytes; i++){
                if(data[i] < 32 && data[i] != 0) { // Caracteres no imprimibles (excepto NULL)
                    valid_data = false;
                    break;
                }
            }

            if(valid_data) {
                ESP_LOGI(TAG, "Datos válidos, generando banner...");
                
                // Generar banner ASCII
                for(int line = 0; line < HEIGHT; line++){
                    for(int i = 0; i < rx_bytes; i++){
                        const asciiChar* c = find_char(data[i]);
                        if(c){
                            uart_write_bytes(UART_USER, c->lines[line], strlen(c->lines[line]));
                            uart_write_bytes(UART_USER, " ", 1);
                        }
                    }
                    uart_write_bytes(UART_USER, "\r\n", 2);
                }
                uart_write_bytes(UART_USER, "\r\n", 2);
            } else {
                ESP_LOGW(TAG, "Datos corruptos recibidos, ignorando...");
            }
        }
    }
    free(data);
}
