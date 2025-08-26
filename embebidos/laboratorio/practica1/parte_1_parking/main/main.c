#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "parking.h"
#include "driver/gpio.h"

//definamos nuestros pines de entrada y de salida 
//necesitamos los 4 pines para la entrada de botones



void app_main(void)
{
    init_GPIO();

    while (true)
    {
        
    }
    
    
}


void init_GPIO(void){

    //configuramos el PIN como GPIO
    //INPUT
    gpio_reset_pin(UN_PESO_IN);
    gpio_reset_pin(CINCO_PESO_IN);
    gpio_reset_pin(DIEZ_PESO_IN);
    gpio_reset_pin(VEINTE_PESO_IN);

    //OUTPUT
    gpio_reset_pin(UN_PESO_OUT);
    gpio_reset_pin(CINCO_PESO_OUT);
    gpio_reset_pin(DIEZ_PESO_OUT);
    gpio_reset_pin(VEINTE_PESO_OUT);
    //configuracion como salida 
    




    //configuracion como entrada 
    //la entrada es el que ocupa el pull-up / pull-down


}