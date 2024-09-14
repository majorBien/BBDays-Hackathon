/*
 * gpio.c
 *
 *  Created on: 14 wrz 2024
 *      Author: majorbien
 */
 #include "gpio.h"
 #include "tasks_settings.h"
 #include "freertos/FreeRTOS.h"
 
bool openFirstAidKit;

void gpioTask(void *pvParameters)
{
	while(1)
	{
		gpioControl();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}

void gpioConfig(void)
{
    esp_rom_gpio_pad_select_gpio(LOCK);

    gpio_set_direction(LOCK, GPIO_MODE_OUTPUT);
}


void gpioControl(void)
{
	if(openFirstAidKit==0)
	{
		gpio_set_level(LOCK, 0);	
	}
	else
	{
		gpio_set_level(LOCK, 1);
	}
		
}

void gpioTaskStart(void)
{
	xTaskCreatePinnedToCore(&gpioTask,"gpio", GPIO_STACK_SIZE, NULL, GPIO_PRIORITY, NULL, GPIO_CORE_ID);
}


	
