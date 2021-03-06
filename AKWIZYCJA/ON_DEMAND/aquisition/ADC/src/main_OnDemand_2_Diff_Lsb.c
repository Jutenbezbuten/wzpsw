#include <asf.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)

#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

#define OVERSAMPLING_RATIO 16
static void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
	ADC_REF_BANDGAP);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN5, 8);
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

uint16_t MeanOversampled(void);

int main (void) {
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = 9600,
		.charlength = USART_CHSIZE_8BIT_gc,
		.paritytype = USART_PMODE_DISABLED_gc,
		.stopbits = false
	};
	
	sysclk_init();
	adc_init();
	
	stdio_serial_init(&USARTE0, &USART_SERIAL_OPTIONS);
	ioport_set_pin_dir(UART_TXPIN, IOPORT_DIR_OUTPUT);
	
	adc_enable(&MY_ADC);
	
	uint16_t uiResult;
	
	while(1) {
		uiResult=0;
		char cRequest;
		scanf("%c",&cRequest);

		uiResult = MeanOversampled();
		printf("%d \r",uiResult);	
	}
}

uint16_t MeanOversampled(void) {
	uint16_t uiOversampleSum=0;
	
	for (unsigned char ucSampleCounter=0; ucSampleCounter<OVERSAMPLING_RATIO; ucSampleCounter++){
		adc_start_conversion(&MY_ADC, MY_ADC_CH);
		adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
		uiOversampleSum=uiOversampleSum+adc_get_result(&MY_ADC, MY_ADC_CH);
	}
	return (uint16_t) (uiOversampleSum/OVERSAMPLING_RATIO);
}

//single ended: 1200-1300
//differential: 0-200
//max wzmocnienie 8
