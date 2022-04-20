#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "eeprom.h"

#include "driver/adc.h"
#include "driver/dac.h"
#include "driver/dac_common.h"

static const char *TAG = "MAIN";

/*
Application 2 - SPI and DAC
1. Initialize DAC and SPI
2. If write to memory, LED (white) from DAC_CHANNEL_1, GPIO 25 is turned on
3. Print the value that is to be written and what is in memory before write
4. If read from memory, LED (red) from DAC_CHANNEL_2, GPIO26 is turned on
5. Print the value that is read and what is in memory after write
*/

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


void app_main(void)
{
     //initialize DAC
    dac_output_enable(DAC_CHANNEL_1); //ativar a saida da DAC no GPIO25
    dac_output_enable(DAC_CHANNEL_2); //ativar a saida da DAC no GPIO26


	EEPROM_t dev;
	spi_master_init(&dev);
	int32_t totalBytes = eeprom_TotalBytes(&dev);
	ESP_LOGI(TAG, "totalBytes=%d Bytes",totalBytes);

	// Get Status Register
	uint8_t reg;
	esp_err_t ret;
	ret = eeprom_ReadStatusReg(&dev, &reg);
	if (ret != ESP_OK) {
		ESP_LOGI(TAG, "ReadStatusReg Fail %d",ret);
		while(1) { vTaskDelay(1); }
	} 
	ESP_LOGI(TAG, "readStatusReg : 0x%02x", reg);

	uint8_t wdata[128];
	int len;
	
    unsigned char test_str[] = "Hello";
    printf("Writing...\n");
    printf("data to write: ");
    for (int i=0; i<sizeof(test_str); i++) {
        uint8_t data =  test_str[i];
        printf("%02X ", (char) test_str[i]);
        wdata[i]= data;
    }
    printf("\n");
    for (int i=0; i < 255; i++){
        dac_output_voltage(DAC_CHANNEL_1, i);  //colocar voltagem na saída do canal DAC do GPIO25  
        vTaskDelay(30 / portTICK_PERIOD_MS); //pequeno delay
    }
    dac_output_voltage(DAC_CHANNEL_1, 0);
    
    for (int addr=0; addr<sizeof(test_str);addr++) {
        len =  eeprom_WriteByte(&dev, addr, wdata[addr]);
        if (len != 1) {
            ESP_LOGI(TAG, "WriteByte Fail addr=%d", addr);
            while(1) { vTaskDelay(1); }
        }
    }

    // Read 128 byte from Address=0
    uint8_t rbuf[128];
    memset(rbuf, 0, 128);
    dump(rbuf, 128); //rbuf antes de se ler da memoria = tudo a 0
    len =  eeprom_Read(&dev, 0, rbuf, sizeof(test_str));
    if (len != sizeof(test_str)) {
        ESP_LOGI(TAG, "Read Fail");
        while(1) { vTaskDelay(1); }
    }
    //dump(rbuf, 128);
    printf("Reading...\n");
    int j = 0;
    for (j=0; j< 255; j++){
        dac_output_voltage(DAC_CHANNEL_2, j); //colocar voltagem na saída do canal DAC do GPIO25 
        vTaskDelay(30 / portTICK_PERIOD_MS); //pequeno delay
    }
    dac_output_voltage(DAC_CHANNEL_2, 0);
    dump(rbuf, 128); //rbuf depois de se ler da memoria -> caracteres escritos em hexadecimal
    printf("Word read from memory: ");
    for(int i=0; i < sizeof(test_str); i++){
        printf("%c",  rbuf[i]);
    }
    printf("\n");
    ESP_LOGI(TAG, "Example finished.");
}
