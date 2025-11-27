#include <stdio.h>
#include<string.h>


//modulos de freerots
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/event_groups.h>
#include<freertos/queue.h>

//manejador de errores 
#include<esp_err.h>
#include<esp_log.h>

//drivers 
#include<driver/uart.h>


//WIFI-FLASH
#include<esp_wifi.h>
#include<esp_spiffs.h>
#include<esp_flash.h>
#include<nvs_flash.h>
#include<esp_netif.h>
#include<esp_http_server.h>



//modulos porpios 
#include"modulos/WIFI/wifi_lib.h"
#include"modulos/UART/uart_lib.h"
#include"modulos/GPIO/gpio_lib.h"


//macros 
//el ESP de AP de cominicara con EL ESP LM75AB por el UART 



//varibales 


//funciones 


//tareas 

void task_main(void *params);

//esta tarea se encarga de estar escuchando cunado el ESP que se encarga de la temperatura le manda algo por UART 
static void deamon_uart_TMEP(void *params);


void app_main(void)
{

    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    //servidor - WIFI
    init_spiffs();
    wifi_init_softap();
    start_web_server();


    //inicamos tareas 

    xTaskCreate(task_main, "task_main", 4098, NULL, 9, NULL);
    xTaskCreate(deamon_uart_TMEP, "task_TMP",4098, NULL, 10, NULL);


}




static void task_main(void *params){


    while(1){






    }


}


void deamon_uart_TMEP(void *params){

    uart_event_t event; 



}
