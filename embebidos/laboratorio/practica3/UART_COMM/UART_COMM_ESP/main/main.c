#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>
#include<string.h>
#include<ctype.h>
#include "esp_log.h"

// #include "uart_com_esp.h"



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



    //     ESP_LOGI(TAG, "=== TEST DE HARDWARE ===");
    // ESP_LOGI(TAG, "Configurando UART2 SIN conexiones físicas");
    
    // uart_config_t uart_config = {
    //     .baud_rate = 115200,
    //     .data_bits = UART_DATA_8_BITS,
    //     .parity = UART_PARITY_DISABLE,
    //     .stop_bits = UART_STOP_BITS_1,
    //     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    //     .source_clk = UART_SCLK_DEFAULT,
    // };
    
    // ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0));
    // ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    // ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    
    // uart_flush(UART_NUM_2);
    // uart_flush_input(UART_NUM_2);
    
    // ESP_LOGI(TAG, "UART2 configurado. TX:%d, RX:%d", TX_PIN, RX_PIN);
    // ESP_LOGI(TAG, "==================================");
    // ESP_LOGI(TAG, "IMPORTANTE: NO debe haber cables conectados a GPIO16/17");
    // ESP_LOGI(TAG, "Si ves datos, el pin RX está captando ruido");
    
    // while(1) {
    //     size_t bytes_available;
    //     uart_get_buffered_data_len(UART_NUM_2, &bytes_available);
        
    //     if(bytes_available > 0) {
    //         ESP_LOGW(TAG, "¡PROBLEMA! Se detectaron %d bytes SIN conexiones", bytes_available);
    //         ESP_LOGW(TAG, "Esto indica ruido eléctrico o pin defectuoso");
            
    //         // Leer y limpiar
    //         uint8_t data[100];
    //         uart_read_bytes(UART_NUM_2, data, bytes_available > 100 ? 100 : bytes_available, 0);
    //         uart_flush(UART_NUM_2);
    //     } else {
    //         ESP_LOGI(TAG, "OK - No hay datos (comportamiento normal)");
    //     }
        
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }
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
    // Si no encuentra el carácter, retorna espacio
    for(int i=0; i< FONT_SIZE; i++){
        if(ASCII[i].character == ' '){
                return &ASCII[i];
        }
    }
    return NULL;
}




// void task_banner(void *params){
//     uint8_t *data = (uint8_t*)malloc(BUF_SIZE);
//     ESP_LOGI(TAG, "tarea BANNER inicada...");

//     // Limpiar buffer inicial una vez más
//     uart_flush(UART_ESP_COMM);
//     uart_flush_input(UART_ESP_COMM);

//     while(1){
//         // Verificar si hay datos disponibles antes de leer
//         size_t bytes_available;
//         uart_get_buffered_data_len(UART_ESP_COMM, &bytes_available);
        
//         if(bytes_available > 0) {
//             ESP_LOGI(TAG, "detectados %d bytes disponibles", bytes_available);
            
//             // Limpiar buffer de recepción
//             memset(data, 0, BUF_SIZE);
            
//             // Leer byte por byte para control preciso
//             char message[MAX_INPUT + 1];
//             int msg_len = 0;
            
//             for(int i = 0; i < bytes_available && i < MAX_INPUT; i++) {
//                 int rx_bytes = uart_read_bytes(UART_ESP_COMM, data, 1, pdMS_TO_TICKS(100));
                
//                 if(rx_bytes == 1) {
//                     char received_char = data[0];
                    
//                     // Verificar que sea un carácter ASCII válido
//                     if(received_char >= 32 && received_char <= 126) {
//                         message[msg_len++] = received_char;
//                     } else if(received_char == 0) {
//                         // Fin de mensaje
//                         break;
//                     } else {
//                         ESP_LOGW(TAG, "caracter inválido recibido: 0x%02X", received_char);
//                     }
//                 } else {
//                     break;
//                 }
//             }
            
//             if(msg_len > 0) {
//                 message[msg_len] = '\0';
//                 ESP_LOGI(TAG, "¡MENSAJE VALIDO! Longitud: %d, Contenido: '%s'", msg_len, message);
                
//                 ESP_LOGI(TAG, "generando banner ASCII...");
//                 for(int line = 0; line < HEIGHT; line++){
//                     for(int i = 0; i < msg_len; i++){
//                         const asciiChar* c = find_char(message[i]);
//                         if(c){
//                             uart_write_bytes(UART_USER, c->lines[line], strlen(c->lines[line]));
//                             uart_write_bytes(UART_USER, " ", 1);
//                         }
//                     }
//                     uart_write_bytes(UART_USER, "\r\n", 2);
//                 }
//                 uart_write_bytes(UART_USER, "\r\n", 2);
//             } else {
//                 ESP_LOGW(TAG, "No se recibieron caracteres validos, limpiando buffer...");
//             }
            
//             // Limpiar cualquier dato residual
//             uart_flush(UART_ESP_COMM);
//             uart_flush_input(UART_ESP_COMM);
//         }
        
//         vTaskDelay(pdMS_TO_TICKS(100));
//     }
//     free(data);
// }

void task_banner(void *params){
    uint8_t *data = (uint8_t*)malloc(BUF_SIZE);
    ESP_LOGI(TAG, "Tarea banner iniciada, esperando datos en UART2...");

    while(1){
        // Limpiamos el buffer antes de leer
        memset(data, 0, BUF_SIZE);
        
        ESP_LOGI(TAG, "Esperando datos por UART2...");
        int rx_bytes = uart_read_bytes(UART_ESP_COMM, data, BUF_SIZE-1, portMAX_DELAY);

        if(rx_bytes > 0){
            data[rx_bytes] = '\0';
            ESP_LOGI(TAG, "¡DATOS RECIBIDOS! Bytes: %d, Contenido: '%s'", rx_bytes, (char*)data);
            
            // Mostrar datos en hexadecimal para debug
            char hex_str[256] = {0};
            for(int i = 0; i < rx_bytes && i < 32; i++) {
                sprintf(hex_str + strlen(hex_str), "%02X ", data[i]);
            }
            ESP_LOGI(TAG, "Datos en HEX: %s", hex_str);

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
            ESP_LOGW(TAG, "No se recibieron datos, rx_bytes = %d", rx_bytes);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Pequeña pausa para evitar saturar el CPU
    }
    free(data);
}