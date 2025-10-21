#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include<stdlib.h>
#include "sort_array.h"
#include<freertos/event_groups.h>
#include<esp_log.h>
#include<string.h>
#include<driver/uart.h>
//se nos pone pendejo con scanf asi que usare UART
//para seguir preacticando que nos agarre con los calzones abajo 

QueueHandle_t status_uart; 

void app_main(){


  init_uart();

  //tarea que esta leyendo datos de UART

  //tarea que procesara elo datos ya en el buffer 


}



//#################TAREAS



void task_event_uart(void *params){

  //aqui a fuera, no quiero que lo cree y lo cree a cada rato. 
  uint8_t *buffer = malloc(BUFF);
  uart_event_t event;

  while(1){

    if(xQueueReceive(status_uart, (void *)&event, portMAX_DELAY)){
      memset(buffer,0,BUFF); //limpiamos el buffer circular, que este listo para recibir los datos 
      switch(event.type){


        case UART_DATA : {
          
          uart_read_bytes(UART_SEL,buffer, event.size, portMAX_DELAY);
          //echo
          uart_write_bytes(UART_SEL,(const char*)buffer, event.size);

          process_input(buffer, event.size);
           

          



        }break;

        default:{
          printf("si");
        }break;

      }


    }


  }





}


//#################FUNCIONES


void init_uart(){


  uart_config_t uart_config ={

    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity =UART_STOP_BITS_1 ,
    .stop_bits = UART_PARITY_DISABLE,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,

  };

  ESP_ERROR_CHECK( uart_param_config(UART_SEL, &uart_config));

  ESP_ERROR_CHECK(uart_driver_install(UART_SEL, BUFF*2, BUFF*2, 20, &status_uart,0));

  ESP_ERROR_CHECK(uart_set_pin(UART_SEL, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));

  uart_flush(UART_SEL);
  const char *config = "UART configurado";

  uart_write_bytes(UART_SEL, config, strlen(config));

}

void process_input(uint8_t *input, uint8_t size){


  uint8_t add = 0; 
  uint8_t i=0;
  uint8_t exp = 1;
  //sabemos el numero real 
  
  while(input[i] != '\n'){
    i++;
  }
  

  for(uint8_t j = i; j >= 0; j--){
    
    uint8_t current_data = input[j];
    current_data -= '0';
    
    current_data  *= exp;
    exp *=10;
    add +=current_data;

  }

  uart_write_bytes(UART_SEL, (const char*)&add, sizeof(add));


}



