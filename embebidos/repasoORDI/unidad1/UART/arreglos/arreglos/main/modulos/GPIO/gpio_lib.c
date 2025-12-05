
#include<driver/gpio.h>
#include"gpio_lib.h"


void init_gpio(){


    gpio_reset_pin(GPIO1);
    gpio_reset_pin(GPIO2);
    gpio_reset_pin(GPIO3);
    gpio_reset_pin(GPIO4);
    gpio_reset_pin(GPIO5);


    gpio_set_direction(GPIO1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO2, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO3, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO4, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO5, GPIO_MODE_OUTPUT);

    gpio_set_level(GPIO1,0);
    gpio_set_level(GPIO2,0);
    gpio_set_level(GPIO3,0);
    gpio_set_level(GPIO4,0);
    gpio_set_level(GPIO5,0);

}

