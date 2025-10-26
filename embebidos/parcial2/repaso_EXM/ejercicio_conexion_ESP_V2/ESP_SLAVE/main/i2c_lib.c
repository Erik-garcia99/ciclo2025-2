#include"i2c_lib.h"
// #include<driver/i2c.h>
#include<driver/i2c_slave.h>
#include<freertos/FreeRTOS.h>
#include<driver/i2c_types.h>
#include<esp_log.h>
#include <string.h> 

#define ACK_ENABLE 0x1


const char *TAG = "I2C_ESP_SLAVE";


uint8_t i2c_rx_buffer[RX_BUFFER];
size_t i2c_rx_bytes = 0;
bool i2c_data_ready = false;




//funciones que sirven con la vieja api
/*
esp_err_t i2c_slave_init(i2c_port_t port_num,int sda_pin, int scl_pin, uint16_t slave_addr){

    esp_err_t ret;

    i2c_config_t i2c_slave ={

        .mode = I2C_MODE_SLAVE,
        .sda_io_num = sda_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = scl_pin,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = slave_addr,
    };

    ret = i2c_param_config(port_num, &i2c_slave);

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "erro al configurar los parametros:  %s", esp_err_to_name(ret));
        return ret;
    }
    return i2c_driver_install(port_num,i2c_slave.mode,RX_BUFFER,TX_BUFFER,0);
}*/

/*
esp_err_t i2c_master_send(uint8_t *data, uint8_t len, uint16_t salve_addr, i2c_port_t port_num){

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, salve_addr << 1 | I2C_MASTER_WRITE, ACK_ENABLE);
    i2c_master_write(cmd, data, len, ACK_ENABLE);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(port_num, cmd, 1000/portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;

}*/

//------------->> FUNCIONES CON LA NUEVA API 

esp_err_t i2c_slave_init(i2c_port_t port_num,int sda_pin, int scl_pin, uint16_t slave_addr, i2c_slave_dev_handle_t *i2c_slave_handler){

    i2c_slave_config_t i2c_slave ={
        .i2c_port= port_num,
        .sda_io_num = sda_pin,
        .scl_io_num=scl_pin,
        .clk_source= I2C_CLK_SRC_DEFAULT,
        .slave_addr = slave_addr,
        .send_buf_depth = TX_BUFFER,
    };

    esp_err_t ret = i2c_new_slave_device(&i2c_slave, i2c_slave_handler);

    // Configurar callbacks
    i2c_slave_event_callbacks_t cbs = {
        .on_recv_done = i2c_slave_rx_done_callback,
    };

    ret = i2c_slave_register_event_callbacks(*i2c_slave_handler, &cbs, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al registrar callbacks: %s", esp_err_to_name(ret));
        return ret;
    }

    // Iniciar la primera recepción
    ret = i2c_slave_receive(*i2c_slave_handler, i2c_rx_buffer, RX_BUFFER);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al iniciar recepción: %s", esp_err_to_name(ret));
        return ret;
    }


    return ret;
}

static bool i2c_slave_rx_done_callback(i2c_slave_dev_handle_t slave_handle, const i2c_slave_rx_done_event_data_t *event_data, void *user_data){


    ESP_LOGI(TAG, "Callback: Datos recibidos - %d bytes", event_data->buffer);

    if (event_data->rx_data_len > 0 && event_data->rx_data_len <= RX_BUFFER) {
        memcpy(i2c_rx_buffer, event_data->data, event_data->rx_data_len);
        i2c_rx_bytes = event_data->rx_data_len;
        i2c_data_ready = true;
        
        ESP_LOG_BUFFER_HEXDUMP(TAG, i2c_rx_buffer, i2c_rx_bytes, ESP_LOG_INFO);
    }

     // Reiniciar recepción para el próximo mensaje
    esp_err_t ret = i2c_slave_receive(slave_handle, i2c_rx_buffer, RX_BUFFER);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al reiniciar recepción: %s", esp_err_to_name(ret));
    }
    
    return true;

}