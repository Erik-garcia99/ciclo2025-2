#include <stdio.h>
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define ledR 33
#define ledG 25
#define ledB 26
#define R_dealy 1000
#define G_dealy 2000
#define B_dealy 4000

const char *tag = "Main";

esp_err_t init_led(void);

esp_err_t create_task(void);

void vTaskR(void *pvParameters);
void vTaskG(void *pvParameters);
void vTaskB(void *pvParameters);

void app_main(void){
    init_led();
    create_task(); //solo mandamos a llamar a la funcion que crea a las tareas, fuera del while, si esta dentro este causara un error
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        printf("hello form main\n");
        /*
        vTaskDelay(pdMS_TO_TICKS(500));

        gpio_set_level(ledR,1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(ledG,1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_set_level(ledB,1);
        vTaskDelay(pdMS_TO_TICKS(2000));

        //down
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(ledR,0);
        gpio_set_level(ledG,0);
        gpio_set_level(ledB,0);*/
    }

}


esp_err_t init_led(void){
    gpio_reset_pin(ledR);
    gpio_set_direction(ledR,GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledG);
    gpio_set_direction(ledG,GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledB);
    gpio_set_direction(ledB,GPIO_MODE_OUTPUT);
    return ESP_OK;
}


esp_err_t create_task(void){

    //las tareas no se llaman directamante, solo se crean y empiezan a ejecutarse
    
    //estos parametros los menciona la documetnacion que se deben de crear y pasarlo como punteros 


    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;
    // el primer parametro recibira la funcion la cual realizara la tarea 
    xTaskCreate(vTaskR, "vTaskR", 2048,&ucParameterToPass,1,&xHandle);

    xTaskCreate(vTaskG, "vTaskG", 2048,&ucParameterToPass,1,&xHandle);

    xTaskCreate(vTaskB, "vTaskB", 2048,&ucParameterToPass,1,&xHandle);
    
    return ESP_OK;

}

/**
 * 
 * lo que se esta haciendo aqui, es algo que es un poco diferente a como estamos acostumbrados a trabajr en C, la funcion << create_task >> lo que hace es crear las tareas 
 * estas funciones no se mandan a llamar como las funciones regulares que siempre hemos usadno en C, estas solo se declaran, lo que esta haceindo es que se estan ejecutando multiples procedimientos al "mismo tiempo", puesto que estamos uasndo un SO, el freeRTOS, la funciones que estan dentro, tienen esa estrucutra ya establecida por la propida docuemtnacion, creo que lo mas improtantes es el primer parametro el cual es el nombre de X tarea y despues como un identificador para relacionar la tarea, despues el 3ro es la cantidad de memoria que va a reservar en la pial, despues es
 * 
 * 1 aputnador 1que aun no se para que funciona, el penunitimo es para el nivel de prioridad de la tarea y el ultimo tampoco aun no se uqe onda. 
 */


void vTaskR(void *pvParameters){
    
    while (true)
    {  
        ESP_LOGI(tag,"LED R");
        //encendemos
        gpio_set_level(ledR,1);
        vTaskDelay(pdMS_TO_TICKS(R_dealy));
        //apagando
        gpio_set_level(ledR,0);
        vTaskDelay(pdMS_TO_TICKS(R_dealy));
    }
    
}


void vTaskG(void *pvParameters){
    
    while (true)
    {   
        ESP_LOGW(tag,"LED G");
        //encendemos
        gpio_set_level(ledG,1);
        vTaskDelay(pdMS_TO_TICKS(G_dealy));
        //apagando
        gpio_set_level(ledG,0);
        vTaskDelay(pdMS_TO_TICKS(G_dealy));
    }
    
}

void vTaskB(void *pvParameters){
    
    while (true)
    {   
        ESP_LOGE(tag,"LED B");
        //encendemos
        gpio_set_level(ledB,1);
        vTaskDelay(pdMS_TO_TICKS(B_dealy));
        //apagando
        gpio_set_level(ledB,0);
        vTaskDelay(pdMS_TO_TICKS(B_dealy));
    }
    
}