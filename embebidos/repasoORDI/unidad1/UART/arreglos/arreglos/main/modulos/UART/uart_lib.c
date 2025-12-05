
#include<string.h>
#include<stdlib.h>

#include<driver/uart.h>
#include<esp_err.h>
#include<esp_log.h>
#include"uart_lib.h"

static const char *TAG ="UART_LIBRERY";

extern QueueHandle_t uart_queue, data_queue;

void init_uart(uart_port_t uart_num, uart_word_length_t data_len,uart_stop_bits_t uart_stop_b, uart_parity_t parity_mode, int rx_pin, int tx_pin){

    uart_config_t config={

        .baud_rate = 115200,
        .data_bits=data_len,
        .parity=parity_mode,
        .stop_bits=uart_stop_b,
        .flow_ctrl=UART_HW_FLOWCTRL_DISABLE,
        .source_clk=UART_SCLK_DEFAULT,
    };


    ESP_ERROR_CHECK(uart_param_config(uart_num, &config));

    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUFFER * 2, BUFFER *2, 20, &uart_queue, 0));

    char exito[] = "uart inicalizod correctamente\n";
    uart_write_bytes(uart_num, exito, strlen(exito));

}


void uart_task(void *params){

    uart_event_t event;
    uint8_t *buffer=(uint8_t*)malloc(BUFFER);
    uint8_t data[BUFFER];
    int index_data=0;
    memset(data, 0, BUFFER);
    while(1){

        if(xQueueReceive(uart_queue, (void*)&event,portMAX_DELAY)){
            memset(buffer, 0, BUFFER);
            //memset(data, 0, BUFFER);

            switch(event.type){

                case UART_DATA:{

                    int len = uart_read_bytes(UART_SEL, buffer, event.size, portMAX_DELAY);
                    
                    for(int i=0; i<event.size; i++){
                        char c=buffer[i];
                        if((c >= 'a' && c<= 'z') || 
                            (c >= 'A' && c<='Z') || 
                            (c>='0' && c<='9') || (c ==' ')){
                            uart_write_bytes(UART_SEL,(const char*)&c,sizeof(c));

                            if(index_data < BUFFER-1){
                                data[index_data++] = c;
                            }
                            
                        }
                        else if(c== '\n' || c =='\r'){
                            data[index_data]='\0';
                            
                            // char exito[]="ingresado con exito";
                            // uart_write_bytes(UART_SEL, exito, strlen(exito));
                            //ahora enviamos por la cola los datos ingresados 

                            //es necesario crear un copia en moemoria dinamica para cada mensaje porque lo uqe pasa 
                            //que se sobre escribn 
                            char *mensaje_nuevo = (char*)malloc(strlen((char*)data) + 1);
                            strcpy(mensaje_nuevo, (char*)data);

                            xQueueSend(data_queue, &mensaje_nuevo, 0);
                            //volvemos al inicio
                            index_data = 0;
                            memset(data, 0, BUFFER);
                            break;
                        }
                        else if(c=='\b'){
                            uart_write_bytes(UART_SEL, "\b \b",3);
                            index_data--;
                        }
                        
                    }

                }break;

                case UART_FIFO_OVF :{
                    xQueueReset(uart_queue);
                    uart_flush(UART_SEL);
                }break;

                case UART_BUFFER_FULL:{
                    xQueueReset(uart_queue);
                    uart_flush(UART_SEL);
                }break;

                default:{
                    ESP_LOGE(TAG, "tipo de evento %d", event.type);
                }break; 
            }

        }
    }
    free(buffer);
}