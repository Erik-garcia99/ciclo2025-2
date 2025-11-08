#ifndef WIFI_LIB_H
#define WIFI_LIB_H

#include"game.h"

/**
 * 
 * @brief esta funcion permite guardar en memoria flash los archivos que serviran como la presetnacion web del sistema guarda, el HTML/CSS/JS asi como imgaenes esto para que todo sea mucho mas rapido
 * 
*/

void init_spiffs(void);

/**
 *
 * @brief el proposito es convertir el ESP32 en un router WIFI, en esta funcion se configura el SSID <nombre de la red>, password, la cantiad mazima de estaciones <conexiones> que permite. configura los aspectos basicos de la red mas no inicaliza el rpoceso de la conversion a un ACCESS POINT
 *
 *
 *
*/

void wifi_init_softap(void);


//metodos que sirven o regresan desde el front hacia el back 


/**
 * 
 * @brief funcion que  
 * 
 * 
*/
void start_web_server(void);


//metodos de URI para gestionar peticiones URI 

/**
 * @brief srive la pagina inical 
 * 
 * @param req -> la peticion 
 * 
 * @return ESP_FAIL -> cunado no se puede acceder o abrir el archivo 
 * @return ESP_OK -> cunado se puede abri con exito 
*/
esp_err_t root_get_uri(httpd_req_t *req);


/**
 * @brief sirve la pagina del juego  
 * 
 * @param req -> la peticion
 * 
 * @return ESP_FAIL si no se pudo cargar la pagina 
 * @return ESP_OK si lo pudo hacer
*/
esp_err_t game_get_handler(httpd_req_t *req);

/**
 * 
 * @brief entrega el codigo CSS para darle un poco de color a la pagina 
 * 
 * 
*/
esp_err_t css_get_handler(httpd_req_t *req);

/**
 * 
 * @brief recibe los datos de los jugadores 
 * 
 * @return ESP_FAIL -> ocurrio un error al pasar datos de font -> back
 * @return ESP_OK -> se recibio datos con exito 
 * 
 * exp
 */
esp_err_t movent_handler(httpd_req_t *req);


/**
 * @brief esta es la pagina que recibe los nombres de los judadores  
 * 
 * 
*/
esp_err_t start_player(httpd_req_t *req);


//exp
esp_err_t game_state_handler(httpd_req_t *req);



#endif