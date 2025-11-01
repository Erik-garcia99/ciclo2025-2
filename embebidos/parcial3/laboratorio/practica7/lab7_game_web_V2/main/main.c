//utilidades/livrerias estandares
#include<stdio.h>
// #include<stdlib.h>
// #include<string.h>

//freeRTOS
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include"freertos/queue.h"

//manjeador de errores
#include<esp_log.h>


//WIFI-FLASH
#include<esp_wifi.h>
#include<esp_spiffs.h>
#include<esp_flash.h>
#include<nvs_flash.h>
#include<esp_netif.h>
#include<esp_http_server.h>


//librerias propias
#include"modulos/GPIO/gpio_lib.h"
#include"modulos/WIFI/wifi_lib.h"


//macros
#define ROWS 7
#define COLS 8


//variables 
typedef struct {
    char board[ROWS][COLS];  // 'O' para jugador 1, 'X' para jugador 2, ' ' para vacío
    char player1_name[32];
    char player2_name[32];
    int player1_chips; //lsa fichas de que tiene cada jugadro
    int player2_chips;
    int current_player;  // 1 o 2 -> para controlar que jugador puede poner 
    int game_over; //termina el juego por ganar o por que se preiosno el boton 
    // int active_game;
    char winner_message[128]; //meensaje del ganaor en dado caso 
}GameState;

static const char *TAG = "AP_CONECT-4_GAME";
static GameState game_state;


static QueueHandle_t button_queue = NULL;


// //funciones

// void init_game(void);

//tareas

//necesito la tarea que esta verificando que no se preisone el boton, esta debe de estar siempre activa 

void proccess_intr(void *params);

void app_main(void){


    button_queue = xQueueCreate(10, sizeof(uint8_t));

    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    //servidor - WIFI
    init_spiffs();
    wifi_init_softap();
    start_web_server();

    //funciones de juego inicales 
    //inicamos el juego 


    //perifericos

    get_queue(&button_queue);

    gpio_init();
    type_int();

    //tarea de perifericos

    xTaskCreate(proccess_intr,"procces_int", 2048, NULL, 8, NULL);
}


void proccess_intr(void *params){

    uint32_t pin_number;

    while(1){

        if(xQueueReceive(button_queue,&pin_number,portMAX_DELAY)){
            //ENTONCES VERIFICAMOS QUE ES LO QUE RECIBIO desde la ISR
            
            //si no es un jugador entonces es el otro 
            if(pin_number == PLAYER_1){
                ESP_LOGI(TAG, "jugadro 1 finalizo el juego");
            }
            else{
                ESP_LOGI(TAG, "jugadro 2 finalizo el juego");
            }

        }


    }


}
