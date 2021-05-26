#include "FreeRTOS.h"
#include "task.h"
#include "led.h"

struct LedParams{
	unsigned char ucBlinkingFreq;
	unsigned char ucLedIndeks;
};

void LedBlink( void *pvParameters ){
	while(1){
		Led_Toggle(((struct LedParams*)pvParameters)->ucLedIndeks);
		vTaskDelay((1000/(((struct LedParams*)pvParameters)->ucBlinkingFreq))/2);
	}
}

void LedCtrl(void *pvParameters){
	unsigned char ucLedCntr=0;
	while(1){
		ucLedCntr++;
		((struct LedParams*)pvParameters)->ucBlinkingFreq=10;
		vTaskDelay(1000);
		((struct LedParams*)pvParameters)->ucBlinkingFreq=5;
		vTaskDelay(1000);
		((struct LedParams*)pvParameters)->ucBlinkingFreq=20;
		((struct LedParams*)pvParameters)->ucLedIndeks=ucLedCntr%4;
		vTaskDelay(1000);
	}
}
int main( void ){
	struct LedParams sLedParams={100,0};
	LedInit();
	xTaskCreate(LedBlink, NULL , 100 , &sLedParams, 2 , NULL );
	xTaskCreate(LedCtrl, NULL , 100 , &sLedParams, 2 , NULL );
	vTaskStartScheduler();
	while(1);
}

