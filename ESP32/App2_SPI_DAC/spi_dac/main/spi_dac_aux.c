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

 #include <stdint.h>
 #include "driver/spi_common.h"
 #include "esp_err.h"
 #include "sdkconfig.h"
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

#define SPIBUS_READ     (0x80)  /*!< addr | SPIBUS_READ  */
#define SPIBUS_WRITE    (0x7F)  /*!< addr & SPIBUS_WRITE */

static const char TAG[] = "main";

void dump(uint8_t *dt, int n)
{
	uint16_t clm = 0;
	uint8_t data;
	uint32_t saddr =0;
	uint32_t eaddr =n-1;

	printf("--------------------------------------------------------\n");
	uint32_t addr;
	for (addr = saddr; addr <= eaddr; addr++) {
		data = dt[addr];
		if (clm == 0) {
			printf("%05x: ",addr);
		}

		printf("%02x ",data);
		clm++;
		if (clm == 16) {
			printf("| \n");
			clm = 0;
		}
	}
	printf("--------------------------------------------------------\n");
}





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


//_-------------------------------------------------
    const char test_str[] = "Hello World!";

    ret = spi_eeprom_write_enable(eeprom_handle);
    //ESP_ERROR_CHECK(ret);
    
    ESP_LOGI(TAG, "Write: %s", test_str);
    for (int i = 0; i < sizeof(test_str); i++) {
        ret = spi_eeprom_write(eeprom_handle, i, test_str[i]);
       // ESP_ERROR_CHECK(ret);
       
    }
   // writeBytes(eeprom_handle, 0, sizeof(test_str), &test_str);
    
    uint8_t test_buf[128];
    memset(test_buf, 0, 128);
    //test_buf[0] = 1;
    //test_buf[1] = 1;
    
    //uint8_t aux[128];
   
    for (int i = 0; i < sizeof(test_str); i++) {
         ret = spi_eeprom_read(eeprom_handle, i, &test_buf[i]);
        //uint8_t *b;
        //b = &test_buf;
        //printf("ola-> %d",(int) b);
       // ESP_ERROR_CHECK(ret);
    }
    for (int i = 0;i<128;i++){
        //uint8_t *b;
        //b = &aux;
        int a = (int)test_buf[i];
        printf(" %d ",a);
    }
    
    printf("\n");
   
   // readBytes(eeprom_handle, 0, sizeof(test_buf), &test_buf);
    //for (int i = 0; i < sizeof(test_str); i++) {
       // printf("%d",test_buf[i]);
       // ESP_ERROR_CHECK(ret);
    //}
    int um = 108;
    printf("1 em char = %c", (char)um);
    printf("\n");
    printf("experiencia %d \n",test_buf[1]);
   
    //printf("ola -> %hhn", test_buf);
    ESP_LOGI(TAG, "Read: %s", test_buf);
    dump(test_buf, 128); //limpar o buffer
    ESP_LOGI(TAG, "Example finished.");
    
//-----------------------------------------------------
   /* uint8_t rbuf[128];
    memset(rbuf, 0, 128);
    len =  eeprom_Read(&dev, 0, rbuf, 128);
    if (len != 128) {
        ESP_LOGE(TAG, "Read Fail");
        while(1) { vTaskDelay(1); }
    }
    ESP_LOGI(TAG, "Read Data: len=%d", len);
    dump(rbuf, 128);*/

    //--------------------------------------------
    /*
    ret = spi_eeprom_write_enable(eeprom_handle);
    ESP_ERROR_CHECK(ret);
*/


    //const char test_str[] = "1";
    //ESP_LOGI(TAG, "Write: %s", test_str);
    /*
    for (int i = 0; i < sizeof(test_str); i++) {
        // No need for this EEPROM to erase before write.
        //do{
            ret = spi_eeprom_write(eeprom_handle, i, 0xFF);
            ESP_ERROR_CHECK(ret);
        //}while(1);
    }
    */

    /*
    write data uart
    uint8_t i = adc_reading;
        uart_write_bytes(UART_NUM_1, &i, 4*5);
        
        printf("UART1 sending to UART2: %d\n", i);
    */

   
   // int c = 1255;

    printf("Writing...\n");
    int i = 0;

    for (i=0; i < 255; i++){
        dac_output_voltage(DAC_CHANNEL_1, i);  //colocar voltagem na saída do canal DAC do GPIO25  
        vTaskDelay(30 / portTICK_PERIOD_MS); //pequeno delay
    }
    dac_output_voltage(DAC_CHANNEL_1, 0);

   // ret = spi_eeprom_write(eeprom_handle, 0, c);
   // ESP_LOGI(TAG, "Write: %d", c);

    


    //ESP_ERROR_CHECK(ret);
   
        //uart_write_bytes(UART_NUM_1, &i, 4*5);
    
    /*UART
    / Read data from the UART
        uint8_t data[128];
        int length = 0;
        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM_2, (size_t*)&length));
        if(length != 0 ) {
            uart_read_bytes(UART_NUM_2, data, length, 20 / portTICK_RATE_MS);
            uint8_t *b;
            b = &data;
            printf("%d - UART2 received from UART1: %d\n", count, (int)*b);
            count++;
        }
    */
    
 /*   uint8_t test_buf[128];
    //char out_data = '';
    for (int i = 0; i < sizeof(test_buf); i++) {
       
        ret = spi_eeprom_read(eeprom_handle, i, &test_buf[i]);
        ESP_ERROR_CHECK(ret);
    } 
    uint8_t *b;
    b = &test_buf;
*/
/*//allocate inside function
		uint8_t test[4];

		//test bad alignment...maybe?
		for (int i = 0; i < sizeof(test); i++) {
			esp_partition_read(partStorage, tamperDataOffset, &test[i], 1);
		}

		printf("Test read %d\n", test[0]);*/

    printf("Reading...\n");
    int j = 0;
    for (j=0; j< 255; j++){
        dac_output_voltage(DAC_CHANNEL_2, j); //colocar voltagem na saída do canal DAC do GPIO25 
        vTaskDelay(30 / portTICK_PERIOD_MS); //pequeno delay
    }
    dac_output_voltage(DAC_CHANNEL_2, 0);

   // ESP_LOGI(TAG, "Read: %d", (int)test_buf[0]);// test_buf);
    //printf("address - %d, value = %d\n", (int )&test_buf[0], (int)*test_buf);
    //ESP_LOGI(TAG, "Example finished.");
    

    while (1) {
        // Add your main loop handling code here.
        vTaskDelay(1);
    }
}
