#include <LPC21xx.H>
#include "servo.h"
#include "led.h"
#include "timer_interrupts.h"

#define DETECTOR_bm (1<<10)
#define BUTTON3_bm (1<<7)

enum DetectorState {ACTIVE,INACTIVE};
enum ServoState {CALLIB, IDLE, IN_PROGRESS};

struct Servo
{
enum ServoState eState;
unsigned int uiCurrentPosition;
unsigned int uiDesiredPosition;
};

struct Servo sServo;

void DetectorInit(void)	{
	IO0DIR=IO0DIR&(~DETECTOR_bm);
}
	
enum DetectorState eReadDetector(void) {
	if((IO0PIN&BUTTON3_bm)==0) {
		return ACTIVE;
	}
		else {
			return INACTIVE;
		}
}

void ServoCallib(void){
	sServo.eState=CALLIB;
}

void ServoGoTo(unsigned int uiPosition){
	sServo.uiDesiredPosition=uiPosition;
	sServo.eState=IN_PROGRESS;
}

void Automat(void){
  
  switch(sServo.eState){
		case CALLIB: 
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
      if (sServo.uiCurrentPosition==sServo.uiDesiredPosition)
				sServo.eState=IDLE;
      else if (sServo.uiCurrentPosition!=sServo.uiDesiredPosition)
        sServo.eState=IN_PROGRESS;
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
}

void ServoInit(unsigned int uiServoFrequency){
	DetectorInit();
	LedInit();
	Timer0Interrupts_Init(1000000 / uiServoFrequency, &Automat);
	ServoCallib();
}
