#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_timer.h"


#define GPIO_MOSI 11
#define GPIO_MISO 13
#define GPIO_SCLK 12
#define GPIO_CS 10
#define SENDER_HOST SPI2_HOST

void app_main(void)
{
    esp_err_t ret;
    spi_device_handle_t handle;

    spi_bus_config_t buscfg = {
        .mosi_io_num	= GPIO_MOSI,
        .miso_io_num	= GPIO_MISO,
        .sclk_io_num	= GPIO_SCLK,
        .quadwp_io_num	= -1,
        .quadhd_io_num	= -1
    };

    spi_device_interface_config_t devcfg = {
        .command_bits		= 0,
        .address_bits		= 0,
        .dummy_bits			= 0,
        .clock_speed_hz		= 21000000,
        .duty_cycle_pos		= 128,
        .mode				= 0,
        .spics_io_num		= GPIO_CS,
        .cs_ena_posttrans	= 3,
        .queue_size			= 3
    };

    int n = 0;
    char sendbuf[9] = {0};
    char recvbuf[9] = {0};
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));


    ret = spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);
    ret = spi_bus_add_device(SENDER_HOST, &devcfg, &handle);
    assert(ret == ESP_OK);


    while(1)
    {
    	memset(sendbuf, 0x03, 8);
    	memset(recvbuf, 0,    8);

        t.length	= 8;
        t.tx_buffer	= sendbuf;
        t.rx_buffer	= recvbuf;
        ret = spi_device_transmit(handle, &t);
        printf("Received: %02X\n", *(uint8_t *)recvbuf);
        n++;

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    ret = spi_bus_remove_device(handle);
    assert(ret == ESP_OK);
}
