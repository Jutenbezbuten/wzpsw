#include <asf.h>
#include <string.h>

#define UART_TXPIN IOPORT_CREATE_PIN(PORTE, 3)

#define MY_ADC    ADCA
#define MY_ADC_CH ADC_CH0

#define GAIN 64
#define SAMPLES_PER_MEASUREMENT 2048
#define BUFFER_SIZE 16
#define REF_VOLTAGE_mV 1000
#define RESOLUTION 2048
#define MULTIPLIER (REF_VOLTAGE_mV/(((float)RESOLUTION)*GAIN))
#define OFFSET 0.33278f
#define MG_MULTIPLIER 249.4387f
static void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf;
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_ON, ADC_RES_12,
	ADC_REF_BANDGAP);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 500000UL);
	adcch_set_input(&adcch_conf, ADCCH_POS_PIN1, ADCCH_NEG_PIN5, GAIN);
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH, &adcch_conf);
}

float MeanOversampled(void);
char *FloatToString(float fInputValue);
float VoltToG(float fVoltage);

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
	float fWeigh;
	
	while(1) {
		fResult=0;
		fConverted=0;
		fWeigh=0;
		char cRequest;
		scanf("%c",&cRequest);

		fConverted= MeanOversampled();
		fResult=(fConverted*MULTIPLIER);
		fWeigh=VoltToG(fResult);
		printf("%s\r",FloatToString(fWeigh));
	}
}

float MeanOversampled(void) {
	float fOversampleSum=0;
	
	for (float fSampleCounter=0; fSampleCounter<SAMPLES_PER_MEASUREMENT; fSampleCounter++){
		adc_start_conversion(&MY_ADC, MY_ADC_CH);
		adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH);
		fOversampleSum=fOversampleSum+(float)adc_get_signed_result(&MY_ADC, MY_ADC_CH);
	}
	return (fOversampleSum/((float)SAMPLES_PER_MEASUREMENT));
}

char *FloatToString(float fInputValue){
	static char scResult[BUFFER_SIZE];
	sprintf(scResult,"%f",fInputValue);
	*strchr(scResult, '.') = ',';
	return scResult;
}

float VoltToG(float fVoltage){
	float fWeight=(fVoltage+OFFSET);
	fWeight=(fWeight*MG_MULTIPLIER);
	return fWeight;
}

//GAIN 64 SAMPLES 2048
//ciezarek 1.1 kg: 4.003
//bez obciazenia: -0.336
//ciezarek 122g: 0.16

//GAIN 1 SAMPLES 16
//ciezarek 1.1 kg: -1.14
//bez obciazenia: -5.75
//ciezarek 122g: -5.23

//SPM:1
//GAIN 1   N 0.71  S  4.57
//GAIN 8   N 0.49  S  4.37
//GAIN 16  N 0.36  S  4.44
//GAIN 64  N 0.29  S  4.32

//GAIN:64
//SPM 1      N 0.29   S 4.32
//SPM 2      N 0.22   S 4.31
//SPM 4      N 0.15   S 4.36
//SPM 8      N 0.10   S 4.34
//SPM 16     N 0.08   S 4.36
//SPM 256    N 0.02   S 4.34
//SPM 1024   N 0.01   S 4.34
//SPM 2048   N 0.007  S 4.33