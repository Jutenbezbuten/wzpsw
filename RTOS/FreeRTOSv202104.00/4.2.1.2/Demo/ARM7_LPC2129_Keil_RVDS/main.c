#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "uart.h"
#include "string.h"
#include "keyboard.h"
#include "queue.h"

QueueHandle_t QueueUART;

void SendString(void *pvParameters) {
	char cString[20];
	while(1){
		xQueueReceive(QueueUART, &cString, 0);
		Transmiter_SendString(cString);
		while (Transmiter_GetStatus()!=FREE){};
	}
}

void LettersTx (void *pvParameters){
	char cMessage[20];
	TickType_t Start, Duration=0;
	while(1){
		CopyString("-ABCDEEFGH-:",cMessage);
		AppendUIntToString(Duration,cMessage);
		AppendString("\n",cMessage);
		Start=xTaskGetTickCount();
		if (errQUEUE_FULL==xQueueSend(QueueUART,&cMessage,0)){
			Led_Toggle(0);
		}
		Duration=xTaskGetTickCount()-Start;
		vTaskDelay(300);
	}
}

void KeyboardTx (void *pvParameters){
		char cMessageKeyboard[] = "-KEYBOARD-\n";
	while(1){
		while(RELASED==eKeyboardRead()){};
		xQueueSend(QueueUART,&cMessageKeyboard,0);
		vTaskDelay(300);
	}
}

int main(void){
	UART_InitWithInt(300);
	QueueUART=xQueueCreate(5,20);
	KeyboardInit();
	LedInit();
	xTaskCreate(LettersTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(KeyboardTx, NULL, 128, NULL, 1, NULL );
	xTaskCreate(SendString,NULL, 128, NULL, 1, NULL);
	vTaskStartScheduler();
	while(1);
}
