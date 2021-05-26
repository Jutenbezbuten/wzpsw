#include <LPC21xx.H>
#include "timer_interrupts.h"

#define mCOUNTER_ENABLE 0x00000001
#define mCOUNTER_RESET  0x00000002

#define mINTERRUPT_ON_MR0 0x00000001
#define mRESET_ON_MR0     0x00000002
#define mMR0_INTERRUPT    0x00000001

#define VIC_TIMER0_CHANNEL_NR 4
#define VIC_TIMER1_CHANNEL_NR 5

#define mIRQ_SLOT_ENABLE 0x00000020

void (*ptrTimer0InterruptFunction) (void);
void (*ptrTimer1InterruptFunction)();

__irq void Timer0IRQHandler(){

	T0IR=mMR0_INTERRUPT; 	
	ptrTimer0InterruptFunction();
	VICVectAddr=0x00; 	
}

__irq void Timer1IRQHandler(){

	T1IR=mMR0_INTERRUPT; 	// skasowanie flagi przerwania 
	(*ptrTimer1InterruptFunction)();		// cos do roboty
	VICVectAddr=0x00; 	// potwierdzenie wykonania procedury obslugi przerwania
}
/**********************************************/
void Timer0Interrupts_Init(unsigned int uiPeriod, void (*ptrInterruptFunction)(void)){ // microseconds


	VICIntEnable |= (0x1 << VIC_TIMER0_CHANNEL_NR);            
	VICVectCntl0  = mIRQ_SLOT_ENABLE | VIC_TIMER0_CHANNEL_NR;  
	VICVectAddr0  =(unsigned long)Timer0IRQHandler; 	    


	T0MR0 = 15 * uiPeriod;                 	      
	T0MCR |= (mINTERRUPT_ON_MR0 | mRESET_ON_MR0);  


	T0TCR |=  mCOUNTER_ENABLE; 
	
	ptrTimer0InterruptFunction=ptrInterruptFunction;
}

void Timer1Interrupts_Init(unsigned int uiPeriod, void (*ptrInterruptFunction)()){ // microseconds

	ptrTimer1InterruptFunction=(*ptrInterruptFunction);
        // interrupts

	VICIntEnable |= (0x1 << VIC_TIMER1_CHANNEL_NR);            // Enable Timer 1 interrupt channel 
	VICVectCntl1  = mIRQ_SLOT_ENABLE | VIC_TIMER1_CHANNEL_NR;  // Enable Slot 1 and assign it to Timer 1 interrupt channel
	VICVectAddr1  =(unsigned long)Timer1IRQHandler; 	   // Set to Slot 1 Address of Interrupt Service Routine 

        // match module

	T1MR0 = 15 * uiPeriod;                 	      // value 
	T1MCR |= (mINTERRUPT_ON_MR0 | mRESET_ON_MR0); // action 

        // timer

	T1TCR |=  mCOUNTER_ENABLE; // start 

}
