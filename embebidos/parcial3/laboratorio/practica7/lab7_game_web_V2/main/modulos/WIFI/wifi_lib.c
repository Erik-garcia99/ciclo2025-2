
//libereias estandares
#include<string.h>

//manejo de erroes
#include<esp_log.h>
#include<esp_err.h>

//librerias wifi-servidor
#include <esp_wifi.h>
#include<esp_spiffs.h>
#include<nvs_flash.h>
#include<esp_netif.h>
#include<esp_https_server.h>

//libreias personalziadas 
#include"wifi_lib.h"
#include"game.h"


//ahora necesito pasar desde main -> wifi y de wifi->main la estrucutra con los nombres de los ugadores asi como las posiciones que estos vana  estar manjeando 
//por lo que debemos crear funciones get y set para estblecer y devolver parametros entre las funciones del main que es donde estara la logica del juego "backend" y wifi que 
//es donde esta el "fontend"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif


static const char *TAG ="modulo/WIFI";

extern GameState game_state;
extern SemaphoreHandle_t mutex; //creo que vamos a tener que cambiar los mutex por grupos de eventos para activar y desactivar que pase de un lado o de otro
extern EventGroupHandle_t event_game;

extern const int player_name_rady_bit, new_move_bit, game_over_bit, py1_btn_bit,py2_btn_bit, update_display;




void init_spiffs(void){
    esp_vfs_spiffs_conf_t conf ={
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "error al iniciar SPIFFS (%s)", esp_err_to_name(ret));
        return;
    }

    size_t total = 0, used = 0;

    ret = esp_spiffs_info(NULL, &total, &used);
    if(ret != ESP_OK){
        ESP_LOGE(TAG, "No se puede obtener la informacion de la particion SPIFFS (%s)", esp_err_to_name(ret));
    }
    else{
        ESP_LOGI(TAG, "SPIFFS: tamanio total %d, usado %d", total, used);
    }
}




void wifi_init_softap(void){
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    const char mi_ssid[]= "mi_esp_ap";

    wifi_config_t wifi_config={
        .ap ={
            .ssid_len = strlen(mi_ssid),
            .channel = 1,
            .password = "123456789",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    memcpy(wifi_config.ap.ssid, mi_ssid, strlen(mi_ssid));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "ESP32 AP iniciado SSID: %s password: %s channel: %d", 
             wifi_config.ap.ssid, wifi_config.ap.password, wifi_config.ap.channel);
}


/**
 * en esta funcion tal vez podria mostrar
 * 
 * 
 */
esp_err_t root_get_handler(httpd_req_t *req){

    FILE *file = fopen("/spiffs/index.html", "r");
    if(file==NULL){
        ESP_LOGE(TAG,"no se pudo abrir el archivo index.html");
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[255];
    while (fgets(line, sizeof(line), file)){
        //req solicitud HTTP en ete caso GET por lo que mandara al navegador lo que haya en el HTML, y la line son los tronoz que estara enviando hasta que este llege al final los envia en pedazos de 255 bytes
        httpd_resp_sendstr_chunk(req, line);
    }

    //cerramos la comunicacion

    httpd_resp_sendstr_chunk(req, NULL);
    fclose(file);
    return ESP_OK;
}

esp_err_t game_get_handler(httpd_req_t *req){

    FILE *file = fopen("/spiffs/game.html", "r");
    if(file==NULL){
        ESP_LOGE(TAG,"no se pudo abrir el archivo index.html");
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[255];
    while (fgets(line, sizeof(line), file)){
        //req solicitud HTTP en ete caso GET por lo que mandara al navegador lo que haya en el HTML, y la line son los tronoz que estara enviando hasta que este llege al final los envia en pedazos de 255 bytes
        httpd_resp_sendstr_chunk(req, line);
    }

    //cerramos la comunicacion

    httpd_resp_sendstr_chunk(req, NULL);
    fclose(file);
    return ESP_OK;
}


esp_err_t css_get_handler(httpd_req_t *req){

    
    FILE *file = fopen("/spiffs/styles.css", "r");
    if(file==NULL){
        ESP_LOGE(TAG,"no se pudo abrir el archivo para lectura");
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    
    httpd_resp_set_type(req, "text/css");

    char line[255];
    while (fgets(line, sizeof(line), file)){
        //req solicitud HTTP en ete caso GET por lo que mandara al navegador lo que haya en el HTML, y la line son los tronoz que estara enviando hasta que este llege al final los envia en pedazos de 255 bytes
        httpd_resp_sendstr_chunk(req, line);
    }

    //cerramos la comunicacion

    httpd_resp_sendstr_chunk(req, NULL);
    fclose(file);
    return ESP_OK;


}



/*
esp_err_t get_state(httpd_req_t *req){
    //se supone que este va a mostrar en pantalla el estaod del juego actual 

    char buff[100];
}*/





esp_err_t start_player(httpd_req_t *req){


    char buff[32];


    //recibidneod los datos 
    int ret, reaming = req->content_len;

    ret= httpd_req_recv(req,buff,MIN(reaming, sizeof(buff)));

    if(ret <=0){
        return ESP_FAIL;
    }

    //terminamos el string
    buff[ret]='\0';

    ESP_LOGI(TAG,"informacion ingresada: %s", buff);
    
    

    char *player1 = strstr(buff, "player1=");
    char *player2 = strstr(buff, "player2=");

    //es que player tiene algo, algo ingreso el ueusaior correctamsnte, 
    if(player1 && player2){
        //se brinca la parte de player de abmos para ahora si estar en el nombre como tal que ingreso el usuario, y sabemos que el final de esa cadena esta en terminacion 
        //de caracter nulo
        player1+=8;
        player2+=8; 

        char *end1 = strchr(player1, '&');
        if(end1){
            *end1 = '\0';
        }

         char *end2 = strchr(player2, '&');
        if(end2){
            *end2 = '\0';
        }

        set_player_names(player1,player2);
        xEventGroupSetBits(event_game, player_name_rady_bit);

        //redirigimos a la pantalla del juego 

        httpd_resp_set_status(req, "303 See Other");
        httpd_resp_set_hdr(req, "Location", "/game.html");
        httpd_resp_send(req, NULL, 0);

        ESP_LOGI(TAG, "jugadores registrados : %s - %s", game_state.player1_name, game_state.player2_name);
    }
    else{
        httpd_resp_sendstr(req, "Error datos de envio");
        return ESP_FAIL;
    }

    return ESP_OK;
}



void set_player_names(const char *player1, const char *player2){

    strncpy(game_state.player1_name, player1, sizeof(game_state.player1_name)-1);
    game_state.player1_name[sizeof(game_state.player1_name)-1]='\0'; //termino la cadena 

    strncpy(game_state.player2_name, player2, sizeof(game_state.player2_name)-1);
    game_state.player2_name[sizeof(game_state.player2_name)-1]='\0'; //termino la cadena 

    
    ESP_LOGI(TAG, "Nombres establecidos: %s, %s", player1, player2);

    // xEventGroupSetBits(event_game,player_name_rady_bit);

}



// Handler para movimientos
esp_err_t movent_handler(httpd_req_t *req) {
    if (game_state.game_over) {
        httpd_resp_sendstr(req, "JUEGO_TERMINADO");
        return ESP_OK;
    }

    char buff[32];
    int ret, remaining = req->content_len;

    ret = httpd_req_recv(req, buff, MIN(remaining, sizeof(buff)));
    if (ret <= 0) {
        return ESP_FAIL;
    }

    buff[ret] = '\0';
    ESP_LOGI(TAG, "Movimiento recibido: %s", buff);

    char *col_str = strstr(buff, "col=");
    if (col_str) {
        col_str += 4;
        int column = atoi(col_str) - 1;

        if (column >= 0 && column < COLS) {
            int result = make_move(column);
            if (result != -1) {
                xEventGroupSetBits(event_game, new_move_bit);
                httpd_resp_sendstr(req, "OK");
            } else {
                httpd_resp_sendstr(req, "INVALID_MOVE");
            }
        } else {
            httpd_resp_sendstr(req, "INVALID_COLUMN");
        }
    } else {
        httpd_resp_sendstr(req, "INVALID_FORMAT");
    }

    return ESP_OK;
}


// Handler para estado del juego (API JSON)
esp_err_t game_state_handler(httpd_req_t *req) {
    char response[2048];
    
    snprintf(response, sizeof(response),
        "{\"board\":%s,\"info\":%s}",
        get_board_json(),
        get_game_info_json());
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    
    return ESP_OK;
}

void start_web_server(void){

    
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    //este metpp uri es la direccion raiz que se muestra, es metodo GET 
    //aqui me muestra a pagina donde se registran los jugadores 
    if(httpd_start(&server,&config) == ESP_OK){

        //archivo estatico 
        httpd_uri_t root_uri={
            .uri="/",
            .method = HTTP_GET,
            .handler = root_get_handler,
        };

        //regritamos la uri que respondera a la peticion 
        httpd_register_uri_handler(server, &root_uri);


        httpd_uri_t game_uri = {
            .uri = "/game.html",
            .method = HTTP_GET,
            .handler = game_get_handler
        };
        httpd_register_uri_handler(server, &game_uri);




        //manejador que va a registrar los usuarios 
        httpd_uri_t start_uri={
            .uri="/start",
            .method=HTTP_POST,
            .handler = start_player,
        };

        httpd_register_uri_handler(server, &start_uri);


        httpd_uri_t recv_post={
            .uri="/move",
            .method=HTTP_POST,
            .handler = movent_handler,
        };

        httpd_register_uri_handler(server, &recv_post);

         httpd_uri_t state_uri = {
            .uri = "/game_state",
            .method = HTTP_GET,
            .handler = game_state_handler
        };
        httpd_register_uri_handler(server, &state_uri);

    }

}



