#include <stdio.h>
#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define ledR 33
#define ledG 25
#define ledB 26

const char *tag = "Main";

esp_err_t init_led(void);

esp_err_t create_task(void);





void app_main(void){
    init_led();
    while (true)
    {
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
        gpio_set_level(ledB,0);
    }
    
}


esp_err_t init_led(void){
    gpio_reset_pin(ledR);
    gpio_set_direction(ledR,GPIO_MODE_OUTPUT);
    gpio_reser_pin(ledG);
    gpio_set_direction(ledG,GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledB);
    gpio_set_direction(ledB,GPIO_MODE_OUTPUT);
    return ESP_OK; 
}


esp_err_t create_task(void){

    //las tareas no se llaman directamante, solo se crean y empiezan a ejecutarse 
    return ESP_OK;

}


