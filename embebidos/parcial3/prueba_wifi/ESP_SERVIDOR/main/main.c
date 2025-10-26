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

static const char *TAG ="web server";


void init_spiffs(void);
void wifi_init_softap(void);
void start_web_server(void);
esp_err_t html_get_handler(httpd_req_t *req);
esp_err_t css_get_handler(httpd_req_t *req);


void app_main(void)
{
    esp_err_t ret = nvs_flash_init(); 

    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    //inicializar SPIFFS
    init_spiffs();

    //iniciar wifi en modo access point 
    wifi_init_softap();

    //iniciar el servidor web
    start_web_server();
}

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


void start_web_server(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if(httpd_start(&server, &config) == ESP_OK){
        httpd_uri_t html_uri ={
            .uri = "/",
            .method = HTTP_GET,
            .handler = html_get_handler,
            .user_ctx= NULL
        };

        httpd_register_uri_handler(server, &html_uri);

        httpd_uri_t css_uri ={
            .uri = "/styles.css",
            .method = HTTP_GET,
            .handler = css_get_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &css_uri);

        ESP_LOGI(TAG, "Web server iniciado");
    }
}

//handler para servir archivo html 
esp_err_t html_get_handler(httpd_req_t *req){
    FILE * file = fopen("/spiffs/index.html", "r");
    if(file == NULL){  // CORREGIDO: == en lugar de =
        ESP_LOGE(TAG, "error al abrir el archivo de lectura");
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    char line[256];

    while(fgets(line, sizeof(line), file)){
        httpd_resp_sendstr_chunk(req, line);
    }

    httpd_resp_sendstr_chunk(req, NULL);

    fclose(file);

    return ESP_OK;
}

esp_err_t css_get_handler(httpd_req_t *req){
    FILE * file = fopen("/spiffs/styles.css", "r");
    if(file == NULL){  // CORREGIDO: == en lugar de =
        ESP_LOGE(TAG, "error al abrir el archivo de lectura");
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "text/css");
    char line[256];

    while(fgets(line, sizeof(line), file)){
        httpd_resp_sendstr_chunk(req, line);
    }

    httpd_resp_sendstr_chunk(req, NULL);

    fclose(file);

    return ESP_OK;
}