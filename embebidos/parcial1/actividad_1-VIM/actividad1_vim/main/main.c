#include<stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<freertos/event_groups.h>
#include<freertos/queue.h>
#include<driver/gpio.h>
#include<driver/uart.h>
#include"uart_lib.h"
#include<esp_log.h>	
#include<string.h>
#include <stdlib.h>



const static char *TAG = "UART1";

static const char lf[] = "\n";

EventGroupHandle_t status_uart;

const int array_ready = BIT0; //bit que me dice que ya se ingreso el tamanio del arreglo 

static bool ready = false; //indicando que aun no se ha asigando el tamanio del arreglo

void app_main(void)
{

	status_uart = xEventGroupCreate();

	init_uart(UART_SEL,2,4);
	xTaskCreate(recive_uart_task, "uart_task",4098,NULL,9,NULL);
	xTaskCreate(uart_task_main,"uart_main",4098,NULL,10,NULL);


}

//###################################################################

//tarea prinicpoal:

void uart_task_main(void *params){


	while(1){



	}



}





//seccion de tareas
void recive_uart_task(void *params){

	
	uart_event_t event;
	//por esto puedo vailar, el parametro de malloc es el tamanio que se le dara, en este caso sera del tamanio del buffer
	uint8_t *buffer = malloc(BUF);

	while(1){
		
		if(xQueueReceive(uart_event,(void *)&event,portMAX_DELAY)){
		
			memset(buffer, 0 , BUF);
		
			switch(event.type){
			
				case UART_DATA:{

					//se recivio la infromacion 
					uart_read_bytes(UART_SEL, buffer,event.size,portMAX_DELAY); //espera hasta que le llegue algo
					
					//ahora depende de lo que se reciba veremos que onda 
					
					

					for(int i=0; i< event.size; i++){
					
						char c =(char)buffer[i]; //sacamos el primer dato del buffer, porque puede haber mas de 1 para poder decidir 
					
						if(c >= '0' && c <='9'){
							
							input_buffer[input_index++] = c;
							uart_write_bytes(UART_SEL,(const char*)&c, sizeof(c));
						
						}
						else if(c == '\n' || c == '\r'){
						
							//legamos al final de la linea 
							if(input_index > 0){
							
								//debio de haber ingresado algo al arreglo
								input_buffer[input_index]= '\0';
								//char ptr;
								//int i=0;
								uart_write_bytes(UART_SEL,"dato ingresado:\n",20);
								uart_write_bytes(UART_SEL,input_buffer,strlen(input_buffer));

								procces_input(input_buffer);

								/*
								while(ptr!="\0"){
									ptr =input_buffer[i];
								       	uart_write_bytes(UART_SEL,(const char*)&ptr,sizeof(ptr));
									i++;	
								}*/
								uart_write_bytes(UART_SEL, lf, 1);
								

								memset(input_buffer,0,256);//limpamos el buffer
								input_index= 0;
							}
						
						}
						else if(c == '\b' || c == 127){
						
							input_index--;
							uart_write_bytes(UART_SEL, "\b \b",3);
						}
					
					}
					
				
				}break;


				case UART_BUFFER_FULL:{

					//limpamos la colca 
					xQueueReset(uart_event);
					uart_flush_input(UART_SEL);
					ESP_LOGI(TAG, "buffer full");
				}break;

				case UART_FIFO_OVF:{

					//limpiamos la fiflo 

					xQueueReset(uart_event);
					uart_flush_input(UART_SEL);
					ESP_LOGI(TAG, "OVF FIFO");
				}break;

				case UART_FRAME_ERR:{
					const char *error = "frame error";
					uart_write_bytes(UART_SEL,error, strlen(error));
					uart_write_bytes(UART_SEL, lf, 1);
				} break;

				case UART_PARITY_ERR:{
					const char *error = "parity error";

					uart_write_bytes(UART_SEL,error, strlen(error));
					uart_write_bytes(UART_SEL,lf, 1);
				}break;

				default: {

					const char *other_value = "other error";
					uart_write_bytes(UART_SEL,other_value, strlen(other_value));
					uart_write_bytes(UART_SEL,lf, 1);

				}
			}
		}
	}




}





//#####################################################################
//seccion de funciones 

void init_uart(int num_uart, int pin_tx, int pin_rx){

	
	//definicion predefinida 
	uart_config_t uart_config = {
	
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl =UART_HW_FLOWCTRL_DISABLE, 
		.source_clk = UART_SCLK_DEFAULT,
	};



	if(num_uart == UART_NUM_0){
		//pongamos primero con el valos a trabajar y despues con el otros la verga
		
		ESP_ERROR_CHECK(uart_driver_install(UART_SEL,BUF*2, BUF*2,20,&uart_event,0));
		ESP_ERROR_CHECK(uart_param_config(UART_SEL,&uart_config));

		ESP_ERROR_CHECK(uart_set_pin(UART_SEL,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));

		uart_flush(UART_SEL);

		//ESP_LOGI(TAG,"UART configurado");
		const char *conf = "uart configurado\n";
		uart_write_bytes(UART_SEL,conf,strlen(conf));

	}
}



void procces_input(char *input){


	uint8_t number = (uint8_t)atoi(input);

	

	





}







