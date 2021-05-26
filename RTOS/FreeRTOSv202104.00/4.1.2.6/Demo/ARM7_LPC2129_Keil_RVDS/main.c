#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "uart.h"
#include "string.h"
#include "keyboard.h"

xSemaphoreHandle xSemaphore;

void SendString(char cString[]) {
	xSemaphoreTake(xSemaphore, portMAX_DELAY);
	Transmiter_SendString(cString);
	while (Transmiter_GetStatus()!=FREE){};
	xSemaphoreGive(xSemaphore);
}

void LettersTx (void *pvParameters){
	char cMessage[20];
	TickType_t Start, Duration=0;
	while(1){
		CopyString("-ABCDEEFGH-:",cMessage);
		AppendUIntToString(Duration,cMessage);
		AppendString("\n",cMessage);
		Start=xTaskGetTickCount();
		SendString(cMessage);
		Duration=xTaskGetTickCount()-Start;
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
		char cMessage[] = "-KEYBOARD-\n";
	while(1){
		while(RELASED==eKeyboardRead()){};
		SendString(cMessage);
		vTaskDelay(300);
	}
}

int main(void){
	UART_InitWithInt(300);
	vSemaphoreCreateBinary(xSemaphore);
	KeyboardInit();
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 1, NULL );
	vTaskStartScheduler();
	while(1);
}

