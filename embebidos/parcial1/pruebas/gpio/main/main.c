#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h" // Necesario para las colas
#include "driver/gpio.h"

// --- DEFINICIÓN DE PINES A MONITOREAR ---
#define GPIO_UP     18
#define GPIO_DOWN   19
#define GPIO_FINISH 21
#define GPIO_INIT   22

// Máscara de bits generada para configurar todos tus pines a la vez.
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_UP) | (1ULL<<GPIO_DOWN) | (1ULL<<GPIO_FINISH) | (1ULL<<GPIO_INIT))

// --- DECLARACIÓN GLOBAL DE LA COLA ---
// Esta variable será usada por la ISR y la tarea para comunicarse.
static QueueHandle_t gpio_evt_queue = NULL;


/**
 * @brief RUTINA DE SERVICIO DE INTERRUPCIÓN (ISR)
 * Se ejecuta al presionar un botón. Su única misión es enviar
 * el número de pin a la cola. Debe ser muy rápida.
 */
static void IRAM_ATTR gpio_interrupt_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    // Envía el dato a la cola de forma segura desde la ISR.
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

/**
 * @brief TAREA DE MANEJO DE GPIO
 * Esta tarea espera en un bucle a que lleguen datos a la cola.
 * Cuando recibe un número de pin, lo imprime en la terminal.
 * Aquí es donde se realiza el trabajo "lento" de forma segura.
 */
static void gpio_task_handler(void* arg)
{
    uint32_t io_num;
    // Bucle infinito para procesar eventos continuamente
    while(1) {
        // Espera a recibir un dato de la cola.
        // La tarea se bloqueará aquí, sin consumir CPU, hasta que llegue un evento.
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO #%lu activado.\n", io_num);
        }
    }
}


/**
 * @brief FUNCIÓN PRINCIPAL DE LA APLICACIÓN
 * Esta función se ejecuta una sola vez al arrancar el ESP32.
 * Su trabajo es configurar todo el sistema.
 */
void app_main(void)
{
    // 1. CONFIGURAR PINES GPIO
    gpio_config_t io_conf = {}; // Inicializar la estructura a ceros
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Interrupción al pasar de ALTO a BAJO
    io_conf.pull_up_en = 1; // Habilitar resistencia pull-up interna
    gpio_config(&io_conf);
    
    // 2. CREAR LA COLA
    // Se crea una cola que puede almacenar hasta 10 eventos (números de pin).
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    // 3. CREAR Y LANZAR LA TAREA DE MANEJO
    // Esta es la declaración y creación de la tarea que procesará los eventos.
    xTaskCreate(gpio_task_handler, "gpio_task_handler", 2048, NULL, 10, NULL);
    
    // 4. CONFIGURAR LAS INTERRUPCIONES
    // Instalar el servicio global de interrupciones para los pines GPIO.
    gpio_install_isr_service(0);

    // Asignar nuestra función ISR a cada pin específico.
    gpio_isr_handler_add(GPIO_UP, gpio_interrupt_handler, (void*) GPIO_UP);
    gpio_isr_handler_add(GPIO_DOWN, gpio_interrupt_handler, (void*) GPIO_DOWN);
    gpio_isr_handler_add(GPIO_FINISH, gpio_interrupt_handler, (void*) GPIO_FINISH);
    gpio_isr_handler_add(GPIO_INIT, gpio_interrupt_handler, (void*) GPIO_INIT);

    // Mensaje de inicio
    printf("Programa iniciado. Conecta un pin a GND para probar.\n");
    printf("- GPIO %d (_UP_)\n- GPIO %d (_DOWN_)\n- GPIO %d (_FINISH_)\n- GPIO %d (_INIT_)\n", GPIO_UP, GPIO_DOWN, GPIO_FINISH, GPIO_INIT);
}