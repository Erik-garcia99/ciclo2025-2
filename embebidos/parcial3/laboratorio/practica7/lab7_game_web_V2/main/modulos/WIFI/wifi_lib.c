
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


static const char *TAG ="modulo/WIFI";



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


void start_web_server(void){

    
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    //este metpp uri es la direccion raiz que se muestra, es metodo GET 

    if(httpd_start(&server,&config) == ESP_OK){
        httpd_uri_t root_uri={
            .uri="/",
            .method = HTTP_GET,
            .handler = root_get_handler,
        };

        //regritamos la uri que respondera a la peticion 
        httpd_register_uri_handler(server, &root_uri);
    }

}
