#include "uart_lib.h"
#include<driver/uart.h>
#include<esp_log.h>
#include<freertos/queue.h>
#include<freertos/event_groups.h>
#include<stdlib.h>
/**
 * 
 *programa que maneja los estados de uart, inicar, trasmitir, recibir, tratar lo que recibe si es el caso  
 * 
*/


QueueHandle_t event_uart;




int input_index = 0;
unsigned char input[BUFF] = {0};

void uart_init(uart_port_t uart_num, uart_word_length_t len_frame, uart_parity_t uart_parity, uart_stop_bits_t uart_stop_bits, int TX, int RX){


    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = len_frame,
        .parity = uart_parity,
        .stop_bits = uart_stop_bits,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };


    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    //lo degamos la cola en caso de tener que ingresar algo por uart, o tal vez si haga el de mandar de un dato al otro ESP
    ESP_ERROR_CHECK(uart_driver_install(uart_num,BUFF*2, BUFF*2,20, &event_uart,0));

    ESP_ERROR_CHECK(uart_set_pin(uart_num, TX,RX,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

}

//el ESP SLAVE no va a recibir nada desde RX por lo que esto no es necesario, sera comentado 
/*
void receive_task(void *params){

    //convertimos el puntero a la estrucutrua 

    param_uart_t *pvParams = (param_uart_t *)params;

    //se suporne que debera de tener el uart al cual queremos receibir o etc 


    //necesimaot el buffer ciruclar por donde se ingresaran los datos por UART 

    uint8_t *buffer = malloc(BUFF);

    uart_event_t event;

    while(1){
        
        if(xQueueReceive(event_uart, (void*)&event, portMAX_DELAY)){

            switch(event.type){

                case UART_DATA : {
                    //en este caso recicio algo por RX 

                    int len = uart_read_bytes(pvParams->num_uart, buffer, event.size,portMAX_DELAY);
                    //lo que haremos es pasar lo que ingresmos en es EPS master al SALVE 

                    for(int i=0; i< len; i++){

                        unsigned char c = buffer[i];
                        //ahora debemos de saber que es lo que ingreso 

                        //primero para saber si se termino la cadena es con \n y \b 

                        if(c == '\n' || c == '\r'){

                            if(input_index > 0){
                                
                                input[input_index] = '\0'; //terminamos con la deacena 
                                input_index = 0;
                                xEventGroupSetBits(pvParams->status_uart_S, RX_COMPLETE);
                                break; //lo ultimo para salir del ciclo
                                
                                
                            }
                        }

                        else if((c>='0' && c <= '9') || (c >= 'a' && c<='z') || (c>='A' && c<='Z')){
                            input[input_index]= c;
                            input_index++;
                            uart_write_bytes(pvParams->num_uart, (const char*)&c, 1);
                        }

                        else if(c == '\b'){
                            uart_write_bytes(pvParams->num_uart, "\b \b",3);
                            input_index--;
                        }
                    }



                        
                }break;

                case UART_FIFO_OVF:
                case UART_BUFFER_FULL:{
                    uart_flush_input(pvParams->num_uart);
                    xQueueReset(event_uart);
                    ESP_LOGW("UART", "Buffer UART lleno, limpiando...");
                } break;



                default:{
                    const char *mess = "tipo de evento desconocido\n";
                    uart_write_bytes(pvParams->num_uart,mess,strlen(mess));
                }break;
            }

        }


    }
    


}

*/
