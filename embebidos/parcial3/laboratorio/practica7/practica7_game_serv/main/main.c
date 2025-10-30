#include "modulos/WIFI/wifi_lib.h"
#include<esp_wifi.h>
#include<esp_event.h>
#include<esp_spiffs.h>
#include<nvs_flash.h>
#include<esp_netif.h>
#include<esp_http_server.h>
#include<esp_log.h>
#include<esp_err.h>
#include<string.h>
#include<stdio.h>

//macros
//porque mi tablero tendra 7 x 8 cunado el juego original tiene 6 x 7, la fila y columna extra 
//es para colocar los infices de las corrdenadas, en las filas se colocora de A-F y en las 
//columnas de 1 - 7. para poder ingresar con exito las coordenadas
#define ROWS 7
#define COLS 8

//variables

// Estructura del estado del juego
typedef struct {
    char board[ROWS][COLS];  // 'O' para jugador 1, 'X' para jugador 2, ' ' para vacío
    char player1_name[32];
    char player2_name[32];
    int player1_chips;
    int player2_chips;
    int current_player;  // 1 o 2
    int game_over;
    char winner_message[128];
}GameState;

static const char *TAG = "AP_CONECT-4_GAME";
static GameState game_state;


//funciones

//tareas



void app_main(void)
{

}