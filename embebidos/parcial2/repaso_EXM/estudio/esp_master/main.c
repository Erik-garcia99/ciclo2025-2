#include <stdio.h>
#include "modulos/I2C/i2c_lib.h"
#include"modulos/UART/uart_lib.h"
#include<stdlib.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/uart.h>
#include <portmacro.h>
#include<esp_log.h>

//tal vez enviemos numeros de 1 a 10 al salve y que lo imprima, no se que podemos hcer jjjk 


//macros
//I2C 
#define ESP_SALVE 0x40 
#define I2C_SL I2C_NUM_0
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define SPEED 100000

//UART
#define UART_PORT UART_NUM_0
//si usamos otro uart podemos estbalecer los pines pero si es el 0 por lo genreal no se cambia ya que es con este con elq ue se comunica con la PC 
// #define TX_PIN 
// #define RX_PIN 

//genreales

static const char *TAG = "ESP_MASTER";

//funciones 

//tareas 

void task_main(void *params);


void app_main(void)
{

    uart_init(UART_PORT,UART_DATA_8_BITS, UART_PARITY_DISABLE, UART_STOP_BITS_1, 1, 3);

    init_I2C_master(I2C_SL,I2C_SDA_PIN, I2C_SCL_PIN, SPEED);

    xTaskCreate(task_main, "task_main", 4098, NULL, 10, NULL);
    
}


void task_main(void *params){



    uint8_t acc = 0; // esta varibale es la que le vamos a pasar al esclavo para que lo imprima 

    while(1){

        esp_err_t log = i2c_master_send(I2C_SL, &acc, sizeof(acc), ESP_SALVE);

        if(log != ESP_OK){
            const char *mess = "error al pasar el argumento\n";
            uart_write_bytes(UART_NUM_0, mess, strlen(mess));
            break;
        }
        else{
            

            ESP_LOGI(TAG, "dato enviado: %d" , acc);
            vTaskDelay(500/ portTICK_PERIOD_MS);
            acc++;
            acc %= 10;
        }
        

    }

    vTaskDelete(NULL);


}
