
/*
 * twi1_slave.c
 *
 * Created: 17.11.2023 23:05:32
 *  Author: Vanya
 */ 
#include "twi1_slave.h"

uint8_t *twi_buff;
uint8_t byteIndex;
static volatile uint8_t status = 0xF8;






