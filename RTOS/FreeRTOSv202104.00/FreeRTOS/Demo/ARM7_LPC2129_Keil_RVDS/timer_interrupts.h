__irq void Timer0IRQHandler(void);
void Timer0Interrupts_Init(unsigned int uiPeriod, void (*ptrInterruptFunction)(void));
void Timer1Interrupts_Init(unsigned int uiPeriod, void (*ptrInterruptFunction)());
