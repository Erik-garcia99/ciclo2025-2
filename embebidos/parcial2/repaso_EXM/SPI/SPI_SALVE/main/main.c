#include <stdio.h>
#include<freertos/FreeRTOS.h>
#include<freertos/task.h>
#include<driver/spi_slave.h>
#include<driver/gpio.h>
#include<esp_log.h>
#include<string.h>
#include <esp_err.h>


#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 13

static const char *TAG = "SPI_SLAVE";


void app_main(void)
{

    spi_bus_config_t buscfg ={

        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num =PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,

    };

    spi_slave_interface_config_t svlcfg ={
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size =5,
        .flags =0
    };


    //inicalizar bus SPI en modo esclavo }

    ESP_ERROR_CHECK(spi_slave_initialize(HSPI_HOST,&buscfg, &svlcfg, SPI_DMA_CH_AUTO));

    char recvbuf[128] = {0};

    spi_slave_transaction_t trans;

    memset(&trans, 0, sizeof(trans));

    trans.length =  8*8; //longitud de bits

    trans.rx_buffer = recvbuf;

    while(1){


        ESP_LOGI(TAG, "esperando datos del controlador...");
        ESP_ERROR_CHECK(spi_slave_transmit(HSPI_HOST, &trans, portMAX_DELAY));
        ESP_LOGI(TAG, "datos recibidos %d", recvbuf);
        memset(recvbuf, 0, 128);


    }

    spi_slave_free(HSPI_HOST);

}