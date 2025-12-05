#include <stdio.h>
#include<string.h>

#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/spi_master.h>
#include<driver/gpio.h>
#include<esp_log.h>
#include <portmacro.h>



#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5
#define BUFFER_LEN 16 

static const char *TAG = "SPI- HOST";

void app_main(void)
{

    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
    };

    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg={
        .clock_speed_hz = 1 * 1000 * 1000, 
        .mode=0,
        .spics_io_num= PIN_NUM_CS,
        .queue_size = 1,
        .flags =0,
        .cs_ena_posttrans = 16,
    };

    spi_device_handle_t handle;

    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &handle));


    char data[BUFFER_LEN] ={0};

    uint8_t i=0;

    spi_transaction_t trans;

    memset(&trans, 0, sizeof(trans));
    trans.length = 8*8;
    trans.tx_buffer = data;

    while(1){
        memset(data, 0, BUFFER_LEN);
        snprintf(data, BUFFER_LEN, "hola: %02X!", i++);
        ESP_ERROR_CHECK(spi_device_transmit(handle, &trans));
        ESP_LOGI(TAG, "datos enviados al periferico: %s", data);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    spi_bus_remove_device(handle);
    spi_bus_free(HSPI_HOST);

}