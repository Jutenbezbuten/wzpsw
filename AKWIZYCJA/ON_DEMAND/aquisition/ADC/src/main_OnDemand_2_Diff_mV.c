#include <asf.h>
#include <string.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)

#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

#define GAIN 1
#define SAMPLES_PER_MEASUREMENT 16
#define BUFFER_SIZE 16
#define REF_VOLTAGE_mV 1000
#define RESOLUTION 2048
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
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN5, GAIN);
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

float MeanOversampled(void);
char *FloatToString(float fInputValue);

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
	
	float fConverted;
	float fResult;
	
	while(1) {
		fResult=0;
		fConverted=0;
		char cRequest;
		scanf("%c",&cRequest);

		fConverted= MeanOversampled();
		fResult=(fConverted*REF_VOLTAGE_mV)/(GAIN*RESOLUTION);
		printf("%s\r",FloatToString(fResult));
	}
}

float MeanOversampled(void) {
	float fOversampleSum=0;
	
	for (unsigned char ucSampleCounter=0; ucSampleCounter<SAMPLES_PER_MEASUREMENT; ucSampleCounter++){
		adc_start_conversion(&MY_ADC, MY_ADC_CH);
		adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
		fOversampleSum=fOversampleSum+(float)adc_get_result(&MY_ADC, MY_ADC_CH);
	}
	return (fOversampleSum/SAMPLES_PER_MEASUREMENT);
}

char *FloatToString(float fInputValue){
	static char scResult[BUFFER_SIZE];
	sprintf(scResult,"%f",fInputValue);
	*strchr(scResult, '.') = ',';
	return scResult;
	}

//zmierzone napiecie: 159 mV
//wartosc LSB: 313.69
//rozdzielczosc: 313.69/0.159=1973
//teoretycznie: 2048 (poniewaz SIGN_ON)
//powinno byc: 153 mV
//po konwersji na mV: 153.23 mV