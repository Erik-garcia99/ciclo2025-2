#ifndef GAME_H
#define GAME_H
//aqui ira todas la decinidiones para GAME para compartrir entre el main, WIFI y GPIO, para poder mandar estrucutras o parametros entre estos 2. porque algunas cosas no son tareas 

#include"freertos/FreeRTOS.h"

//macros
#define ROWS 6
#define COLS 7

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
    int last_move_col;
    int last_move_row;
}GameState;

extern GameState game_state;
extern GameState game_state_wifi;

extern EventGroupHandle_t event_game;
extern const int player_name_rady_bit, new_move_bit, game_over_bit, py1_btn_bit,py2_btn_bit, update_display;


//funciones
/**
 * @brief funcion que pone en inicio el juego  
 * 
 * 
 * 
*/
void init_game(void);

/**
 * 
 * @brief funcion que establece los nombres en la estrucutrura del juego 
 * 
 * @param player1 -> nombre del primer jugador 
 * @param player2-> nombre del regundo jugador 
 * 
 * @return NULL 
 * 
 * 
*/

void set_player_names(const char *player1, const char *player2);

/**
 * @brief tomara la columna en donde el uusario queier ingresar su ficha, si la columna es correcta lo inserta, 
 * 
 * @param column -> indice de la columna 
 * 
 * @return -1 si la columan es incorrecta o si se termino el juego 
 * @return algo mas 
 */
int make_move(int column);

/**
 * 
 * @brief estara chechando si un jugador gano 
 * 
 * @return 0 si no hay ganadores o no se pued einsertar
 * @return >0 si se pudo insertar 
*/
int check_winner(void);

char* get_board_json(void);
char* get_game_info_json(void);


#endif