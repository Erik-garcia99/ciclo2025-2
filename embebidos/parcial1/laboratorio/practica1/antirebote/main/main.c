#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// anodo pos+
#define INPUT_PIN 17
#define LED_PIN 2
#define DEBOUNCE_TIME 50 // TIEMPO EN Ms

// Variable para almacenar el tiempo de la última interrupción
static volatile int64_t last_interrupt_time = 0;

int state = 0;
QueueHandle_t handlerQueue;

// Declaraciones anticipadas de las funciones
static void IRAM_ATTR gpio_interrupt_handler(void *args);
void LED_control_task(void *params);

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(INPUT_PIN);
    gpio_set_direction(INPUT_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(INPUT_PIN);
    gpio_pullup_dis(INPUT_PIN);
    gpio_set_intr_type(INPUT_PIN, GPIO_INTR_POSEDGE);

    handlerQueue = xQueueCreate(10, sizeof(int));

    xTaskCreate(LED_control_task, "LED_control_Task", 2048, NULL, 1, NULL);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(INPUT_PIN, gpio_interrupt_handler, (void *)INPUT_PIN);
}

static void IRAM_ATTR gpio_interrupt_handler(void *args)
{
    int pinNumber = (int)args;
    
    int64_t current_time = esp_timer_get_time();//tiempo en uS
    //verificamos si es sufuciente tiempo para que la presion sobre el boton sea un valor casi constante
    if((current_time - last_interrupt_time) > (DEBOUNCE_TIME * 1000)){
        xQueueSendFromISR(handlerQueue, &pinNumber, NULL);
        last_interrupt_time = current_time;
    }
    
}

void LED_control_task(void *params)
{
    int pinNumber, count = 0;

    while (true)
    {
        if (xQueueReceive(handlerQueue, &pinNumber, portMAX_DELAY))
        {

            printf("GPIO %d was pressed %d times. The state is %d\n", 
                   pinNumber, count++, gpio_get_level(INPUT_PIN));
        }
        gpio_set_level(LED_PIN, gpio_get_level(INPUT_PIN));
    }
}


//como pondemos un antirebote, el que mas conozco y el que hicismo es por medio de tiempo, y al aprecer es el que la maestra nos recomendio idirectamante usar, usando TIMERS, por lo que hay que programar un antirebote. 











