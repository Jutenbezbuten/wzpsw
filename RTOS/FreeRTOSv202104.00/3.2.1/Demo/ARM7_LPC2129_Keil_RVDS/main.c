#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

void LedBlink( void *pvParameters ){
	while(1){
		Led_Toggle(0);
		vTaskDelay((1000/(*((unsigned char*)pvParameters)))/2);
	}
}

void LedCtrl(void *pvParameters){
	while(1){
		vTaskSuspend(*((TaskHandle_t*)pvParameters));
		vTaskDelay(1000);
		vTaskResume(*((TaskHandle_t*)pvParameters));
		vTaskDelay(1000);
	}
}
int main( void ){
	TaskHandle_t LedBlinkHandle;
	unsigned char ucBlinkingFreq = 10;
	LedInit();
	xTaskCreate(LedBlink, NULL , 100 , &ucBlinkingFreq, 2 , &LedBlinkHandle );
	xTaskCreate(LedCtrl, NULL , 100 , &LedBlinkHandle, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

