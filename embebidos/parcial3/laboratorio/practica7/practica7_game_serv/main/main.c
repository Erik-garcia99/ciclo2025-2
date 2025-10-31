//modulos presonalizados 
#include "modulos/WIFI/wifi_lib.h"
#include"modulos/GPIO/gpio_lib.h"
//librerias de freertos
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/queue.h>
// #include<freertos/semphr.h> //muy posiblemente semaforos


//librerias para wifi
#include<esp_wifi.h>
#include<esp_event.h>
#include<esp_spiffs.h>
#include<nvs_flash.h>
#include<esp_netif.h>
#include<esp_http_server.h>

//librerias de majeo de errores
#include<esp_log.h>
#include<esp_err.h>

//perifericos
#include<driver/gpio.h>

//librerias de utilidades basicas 
#include<string.h>
#include<stdio.h>
#include<stdlib.h>





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

QueueHandle_t xQueueButton;
//funciones

//-------> funciones relacionadas al juego
void init_game(void); 


//tareas
//cuales seran las tareas sera el que esta cehcnado si se presiono un boton 
//creo que deberia ser la tarea que esta intereactucando con el juego 

void task_main(void *parms);



void app_main(void)
{

    //primero trata de inicar la escritrua en flash 
    esp_err_t ret = nvs_flash_init(); 
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    //inicializar SPIFFS
    init_spiffs();

    //iniciar wifi en modo access point 
    wifi_init_softap();


    //inicando perifericos 

    xQueueButton = xQueueCreate(10, sizeof(uint32_t));
    gpio_init();
    gpio_set_queue_handle(xQueueButton);
    type_int();

    


    //incializando tareas:
    xTasksCreate(task_main,"task_main", 4098, NULL, 10,NULL);
    ESP_LOGI(TAG, "Sistema Conecta 4 iniciado completamente");
    // ESP_LOGI(TAG, "Conectate al WiFi: SSID: mi_esp_ap, Password: 123456789");

}