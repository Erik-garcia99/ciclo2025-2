//librerias propias
#include"modulos/GPIO/gpio_lib.h"
#include"modulos/WIFI/wifi_lib.h"

//WIFI-FLASH
#include<esp_wifi.h>
#include<esp_spiffs.h>
#include<esp_flash.h>
#include<nvs_flash.h>
#include<esp_netif.h>
#include<esp_http_server.h>

//freeRTOS
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>



//utilidades/livrerias estandares
#include<stdio.h>
// #include<stdlib.h>
// #include<string.h>


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

// static const char *TAG = "conecta4";

// //funciones

// void init_game(void);

//tareas

//necesito la tarea que esta verificando que no se preisone el boton, esta debe de estar siempre activa 

void proccess_intr(void *params);

void app_main(void){

    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    //servidor - WIFI
    init_spiffs();
    wifi_init_softap();


    //funciones de juego inicales 
    //inicamos el juego 


    //perifericos

    gpio_init();
    type_int();

    //tarea de perifericos

    xTaskCreate(proccess_intr,"procces_int", 2048, NULL, 8, NULL);
}


void proccess_intr(void *params){

    while(1){

        


    }


}
