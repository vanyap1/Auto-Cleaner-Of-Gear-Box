/*
Copyright (c) 2021 Devilbinder (Binder Tronics) https://bindertronics.com/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */ 



//sprintf(char_array , "Counter: %d", buff_count);
//uart_send_string((char *)char_array);


#include "uart_hal.h"
#include "string.h"
#include <stdbool.h>

volatile static uint8_t rx_buffer[RX_BUFFER_SIZE] = {0};
volatile static uint16_t rx_count = 0;	
volatile static uint8_t uart_tx_busy = 1;
static uint16_t rx_read_pos = 0;
char test_array[128]="";
bool read_complete = true;
char keyCode = 0xff;
char tmpkeyChar;
bool screenReady = 0;

uint8_t init0[2] = {0xe8, 0x06};
uint8_t init1[2] = {0xe6, 0x32};
uint8_t init2[2] = {0xb5, 0x01};
uint8_t init3[3] = {0xcb, 0x0e, 0x01};
	
	

bool serial_complete(void)
{
	return read_complete;
}

uint8_t getKey(){
	read_complete = false;
	uint8_t returnVal = keyCode;
	keyCode = 0xff;
	return returnVal;
}

const char* serial_read_data(void){
	//uint8_t serial_read_data(void){
	read_complete = false;
	rx_count=0;
	return test_array;
}




ISR(USART0_RX_vect){
	tmpkeyChar = UDR0;
	if (tmpkeyChar != 0xCC){
		read_complete = true;
		keyCode = tmpkeyChar;
	}else{
		screenReady = true;
	}	
}

//memset(rx_buffer, 0, sizeof(rx_buffer));

ISR(USART0_TX_vect){
	uart_tx_busy = 1;
}


void uart_init(uint32_t baud,uint8_t high_speed){
	UCSR0B = 0;
	UCSR0A = 0;
	uint8_t speed = 16;
	
	if(high_speed != 0){
		speed = 8;
		UCSR0A |= 1 << U2X0;
	}
	
	baud = (F_CPU/(speed*baud)) - 1;
	
	UBRR0H = (baud & 0x0F00) >> 8;
	UBRR0L = (baud & 0x00FF);
	
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0) | (1 << TXCIE0);
	UCSR0C=0b10000110;
}


void uart_send_byte(uint8_t c){
	while(uart_tx_busy == 0);
	uart_tx_busy = 0;
	UDR0 = c;
}

void uart_send_array(uint8_t *c,uint16_t len){
	screenReady = false;
	for(uint16_t i = 0; i < len;i++){
		uart_send_byte(c[i]);
	}
	while(screenReady == false);
}

void uart_send_array_simple(uint8_t *c,uint16_t len){
	for(uint16_t i = 0; i < len;i++){
		uart_send_byte(c[i]);
	}
	_delay_ms(25);
}




void screenInit(void){
		uart_init(9600,1);
		uart_send_array_simple((uint8_t *)init0, sizeof(init0));
		_delay_ms(6);
		uart_init(74880,1);
		uart_send_array_simple((uint8_t *)init1, sizeof(init1));
		_delay_ms(1);
		uart_send_array_simple((uint8_t *)init2, sizeof(init2));
		_delay_ms(10);
		//uart_send_array((uint8_t *)init3, sizeof(init3));
		//_delay_ms(10);
		
		_delay_ms(1100);
		uart_send_byte(0xf4);
		_delay_ms(100);
		
		uint8_t initData[16] = {0xc7, 0x01, 0x0, 0x0, 0x1e, 0x0, 0x03}; //horisontal line
		uart_send_array((uint8_t *)initData, 7);
		
		initData[2] = 125;
		uart_send_array((uint8_t *)initData, 7);
		_delay_ms(100);
		//uint8_t initData1[6] = {0xc5, 0x06, 0xea, 0x00, 0x02, 0x74}; //vertical line
		//uart_send_array((uint8_t *)initData1, 6);
		//_delay_ms(10);
		
		
		uart_send_array((uint8_t *)init3, sizeof(init3));
		_delay_ms(10);

}


void screenPrintString(uint8_t xPox, uint8_t yPox, uint8_t *c, uint8_t selectType){
	//if(selectType == 0 || selectType == 1){
		//uint8_t tmp[7] = {0xc7, selectType, yPox, xPox, 10, 0x06, 16};
		//uart_send_array((uint8_t *)tmp, sizeof(tmp));
	//}
	screenReady = false;
	uint16_t i = 0;
		
	uart_send_byte(0xeb); //0xeb
	uart_send_byte(yPox); //0x20
	uart_send_byte(xPox); //0x0c
	
	do{
		uart_send_byte(c[i]);
		i++;
	}while(c[i] != '\0');
	uart_send_byte(' ');
	uart_send_byte(0x0);
	while(screenReady == false);
	if(selectType == 2){
		uint8_t tmp[7] = {0xc7, selectType, yPox, xPox, strlen((char *)c), 0x06, 16};
		uart_send_array((uint8_t *)tmp, sizeof(tmp));
	}
	
}



void screenVerticalBar(uint8_t xPox, uint8_t yPox, uint8_t barVal, uint8_t maxHeight){
	
	uint8_t barProgVertical[10] = {0xfd,0x3c,xPox,0x05,0x01,0xff,0xff,0xff,0xff,0xff};
	for (uint8_t i=maxHeight; i>0; i-- ){				
		
		barProgVertical[1] = (yPox + maxHeight) - i;
		
		if(i<=barVal){
			barProgVertical[5] = 0xff;
			barProgVertical[6] = 0xff;
			barProgVertical[7] = 0xff;
			barProgVertical[8] = 0xff;
			barProgVertical[9] = 0xff;
			}else{
			barProgVertical[5] = 0x80;
			barProgVertical[6] = 0x0;
			barProgVertical[7] = 0x0;
			barProgVertical[8] = 0x0;
			barProgVertical[9] = 0x01;
		}
		uart_send_array(barProgVertical, sizeof(barProgVertical));
		
	}
	barProgVertical[5] = 0xff;
	barProgVertical[6] = 0xff;
	barProgVertical[7] = 0xff;
	barProgVertical[8] = 0xff;
	barProgVertical[9] = 0xff;
	uart_send_array(barProgVertical, sizeof(barProgVertical));
}





uint16_t uart_read_count(void){
	return rx_count;
}




uint8_t uart_flush(void){
	rx_count = 0;
	rx_read_pos = 0;
	//memset((char *)rx_buffer, 0, 128);
	return 0;
}

uint8_t uart_read(void){
	
	uint8_t data = 0;
	data = rx_buffer[rx_read_pos];
	rx_read_pos++;
	rx_count--;
	return data;
}
