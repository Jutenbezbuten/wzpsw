typedef enum DetectorState{
	ACTIVE,
	INACTIVE
} DetectorState;

typedef enum ServoState{
CALLIB, 
IDLE,
IN_PROGRESS
} ServoState;

void ServoInit(unsigned int uiServoFrequency);
void ServoCallib(void);
void ServoGoTo(unsigned int uiPosition);
void DetectorInit(void);
void Servo_Run(void *pvPeroid);
void Automat(void *pvParameters);
void Servo_Wait(unsigned int uiWaitTime);
void Servo_Speed(unsigned int uiPosition, unsigned int uiStepTime);
