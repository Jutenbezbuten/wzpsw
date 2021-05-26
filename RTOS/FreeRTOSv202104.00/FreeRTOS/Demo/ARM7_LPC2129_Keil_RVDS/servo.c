#include <LPC21xx.H>
#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define DETECTOR_bm (1<<10)
#define BUTTON3_bm (1<<7)


QueueHandle_t QueueServo;
unsigned int uiDefaultServoPeriod;

struct Servo{
	enum ServoState eState;
	unsigned int uiCurrentPosition;
	unsigned int uiDesiredPosition;
};

struct Servo sServo;

typedef struct {
	unsigned int uiDesiredPosition;
	unsigned int uiWaitTime;
	unsigned int uiStepTime;
}ServoCommand;

void DetectorInit(void)	{
	IO0DIR=IO0DIR&(~DETECTOR_bm);
}
	
enum DetectorState eReadDetector(void) {
	if((IO0PIN&(DETECTOR_bm))==0) {
		return ACTIVE;
	}
		else {
			return INACTIVE;
		}
}


void Automat(void *pvParameters){
  ServoCommand sServoCommand;

	while (1){
		switch(sServo.eState){
			case CALLIB: 
				sServoCommand.uiStepTime = uiDefaultServoPeriod;
				if (eReadDetector()==INACTIVE){
					Led_StepLeft();
					sServo.eState = CALLIB;
				}
				else{
					sServo.uiCurrentPosition=0;
					sServo.uiDesiredPosition=0;
					sServo.eState=IDLE;
				}
				break;
			case IDLE:
				if (sServo.uiCurrentPosition!=sServo.uiDesiredPosition)
					sServo.eState=IN_PROGRESS;
				else {
					if  (xQueueReceive(QueueServo, &sServoCommand, portMAX_DELAY) == pdTRUE){
					sServo.uiDesiredPosition = sServoCommand.uiDesiredPosition;
					vTaskDelay(sServoCommand.uiWaitTime);
					}
					else {
					sServoCommand.uiStepTime = uiDefaultServoPeriod;
					sServo.eState=IDLE;
				}
				}
				break;
			case IN_PROGRESS:
				if (sServo.uiCurrentPosition<sServo.uiDesiredPosition){
					Led_StepRight();
					sServo.uiCurrentPosition++;
					sServo.eState = IN_PROGRESS;
				}
				else if(sServo.uiCurrentPosition>sServo.uiDesiredPosition){
					Led_StepLeft();
					sServo.uiCurrentPosition--;
					sServo.eState = IN_PROGRESS;
				}
				else if (sServo.uiCurrentPosition==sServo.uiDesiredPosition)
					sServo.eState=IDLE;
				break;
			default:
				break;
		}
		vTaskDelay(sServoCommand.uiStepTime);
	}
}

void ServoInit(unsigned int uiServoFrequency){
	uiDefaultServoPeriod = (configTICK_RATE_HZ/uiServoFrequency);
	QueueServo=xQueueCreate(20, sizeof(ServoCommand));
	DetectorInit();
	LedInit();
	sServo.eState = CALLIB;
	xTaskCreate(Automat, NULL, 128, NULL, 2, NULL);
}



void ServoGoTo(unsigned int uiPosition){
	ServoCommand sServoCommand;
	sServoCommand.uiWaitTime = 0;
	sServoCommand.uiDesiredPosition = uiPosition;
	sServoCommand.uiStepTime = uiDefaultServoPeriod;
	xQueueSend(QueueServo, &sServoCommand, portMAX_DELAY);
}

void ServoCallib(void){
	sServo.eState = CALLIB;
}

void Servo_Wait(unsigned int uiWaitTime)
{
	ServoCommand sServoCommand;
	sServoCommand.uiWaitTime = uiWaitTime;
	sServoCommand.uiStepTime = uiDefaultServoPeriod;
	xQueueSend(QueueServo, &sServoCommand, portMAX_DELAY);
}

void Servo_Speed(unsigned int uiPosition, unsigned int uiStepTime)
{
	ServoCommand sServoCommand;
	sServoCommand.uiWaitTime = 0;
	sServoCommand.uiDesiredPosition = uiPosition;
	sServoCommand.uiStepTime = uiStepTime;
	
	xQueueSend(QueueServo, &sServoCommand, portMAX_DELAY);
}
