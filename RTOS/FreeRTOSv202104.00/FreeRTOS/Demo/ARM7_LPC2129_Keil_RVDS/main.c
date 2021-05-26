#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "semphr.h"
#include "keyboard.h"
#include "queue.h"
#include "servo.h"

void Keyboard (void *pvParameters){
	enum KeyboardState ePreviousState, eCurrentState = RELASED;
	while(1){
		ePreviousState = eCurrentState;
		eCurrentState = eKeyboardRead();
		if (eCurrentState != ePreviousState){
			switch(eCurrentState){
				case BUTTON_0: 
					ServoCallib();
					break;
				case BUTTON_1: 
					Servo_Speed(12,125);
					Servo_Speed(24,250);
					Servo_Speed(36,500);
					Servo_Speed(0,1000);
					break;
				case BUTTON_2:
					ServoGoTo(12);
					Servo_Wait(100);
					ServoGoTo(0);
					ServoGoTo(24);
					Servo_Wait(200);
					ServoGoTo(0);
					ServoGoTo(36);
					Servo_Wait(300);
					ServoGoTo(0);
					break;
				case BUTTON_3: 
					ServoGoTo(12);
					ServoGoTo(0);
					ServoGoTo(24);
					ServoGoTo(0);
					ServoGoTo(36);
					ServoGoTo(0);
					break;
				default:
					break;
			}
	}
		vTaskDelay(100);
	}
}


int main(void){
	KeyboardInit();
	ServoInit(100);
	xTaskCreate(Keyboard, NULL, 128, NULL, 2, NULL);
	vTaskStartScheduler();
	while(1);
}
