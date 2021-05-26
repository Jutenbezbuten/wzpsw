#include <asf.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)


int main (void) {
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	int iCounter=0;
	stdio_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);
	
	while(1) {
		printf("%i \r",iCounter);
		iCounter++;		
	}
}
