#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"

xSemaphoreHandle xSemaphore;

void PulseTrigger( void *pvParameters ){
	while(1){
		vTaskDelay(1000);
		xSemaphoreGive(xSemaphore);
	}
}

void Pulse_LED0(void *pvParameters){
	while(1){
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		Led_Set(0);
		vTaskDelay(100);
		Led_Clear(0);
	}
}

void PulseTriggerThird( void *pvParameters ){
	vTaskDelay(333);
	while(1){
		vTaskDelay(333);
		xSemaphoreGive(xSemaphore);
	}
}

void Pulse_LED1(void *pvParameters){
	while(1){
		xSemaphoreTake(xSemaphore,portMAX_DELAY);
		Led_Set(1);
		vTaskDelay(100);
		Led_Clear(1);
	}
}

int main( void ){
	vSemaphoreCreateBinary(xSemaphore);
	LedInit();
	xTaskCreate(PulseTrigger, NULL , 100 , NULL, 2 , NULL );
	xTaskCreate(Pulse_LED0, NULL , 100 , NULL, 2 , NULL );
	xTaskCreate(PulseTriggerThird, NULL , 100 , NULL, 2 , NULL );
	xTaskCreate(Pulse_LED1, NULL , 100 , NULL, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

