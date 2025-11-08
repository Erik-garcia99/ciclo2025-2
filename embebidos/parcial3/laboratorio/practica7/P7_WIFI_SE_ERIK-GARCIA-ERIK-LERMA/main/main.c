//utilidades/livrerias estandares
#include<stdio.h>
// #include<stdlib.h>
// #include<string.h>

//freeRTOS
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include"freertos/queue.h"
#include"freertos/event_groups.h"

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
#include"game.h"





static const char *TAG = "AP_CONECT-4_GAME";

GameState game_state;

QueueHandle_t button_queue = NULL;

EventGroupHandle_t event_game;

const int player_name_rady_bit = BIT0, new_move_bit=BIT1, game_over_bit=BIT2, py1_btn_bit=BIT3,py2_btn_bit=BIT4, update_display=BIT5;

//tareas

void game_task(void *params);
void button_task(void *params);
void display_task(void *params);



void app_main(void){


    button_queue = xQueueCreate(10, sizeof(uint8_t));

    event_game= xEventGroupCreate();

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

    xTaskCreate(button_task,"procces_int", 2048, NULL, 8, NULL);
    xTaskCreate(game_task, "main_task", 4098, NULL, 10, NULL);
    xTaskCreate(display_task, "display_task", 4096, NULL, 7, NULL);

}

void button_task(void *params) {
    uint32_t pin_number;

    while (1) {
        if (xQueueReceive(button_queue, &pin_number, portMAX_DELAY)) {
            if (pin_number == PLAYER_1) {
                ESP_LOGI(TAG, "Jugador 1 finalizo el juego");
                xEventGroupSetBits(event_game, py1_btn_bit);
            } else {
                ESP_LOGI(TAG, "Jugador 2 finalizo el juego");
                xEventGroupSetBits(event_game, py2_btn_bit);
            }
        }
    }
}


void game_task(void *params) {
    // Esperar a que se ingresen los nombres de los jugadores
    xEventGroupWaitBits(event_game, player_name_rady_bit, pdTRUE, pdTRUE, portMAX_DELAY);
    
    // Inicializar juego
    init_game();
    xEventGroupSetBits(event_game, update_display);

    while (1) {
        // Esperar por eventos del juego
        EventBits_t bits = xEventGroupWaitBits(
            event_game, 
            new_move_bit | py1_btn_bit | py2_btn_bit,
            pdTRUE,  // Clear on exit
            pdFALSE, // Wait for any bit
            portMAX_DELAY
        );

        if (bits & new_move_bit) {
            // El movimiento ya fue procesado en el handler HTTP
            // Solo necesitamos verificar el estado del juego
            int winner = check_winner();
            
            if (winner) {
                // Hay un ganador
                snprintf(game_state.winner_message, sizeof(game_state.winner_message),
                        "FELICIDADES %s!", 
                        (winner == 1) ? game_state.player1_name : game_state.player2_name);
                game_state.game_over = 1;
                xEventGroupSetBits(event_game, game_over_bit | update_display);
            } else if (game_state.player1_chips == 0 && game_state.player2_chips == 0) {
                // Empate
                strcpy(game_state.winner_message, "Suerte para la próxima!");
                game_state.game_over = 1;
                xEventGroupSetBits(event_game, game_over_bit | update_display);
            } else {
                // Cambiar turno
                game_state.current_player = (game_state.current_player == 1) ? 2 : 1;
                xEventGroupSetBits(event_game, update_display);
            }
        }

        if (bits & py1_btn_bit) {
            // Jugador 1 presiono el boton
            snprintf(game_state.winner_message, sizeof(game_state.winner_message),
                    "El jugador %s salió del juego.", game_state.player1_name);
            game_state.game_over = 1;
            xEventGroupSetBits(event_game, game_over_bit | update_display);
        }

        if (bits & py2_btn_bit) {
            // Jugador 2 presiono el boton
            snprintf(game_state.winner_message, sizeof(game_state.winner_message),
                    "El jugador %s salió del juego.", game_state.player2_name);
            game_state.game_over = 1;
            xEventGroupSetBits(event_game, game_over_bit | update_display);
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void display_task(void *params) {
    while (1) {
        // Esperar a que se necesite actualizar la pantalla
        xEventGroupWaitBits(event_game, update_display, pdTRUE, pdTRUE, portMAX_DELAY);
        
        ESP_LOGI(TAG, "Estado del juego actualizado");
        ESP_LOGI(TAG, "Jugador 1: %s - Fichas: %d", game_state.player1_name, game_state.player1_chips);
        ESP_LOGI(TAG, "Jugador 2: %s - Fichas: %d", game_state.player2_name, game_state.player2_chips);
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}



void init_game(void) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            game_state.board[i][j] = ' ';
        }
    }
    
    game_state.player1_chips = 21;
    game_state.player2_chips = 21;
    game_state.current_player = 1;
    game_state.game_over = 0;
    game_state.last_move_col = -1;
    game_state.last_move_row = -1;
    strcpy(game_state.winner_message, "");
}


//############# preguntar de aqui para abajo 

//porque guarda el la ultima columan que movio x jugador? y que es lo que regresa make_move, porque regresa la fila? e -1 lo enteindo pero la fila? 

int make_move(int column) {
    if (column < 0 || column >= COLS || game_state.game_over) {
        return -1;
    }

    for (int row = ROWS - 1; row >= 0; row--) {
        if (game_state.board[row][column] == ' ') {
            game_state.board[row][column] = (game_state.current_player == 1) ? 'O' : 'X';
            game_state.last_move_col = column;
            game_state.last_move_row = row;
            
            if (game_state.current_player == 1) {
                game_state.player1_chips--;
            } else {
                game_state.player2_chips--;
            }
            
            return row;
        }
    }
    return -1;
}

//preguntar como funciona esta funcion? wtf

int check_winner(void) {
    if (game_state.last_move_row == -1 || game_state.last_move_col == -1) {
        return 0;
    }

    char current_piece = game_state.board[game_state.last_move_row][game_state.last_move_col];
    int directions[4][2] = {{1,0}, {0,1}, {1,1}, {1,-1}};

    for (int d = 0; d < 4; d++) {
        int count = 1;
        
        //direccion positiva
        for (int i = 1; i < 4; i++) {
            int new_row = game_state.last_move_row + i * directions[d][0];
            int new_col = game_state.last_move_col + i * directions[d][1];
            
            if (new_row >= 0 && new_row < ROWS && new_col >= 0 && new_col < COLS && 
                game_state.board[new_row][new_col] == current_piece) {
                count++;
            } else {
                break;
            }
        }
        
        //direccion negativa 
        for (int i = 1; i < 4; i++) {
            int new_row = game_state.last_move_row - i * directions[d][0];
            int new_col = game_state.last_move_col - i * directions[d][1];
            
            if (new_row >= 0 && new_row < ROWS && new_col >= 0 && new_col < COLS && 
                game_state.board[new_row][new_col] == current_piece) {
                count++;
            } else {
                break;
            }
        }
        
        if (count >= 4) {
            return game_state.current_player;
        }
    }
    
    return 0;
}


char* get_board_json(void) {
    static char json[1024];
    char temp[100];
    
    strcpy(json, "[");
    for (int row = 0; row < ROWS; row++) {
        strcat(json, "[");
        for (int col = 0; col < COLS; col++) {
            char piece = game_state.board[row][col];
            sprintf(temp, "\"%c\"", piece);
            strcat(json, temp);
            if (col < COLS - 1) strcat(json, ",");
        }
        strcat(json, "]");
        if (row < ROWS - 1) strcat(json, ",");
    }
    strcat(json, "]");
    
    return json;
}


char* get_game_info_json(void) {
    static char json[512];
    
    sprintf(json, 
        "{\"player1_name\":\"%s\",\"player2_name\":\"%s\","
        "\"player1_chips\":%d,\"player2_chips\":%d,"
        "\"current_player\":%d,\"game_over\":%d,"
        "\"winner_message\":\"%s\"}",
        game_state.player1_name, game_state.player2_name,
        game_state.player1_chips, game_state.player2_chips,
        game_state.current_player, game_state.game_over,
        game_state.winner_message);
    
    return json;
}




