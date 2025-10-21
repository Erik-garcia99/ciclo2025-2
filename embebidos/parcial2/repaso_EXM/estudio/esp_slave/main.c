//librerias propias
#include "modulos/I2C/i2c_lib.h"
#include "modulos/UART/uart_lib.h"

//liberias necesarias para la operacion en ESP
#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<esp_log.h>
#include<string.h>

//perifericos
#include<driver/i2c.h>
#include<driver/uart.h>
#include<driver/i2c_types.h>

/**
 * 
 * este proyecto es mucho mas sencillo ya que las clases ya estas listas como quien dice, pero el I2C ocupamos inicar nuestro slave, el cual tiene creo que 1 o 2 parametros distitnos  
 * 
 * 
*/


//macros 

#define I2C_SLAVE I2C_NUM_0
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_SALVE_ADDR 0x40

#define UART_PORT UART_NUM_0


static const char *TAG ="UART_SLAVE";

//tareas 
void task_main_salave(void *params);



void app_main(void)
{

    init_I2C_slave(I2C_SLAVE, I2C_SDA_PIN,I2C_SCL_PIN,I2C_SALVE_ADDR);

    uart_init(UART_PORT, UART_DATA_8_BITS,UART_PARITY_DISABLE,UART_STOP_BITS_1,1, 3);

    xTaskCreate(task_main_salave, "task slave", 4098, NULL, 10, NULL);

}
void task_main_salave(void *params){
    uint8_t rx_buffer[I2C_RX_BUFF] = {0};
    int bytes_read = 0;

    while(1){
        bytes_read = i2c_slave_read_buffer(I2C_SLAVE, rx_buffer, I2C_RX_BUFF, 1000 / portTICK_PERIOD_MS);
        if(bytes_read > 0){
            // Escribir solo los bytes leídos
            uart_write_bytes(UART_PORT, (const char *)rx_buffer, bytes_read);
        } else {
            ESP_LOGI(TAG, "No se recibieron datos (bytes_read = %d)", bytes_read);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}