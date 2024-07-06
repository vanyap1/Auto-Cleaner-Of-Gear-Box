/*
 * adc.c
 *
 * Created: 03.02.2023 19:01:00
 *  Author: User
 */ 
#include "adc_hal.h"
#include "twi_hal1.h"
#include <util/delay.h>

volatile static uint8_t adc_convert_done = 1;

ISR(ADC_vect){
	adc_convert_done = 1;
}

void adc_init(void){
	ADMUX |= REFS_1_1V << REFS0 | 1 << REFS1;
	ADCSRA |= 1 << ADEN | 1 << ADIE | 0b111 << ADPS0;
}


void adc_pin_enable(uint8_t pin){
	DIDR0 |= 1 << pin;
}

void adc_pin_disable(uint8_t pin){
	DIDR0 &= ~(1 << pin);
}

void adc_pin_select(uint8_t souce){
	ADMUX &= 0xF0;
	ADMUX |= souce;
}


uint16_t adc_convert(void){
	ADCSRA |= 1 << ADEN;
	uint8_t adcl = 0;
	uint8_t adch = 0;
	
	adc_convert_done = 0;
	
	ADCSRA |= 1 << ADSC;
	while(adc_convert_done == 0);
	
	adcl = ADCL;
	adch = ADCH;
	ADCSRA &=~ (1 << ADEN);
	return (adch << 8 | adcl);
	
	
}

uint16_t get_adc(uint8_t source){
	adc_pin_select(source);
	//uint16_t adc_tmp_data = adc_convert();
	return adc_convert();
}

uint16_t get_mVolt(uint8_t source){
	adc_pin_select(source);
	//uint16_t adc_tmp_data = adc_convert();
	return ADC_VOLT(adc_convert());
}

void setExtAdcCh(uint8_t devAddr, uint8_t ch, uint8_t pgaMode){
	
}

uint16_t getExtAdc(uint8_t devAddr, uint8_t ch, uint8_t pgaMode){
	uint16_t configReg = 0x8000;
	configReg |= (SPS_8 << DR) | (SINGLE_CONV << MODE) | (pgaMode << PGA) | (ch << MUX);
	
	//configReg =0b1000111110000001;
	uint8_t configRegRaw[2] = {0, 0};
	uint8_t returnRegs[2];
	
	configRegRaw[1] = configReg & 0xFF;
	configRegRaw[0] = (configReg >> 8) & 0xFF;
	
	twi1_write(devAddr,1,configRegRaw,sizeof(configRegRaw));
	_delay_ms(200);
	twi1_read(devAddr,0, returnRegs, sizeof(returnRegs));
	return (returnRegs[0] << 8) + returnRegs[1];
}

