#include"gpio_lib.h"
#include<driver/gpio.h>
#include<esp_timer.h>
#include "esp_attr.h"

#define PLAYER_1 17
#define PLAYER_2 16
#define DEBOUNCE_TIME 200000

static uint64_t LAST_PRESS_PY1 = 0;
static uint64_t LAST_PRESS_PY2 = 0;


void gpio_init(void) {
    // Usaremos el GPIO 17 para jugador 1
    // GPIO 16 para jugador 2

    gpio_reset_pin(PLAYER_1);
    gpio_reset_pin(PLAYER_2);

    gpio_set_direction(PLAYER_1, GPIO_MODE_INPUT);
    gpio_set_direction(PLAYER_2, GPIO_MODE_INPUT);

    // Pull-ups habilitados
    gpio_pullup_en(PLAYER_1);
    gpio_pulldown_dis(PLAYER_1);

    gpio_pullup_en(PLAYER_2);
    gpio_pulldown_dis(PLAYER_2);
}


void type_int(void) {
    gpio_set_intr_type(PLAYER_1, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(PLAYER_2, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(0);

    // Manejador de interrupciones
    gpio_isr_handler_add(PLAYER_1, gpio_isr_handler,(void *)PLAYER_1);
    gpio_isr_handler_add(PLAYER_2, gpio_isr_handler,(void *)PLAYER_2);
}


void IRAM_ATTR gpio_isr_handler(void *args) {
    uint32_t pin_number = (uint32_t)args;
    uint64_t current_time = esp_timer_get_time();

    switch(pin_number) {
        case PLAYER_1: {
            if((current_time - LAST_PRESS_PY1) > DEBOUNCE_TIME) {
                if(gpio_get_level(pin_number) == 0) {
                    LAST_PRESS_PY1 = current_time;
                    // Aquí podrías enviar a una cola si la tuvieras
                }
            }
            break;
        }

        case PLAYER_2: {
            if((current_time - LAST_PRESS_PY2) > DEBOUNCE_TIME) {
                if(gpio_get_level(pin_number) == 0) {
                    LAST_PRESS_PY2 = current_time;
                    // Aquí podrías enviar a una cola si la tuvieras
                }
            }
            break;
        }
    }
}