/*
 * adc.h
 *
 * Created: 03.02.2023 19:01:11
 *  Author: User
 */ 
#ifndef ADC_HAL_H_
#define ADC_HAL_H_

//#include <stdint.h>
#include "../config.h"
//#include <avr/io.h>
//#include <util/delay.h>
#include <avr/interrupt.h>


#endif /* ADC_HAL_H_ */

#define ADC_VOLT(x) (x * 1.188440)
#define REFS_1_1V	1
#define REFS_AVCC	0

#define CONTINUOUS_CONV	0
#define SINGLE_CONV		1

#define MUX				12
#define PGA				9
#define MODE			8
#define DR				5
#define COMP_MODE		4
#define COMP_POL		3
#define COMP_LAT		2
#define COMP_QUE		0




enum{
	AIN0AIN1=0,
	AIN0AIN3,
	AIN1AIN3,
	AIN2AIN3,
	AIN0GND,
	AIN1GND,
	AIN2GND,
	AIN3GND
};
enum{
	FSR_6_144 = 0,
	FSR_4_096,
	FSR_2_048,
	FSR_1_024,
	FSR_0_512,
	FSR_0_256 = 5,
};
enum{
	SPS_8=0,
	SPS_16,
	SPS_32,
	SPS_64,
	SPS_128,
	SPS_250,
	SPS_475,
	SPS_860,
};


enum{
	ADC0_PIN,
	ADC1_PIN,
	ADC2_PIN,
	ADC3_PIN,
	ADC4_PIN,
	ADC5_PIN,
	ADC6_PIN,
	ADC7_PIN,
	ADC8_TEMPERATURE,
	ADC_1V1 = 0b1110,
	ADC_GND,
};


void adc_init(void);
void adc_pin_enable(uint8_t pin);
void adc_pin_disable(uint8_t pin);
void adc_pin_select(uint8_t souce);
uint16_t adc_convert(void);
uint16_t get_adc(uint8_t source);
uint16_t get_mVolt(uint8_t source);
uint16_t getExtAdc(uint8_t devAddr, uint8_t ch, uint8_t pga);