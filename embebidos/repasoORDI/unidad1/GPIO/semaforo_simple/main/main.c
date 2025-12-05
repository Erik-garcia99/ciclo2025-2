#include <stdio.h>

#include<freertos/FreeRTOS.h>
#include<freertos/task.h>

//incluimos la libreria de gpio
#include<driver/gpio.h>


void configurar_pin();



void app_main(void)
{

    //13 rojo
    //14 amarillo
    //12 verde

    configurar_pin();
    while(1){

        gpio_set_level(13, 1);
        vTaskDelay(3000/portTICK_PERIOD_MS);

        gpio_set_level(13, 0);
        gpio_set_level(14, 1);

        vTaskDelay(1000/portTICK_PERIOD_MS);
        gpio_set_level(14, 0);
        gpio_set_level(12, 1);

        vTaskDelay(3000/portTICK_PERIOD_MS);
        gpio_set_level(12, 0);


    }


}


void configurar_pin(){


    //configuramos 3 GPIO como salida 
    //13 -  14 - 16
    gpio_reset_pin(13);
    gpio_reset_pin(14);
    gpio_reset_pin(12);


    gpio_set_direction(13,GPIO_MODE_OUTPUT);
    gpio_set_direction(14,GPIO_MODE_OUTPUT);
    gpio_set_direction(12,GPIO_MODE_OUTPUT);

    gpio_set_level(13, 0);
    gpio_set_level(14, 0);
    gpio_set_level(12, 0);



}