#include "config.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "string.h"
#include "stdbool.h"
#include "uart_hal.h"
#include "gpio_driver.h"
#include "twi_hal1.h"
#include "spi1_hall.h"
#include "stdint.h"
#include "adc_hal.h"
#include "eeprom.h"

#include <stdio.h>
static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);


//Leds definition section
#define RED				0
#define GREEN			1
#define BLUE			2
#define  ADC_ADDR		0x48 //0x90

#define L_TANK			0
#define R_TANK			1

#define  mainScreen		0
#define  setupScreen	1
uint8_t currentScr = 0;


int16_t lastAdcValueLeft = 0;
int16_t lastAdcValueRight = 0;
bool screenPart;


int16_t adcValLeft;
int16_t adcValRight;
bool valve1 = false;
bool valve2 = false;
bool pump = false;



gpio dev1[2] = {{(uint8_t *)&PORTC , PORTC0} , {(uint8_t *)&PORTE , PORTE3}};
gpio dev2[3] = {{(uint8_t *)&PORTC , PORTC2} , {(uint8_t *)&PORTC , PORTC1}};
gpio dev3[3] = {{(uint8_t *)&PORTC , PORTC5} , {(uint8_t *)&PORTC , PORTC4},{(uint8_t *)&PORTC , PORTC3}};

uint8_t menuPointer = 0;

char measBuffer[16] = "\0";
char scrLineBuff[16] = "\0";

char tankLeft[8];
char tankRight[8];



CallibrationValues callValues = {0, 0};

//uint8_t initData[16];
int main(void)
{
    //char char_array[128]="test_data\0";
    twi1_init(400000);
    
    
    gpio_set_pin_direction(&dev1[RED] , PORT_DIR_OUT); gpio_set_pin_level(&dev1[RED], true);
    gpio_set_pin_direction(&dev1[GREEN] , PORT_DIR_OUT); gpio_set_pin_level(&dev1[GREEN], true);
    
    gpio_set_pin_direction(&dev2[RED] , PORT_DIR_OUT); gpio_set_pin_level(&dev2[RED], true);
    gpio_set_pin_direction(&dev2[GREEN] , PORT_DIR_OUT); gpio_set_pin_level(&dev2[GREEN], true);
    
    gpio_set_pin_direction(&dev3[RED] , PORT_DIR_OUT); gpio_set_pin_level(&dev3[RED], true);
    gpio_set_pin_direction(&dev3[GREEN] , PORT_DIR_OUT); gpio_set_pin_level(&dev3[GREEN], true);
    gpio_set_pin_direction(&dev3[BLUE] , PORT_DIR_OUT); gpio_set_pin_level(&dev3[BLUE], true);
	
	EEPROM_read_batch(0, &callValues, sizeof(callValues));
	
	
	sei();
	stdout = &mystdout;
	_delay_ms(500);
	screenInit();
	screenPrintString(75, 0x03, (uint8_t *)"SW ver 0.1a", 0);

	while (1) 
    {
		gpio_toggle_pin_level(&dev3[BLUE]);
		
		if(serial_complete){
			uint8_t key = getKey();
			if(key == 0x00){
				//gpio_toggle_pin_level(&dev1[RED]);	
			}
			if(key == 0x01){
				//gpio_toggle_pin_level(&dev1[GREEN]);
				if(currentScr == mainScreen){
					if(menuPointer < 3){
						menuPointer++;
					}
				}
				if(currentScr == setupScreen){
					if(menuPointer < 5){
						menuPointer++;
					}
				}
				
			}
			if(key == 0x02){
				//gpio_toggle_pin_level(&dev2[RED]);
				if(menuPointer != 0){
					menuPointer--;
				}
			}
			if(key == 0x03){
				//gpio_toggle_pin_level(&dev2[GREEN]);
				
				if(currentScr == mainScreen){
					if(menuPointer == 3){
						currentScr = setupScreen;
						menuPointer = 0;
						valve1 = false;
						valve2 = false;
						pump = false;
					}
					
				}
				
				if(currentScr == setupScreen){
					
					if(menuPointer == 1){
						valve1 =! valve1;
					}
					
					if(menuPointer == 2){
						valve2 =! valve2;
					}
					
					if(menuPointer == 3){
						pump =! pump;
					}
					
					if(menuPointer == 4){
						
						callValues.LTank = lastAdcValueLeft;
						callValues.RTank = lastAdcValueRight;
						EEPROM_update_batch(0, &callValues, sizeof(callValues));
					}
					
					
					if(menuPointer == 5){
						currentScr = mainScreen;
						menuPointer = 0;
						valve1 = false;
						valve2 = false;
						pump = false;
					}
				}
				
				
				
				
				
				
				
				
			}			
		}
		
		
		
		
		//sprintf(measBuffer, "Val=%05d", lastAdcValue - correctionFactor);  //print hex 0x%04X
		
		
		
		
		
	
		int16_t integerPart = adcValLeft / 10;
		int16_t decimalPart = adcValLeft % 10;
		sprintf(tankLeft, "%02d.%dL", integerPart, decimalPart);
		sprintf(measBuffer, "%d.%d", integerPart, decimalPart);
		
		
		integerPart = adcValRight / 10;
		decimalPart = adcValRight % 10;		
		sprintf(tankRight, "%02d.%dL", integerPart, decimalPart);
		
		
		
		
		screenPart =! screenPart;
		if(screenPart){
			lastAdcValueLeft = getExtAdc(ADC_ADDR, AIN0AIN1, FSR_0_256);
			adcValLeft = lastAdcValueLeft-callValues.LTank;
			adcValLeft = (adcValLeft <= 0) ? 0 : adcValLeft;
			screenPrintString(3, 6, (uint8_t *)"NEW OIL", 0);
			screenPrintString(8, 24, (uint8_t *)tankLeft, 0);
			screenVerticalBar(8, 44, adcValLeft/3, 80);
			
		}else{
			lastAdcValueRight = getExtAdc(ADC_ADDR, AIN2AIN3, FSR_0_256);
			adcValRight = lastAdcValueRight-callValues.RTank;
			adcValRight = (adcValRight <= 0) ? 0 : adcValRight;
			screenPrintString(184, 6, (uint8_t *)"OLD OIL", 0);
			screenPrintString(192, 24, (uint8_t *)tankRight, 0);
			screenVerticalBar(192, 44, adcValRight/3, 80);
		}
		
		
		
		uint8_t preselect = 0;
		if (currentScr == mainScreen){
			preselect = (menuPointer == 1) ? 2 : 0;
			screenPrintString(75, 40, (uint8_t *)"Run cycle", preselect);
			preselect = (menuPointer == 2) ? 2 : 0;
			screenPrintString(75, 57, (uint8_t *)"Empty tank", preselect);
			preselect = (menuPointer == 3) ? 2 : 0;
			screenPrintString(75, 74, "Setup    ", preselect);
			screenPrintString(75, 91, "         ", 0);
			screenPrintString(75, 108, "         ", 0);
		}
		if (currentScr == setupScreen){
			sprintf(scrLineBuff, "Valve 1 %d", valve1);
			preselect = (menuPointer == 1) ? 2 : 0;
			screenPrintString(75, 40, scrLineBuff, preselect);
			
			sprintf(scrLineBuff, "Valve 2 %d", valve2);
			preselect = (menuPointer == 2) ? 2 : 0;
			screenPrintString(75, 57, scrLineBuff, preselect);
			
			sprintf(scrLineBuff, "Pump    %d", pump);
			preselect = (menuPointer == 3) ? 2 : 0;
			screenPrintString(75, 74, scrLineBuff, preselect);
			preselect = (menuPointer == 4) ? 2 : 0;
			screenPrintString(75, 91, "SET >0<  ", preselect);
			preselect = (menuPointer == 5) ? 2 : 0;
			screenPrintString(75, 108, "BACK    ", preselect);
		}
		
		//Menu
		
		
		
		screenKeepAlive();
    }
}

