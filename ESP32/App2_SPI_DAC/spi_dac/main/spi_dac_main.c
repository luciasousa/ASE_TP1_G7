#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "spi_eepromMicrochip.h"

#include "esp_event.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "driver/dac_common.h"
#include "esp_system.h"


/*
About this application 
1. Initialize DAC and SPI
2. If write to memory, LED (white) from DAC_CHANNEL_1, GPIO 25 is turned on
3. Print the value that is written
4. If read from memory, LED (red) from DAC_CHANNEL_2, GPIO26 is turned on
5. Print the value that is read 
*/

#ifdef CONFIG_IDF_TARGET_ESP32
#   define EEPROM_HOST    VSPI_HOST
#   define PIN_NUM_MISO 19
#   define PIN_NUM_MOSI 23
#   define PIN_NUM_CLK  18
#   define PIN_NUM_CS   13
#endif

static const char TAG[] = "main";


void app_main(void) {

    //initialize DAC
    dac_output_enable(DAC_CHANNEL_1); //ativar a saida da DAC no GPIO25
    dac_output_enable(DAC_CHANNEL_2); //ativar a saida da DAC no GPIO26

    esp_err_t ret;

#ifndef CONFIG_EXAMPLE_USE_SPI1_PINS
    ESP_LOGI(TAG, "Initializing bus SPI%d...", EEPROM_HOST+1);
    
    spi_bus_config_t buscfg={
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    
    //Initialize the SPI bus
    ret = spi_bus_initialize(EEPROM_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

#else
    ESP_LOGI(TAG, "Attach to main flash bus...");
#endif

    eeprom_config_t eeprom_config = {
        .cs_io = PIN_NUM_CS,
        .host = EEPROM_HOST,
        .miso_io = PIN_NUM_MISO,
    };

#ifdef CONFIG_EXAMPLE_INTR_USED
    eeprom_config.intr_used = true;
    gpio_install_isr_service(0);
#endif

    eeprom_handle_t eeprom_handle;

    ESP_LOGI(TAG, "Initializing device...");
    ret = spi_eeprom_init(&eeprom_config, &eeprom_handle);
    ESP_ERROR_CHECK(ret);

    ret = spi_eeprom_write_enable(eeprom_handle);
    ESP_ERROR_CHECK(ret);

    int c = 1255;

    printf("Writing...\n");
    int i = 0;

    for (i=0; i < 255; i++){
        dac_output_voltage(DAC_CHANNEL_1, i);  //colocar voltagem na saída do canal DAC do GPIO25  
        vTaskDelay(30 / portTICK_PERIOD_MS); //pequeno delay
    }
    dac_output_voltage(DAC_CHANNEL_1, 0);

    ret = spi_eeprom_write(eeprom_handle, 0, c);
    ESP_LOGI(TAG, "Write: %d", c);
    
    uint8_t test_buf[128];
    
    for (int i = 0; i < sizeof(test_buf); i++) {
       
        ret = spi_eeprom_read(eeprom_handle, i, &test_buf[i]);
        ESP_ERROR_CHECK(ret);
    } 
    uint8_t *b;
    b = &test_buf;

    printf("Reading...\n");
    int j = 0;
    for (j=0; j< 255; j++){
        dac_output_voltage(DAC_CHANNEL_2, j); //colocar voltagem na saída do canal DAC do GPIO25 
        vTaskDelay(30 / portTICK_PERIOD_MS); //pequeno delay
    }
    dac_output_voltage(DAC_CHANNEL_2, 0);

    ESP_LOGI(TAG, "Read: %d", (int)test_buf[0]);
    printf("address - %d, value = %d\n", (int )&test_buf[0], (int)*test_buf);
    ESP_LOGI(TAG, "Example finished.");
    

    while (1) {
        vTaskDelay(1);
    }
}
