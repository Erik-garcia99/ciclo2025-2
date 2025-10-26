//librerias propias 
#include"uart_lib.h"
#include"i2c_lib.h"

//librerias bascias
#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<esp_log.h>
//liberias especiales - c


//perifericos
#include<driver/uart.h>
// #include<driver/i2c.h>
#include<driver/i2c_slave.h>
#include<driver/i2c_types.h>


//macros 
//macros de UART 
#define UART_SEL UART_NUM_0

//macros para I2C SLAVE 

#define I2C_PORT I2C_NUM_0
#define I2C_SDA_PIN 21 
#define I2C_SCL_PIN 22
#define I2C_SLAVE_ADDR 0x40

//variables 

static const char *TAG = "MAIN - TASK - SLAVE";
i2c_slave_dev_handle_t slave_handler;

extern uint8_t i2c_rx_buffer[RX_BUFFER];
extern size_t i2c_rx_bytes;
extern bool i2c_data_ready;




//funciones 

//tareas

void main_task(void *params);

void app_main(void)
{

    //inicamos uart 
    uart_init(UART_SEL, UART_DATA_8_BITS, UART_PARITY_DISABLE, UART_STOP_BITS_1, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    

    //iniamos I2C
    /*esp_err_t ret= i2c_slave_init(I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN, I2C_SLAVE_ADDR);
    
    if(ret != ESP_OK){

        ESP_LOGE(TAG, "error al inicar I2C %s", esp_err_to_name(ret));
    }*/

    esp_err_t ret = i2c_slave_init(I2C_PORT, I2C_SDA_PIN, I2C_SCL_PIN, I2C_SLAVE_ADDR, &slave_handler);

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "error al inicar el periferico I2C", esp_err_to_name(ret));
    }
    //iniciar tareas 

    xTaskCreate(main_task, "main_task", 4098, NULL, 8, NULL);
}
void main_task(void *params)
{
    while(1) {
        // Verificar si hay nuevos datos I2C listos para procesar
        if(i2c_data_ready) {
            ESP_LOGI(TAG, "Procesando %d bytes recibidos por I2C", i2c_rx_bytes);
            
            // Enviar por UART
            uart_write_bytes(UART_SEL, (const char*)i2c_rx_buffer, i2c_rx_bytes);
            
            // Limpiar flag
            i2c_data_ready = false;
            i2c_rx_bytes = 0;
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}