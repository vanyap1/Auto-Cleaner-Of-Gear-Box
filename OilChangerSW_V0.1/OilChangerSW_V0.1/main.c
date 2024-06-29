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
#include <stdio.h>
static FILE mystdout = FDEV_SETUP_STREAM((void *)uart_send_byte, NULL, _FDEV_SETUP_WRITE);


//Leds definition section
#define RED			0
#define GREEN		1
#define BLUE		2

gpio dev1[2] = {{(uint8_t *)&PORTC , PORTC0} , {(uint8_t *)&PORTE , PORTE3}};
gpio dev2[3] = {{(uint8_t *)&PORTC , PORTC2} , {(uint8_t *)&PORTC , PORTC1}};
gpio dev3[3] = {{(uint8_t *)&PORTC , PORTC5} , {(uint8_t *)&PORTC , PORTC4},{(uint8_t *)&PORTC , PORTC3}};

uint8_t menuPointer = 0;
char measBuffer[16] = "\0";

//uint8_t initData[16];
int main(void)
{
    char char_array[128]="test_data\0";
    uart_init(9600,1);
    //twi1_init(400000);
    
    
    gpio_set_pin_direction(&dev1[RED] , PORT_DIR_OUT); gpio_set_pin_level(&dev1[RED], true);
    gpio_set_pin_direction(&dev1[GREEN] , PORT_DIR_OUT); gpio_set_pin_level(&dev1[GREEN], true);
    
    gpio_set_pin_direction(&dev2[RED] , PORT_DIR_OUT); gpio_set_pin_level(&dev2[RED], true);
    gpio_set_pin_direction(&dev2[GREEN] , PORT_DIR_OUT); gpio_set_pin_level(&dev2[GREEN], true);
    
    gpio_set_pin_direction(&dev3[RED] , PORT_DIR_OUT); gpio_set_pin_level(&dev3[RED], true);
    gpio_set_pin_direction(&dev3[GREEN] , PORT_DIR_OUT); gpio_set_pin_level(&dev3[GREEN], true);
    gpio_set_pin_direction(&dev3[BLUE] , PORT_DIR_OUT); gpio_set_pin_level(&dev3[BLUE], true);
	
	sei();
	stdout = &mystdout;
	_delay_ms(500);
	screenInit();
	

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
				if(menuPointer < 3){
					menuPointer++;
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
			}			
		}
		
		
		sprintf(measBuffer, "Val=%02d; %d", 11, menuPointer); 
		screenPrintString(3, 6, (uint8_t *)"NEW OIL", 0);
		screenPrintString(8, 24, (uint8_t *)"10.5L", 0);
		screenPrintString(75, 0x03, (uint8_t *)"SW ver 0.1a", 0);
		
		
		screenPrintString(184, 6, (uint8_t *)"OLD OIL", 0);
		screenPrintString(192, 24, (uint8_t *)"17.3L", 0);

		screenVerticalBar(8, 44, 40, 80);
		screenVerticalBar(192, 44, 60, 80);
		
		
		//Menu
		uint8_t preselect = 0;
		preselect = (menuPointer == 0) ? 2 : 0;
		screenPrintString(75, 60, (uint8_t *)"Empty tank", preselect);
		preselect = (menuPointer == 1) ? 2 : 0;
		screenPrintString(75, 77, (uint8_t *)"Run cycle", preselect);
		preselect = (menuPointer == 2) ? 2 : 0;
		screenPrintString(75, 94, measBuffer, menuPointer);
		
    }
}

