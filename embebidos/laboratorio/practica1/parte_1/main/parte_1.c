#include <stdio.h>
#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"freertos/queue.h"
#include"driver/gpio.h"

#define INPUT_PIN 17
#define LED_PIN 2

int state = 0;
QueueHandle_t handlerQueue;



void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(INPUT_PIN);
    gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(INPUT_PIN);
    gpio_pullup_dis(INPUT_PIN);
    gpio_set_intr_type(INPUT_PIN,GPIO_INTR_POSEDGE);

    handlerQueue = xQueueCreate(10,sizeof(int));

    xTaskCreate(LED_Control_Task, "LED_control_Task", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(INPUT_PIN,gpio_interrupt_handler, (void *)INPUT_PIN);
}


static void IRAM_ATTR gpio_interrupt_handler(void *args){
    int pinNumber = (int)args;

    xQueueSendFromISR(handlerQueue, &pinNumber, NULL);
}


void LED_control_task(void *params){
    int pinNumber, count =0;


    while(true){
        if(xQueueReceive(handlerQueue, &pinNumber, portMAX_DELAY)){
            pinrtf("GPIO %d was prossed %d times. the state is %d\n");
        }

        gpio_set_level(LED_PIN, gpio_get_level(INPUT_PIN));
    }
}

