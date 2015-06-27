/*
* HYT939.c
*
* Created: 06.04.2015 12:04:42
*  Author: Martin
*/

#include "defines.h"
#include "global.h"
#include "lib/i2cmaster.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

uint16_t temp_raw;
uint16_t hum_raw;


void HYT_request()
{
	i2c_start(I2C_WRITE(HYT939));
	i2c_stop();
	
	wdt_reset();

	cli();
	WDTCSR |= (_BV(WDCE) | _BV(WDE));	//start timed sequence
	WDTCSR = _BV(WDIE) | _BV(WDP1);		//64ms
	sei();
}


void HYT_read(void)
{
	i2c_start(I2C_READ(HYT939));
	
	hum_raw = i2c_readAck() & 0x3f;
	hum_raw <<= 8;
	hum_raw |= i2c_readAck();
	
	temp_raw = i2c_readAck();
	temp_raw <<= 6;
	temp_raw |= i2c_readNak() >> 2;
	
	i2c_stop();
}

ISR(WDT_vect)
{
	//disable wdt
	wdt_disable();
	
	Status.MeasurementDone = 1;
}