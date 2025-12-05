#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// --- DEFINICIONES ---
#define BTN_A     16  // Pin Selector de Velocidad
#define BTN_B     17  // Pin Paro de Emergencia
#define LED_PIN   13  // Pin del LED

// Estados de la máquina
enum State {
    STATE_IDLE = 0,
    STATE_V1,       // Lento
    STATE_V2,       // Rápido
    STATE_EMERGENCY
};

// --- GLOBALES ---
// Cola para comunicar interrupción -> Tarea
QueueHandle_t evt_queue = NULL; 

// Bandera Volátil para EMERGENCIA (Prioridad Absoluta)
volatile bool emergency_flag = false; 

// --- PROTOTIPOS ---
void setting_gpio(void);
void master_task(void *params);
// IRAM_ATTR debe ir ANTES del void para evitar warnings en algunas versiones de GCC
static void IRAM_ATTR gpio_isr_handler(void *args); 

// --- MAIN ---
void app_main(void)
{
    // 1. Crear la cola (Guardará enteros: el número del pin presionado)
    evt_queue = xQueueCreate(10, sizeof(int));

    // 2. Configurar Hardware
    setting_gpio();

    // 3. Crear la Tarea Maestra (Una sola tarea controla todo)
    xTaskCreate(master_task, "master_task", 2048, NULL, 10, NULL);
}

// --- CONFIGURACIÓN GPIO ---
void setting_gpio(void){
    gpio_config_t io_conf = {};

    // A. Configurar LED
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    
    // Iniciar apagado
    gpio_set_level(LED_PIN, 0);

    // B. Configurar Botones (Input + Pullup + Interrupción Negedge)
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Detectar bajada (presionar)
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BTN_A) | (1ULL << BTN_B);
    io_conf.pull_up_en = 1; // Pull-up activado
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);

    // C. Instalar ISR
    gpio_install_isr_service(0);
    // Asociar la misma función ISR a ambos botones (pasamos el número de pin como argumento)
    gpio_isr_handler_add(BTN_A, gpio_isr_handler, (void*)BTN_A);
    gpio_isr_handler_add(BTN_B, gpio_isr_handler, (void*)BTN_B);
}

// --- INTERRUPCIÓN (ISR) ---
// Esta función se ejecuta en microsegundos. NO uses printf ni delays aquí.
static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pin_number = (int)args;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (pin_number == BTN_B) {
        // --- CASO EMERGENCIA ---
        // Activamos la bandera inmediatamente.
        // Si ya estaba en emergencia, esto lo saca (Toggle).
        // Si quieres que solo active y se quede trabado, usa: emergency_flag = true;
        emergency_flag = !emergency_flag; 
        
    } else if (pin_number == BTN_A) {
        // --- CASO VELOCIDAD ---
        // Solo enviamos evento si NO estamos en emergencia
        if (!emergency_flag) {
            xQueueSendFromISR(evt_queue, &pin_number, &xHigherPriorityTaskWoken);
        }
    }

    // Si la cola despertó a la tarea principal, forzamos el cambio de contexto ya.
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

// --- TAREA MAESTRA (Lógica de Estados y Tiempos) ---
void master_task(void *params)
{
    int btn_received;
    int current_state = STATE_IDLE; // Estado actual de la máquina
    int led_level = 0;
    
    // Tiempos de parpadeo (Ticks)
    const TickType_t TIME_V1 = pdMS_TO_TICKS(500); // 1Hz (500ms on/off)
    const TickType_t TIME_V2 = pdMS_TO_TICKS(100); // 5Hz (100ms on/off)
    
    // Tiempo de espera dinámico
    TickType_t wait_time = portMAX_DELAY; 

    while(1) {

        // 1. VERIFICAR EMERGENCIA (Prioridad Top)
        if (emergency_flag) {
            // MODO FALLO: LED Encendido Fijo
            gpio_set_level(LED_PIN, 1); 
            // Pequeño delay para no quemar la CPU en un bucle infinito
            vTaskDelay(pdMS_TO_TICKS(100)); 
            continue; // Salta al inicio del while, ignora el resto
        }

        // 2. RECIBIR DE COLA O ESPERAR (Lógica del Timeout)
        // xQueueReceive espera 'wait_time'. 
        // - Si llega dato (Botón A) -> Retorna pdTRUE.
        // - Si se acaba el tiempo (Timeout) -> Retorna pdFALSE (Hora de parpadear).
        
        if (xQueueReceive(evt_queue, &btn_received, wait_time) == pdTRUE) {
            
            // --- EVENTO: BOTÓN PRESIONADO ---
            // Cambiamos al siguiente estado
            current_state++;
            if (current_state > STATE_V2) current_state = STATE_IDLE;

            // Configuramos el nuevo comportamiento
            switch (current_state) {
                case STATE_IDLE:
                    wait_time = portMAX_DELAY; // Dormir tarea hasta próximo botón
                    gpio_set_level(LED_PIN, 0); // Apagar LED
                    break;
                case STATE_V1:
                    wait_time = TIME_V1;      // Parpadear lento
                    break;
                case STATE_V2:
                    wait_time = TIME_V2;      // Parpadear rápido
                    break;
            }

        } else {
            // --- EVENTO: TIMEOUT (Nadie presionó nada) ---
            // Significa que pasó el tiempo de espera, toca invertir el LED
            if (current_state != STATE_IDLE) {
                led_level = !led_level;
                gpio_set_level(LED_PIN, led_level);
            }
        }
    }
}