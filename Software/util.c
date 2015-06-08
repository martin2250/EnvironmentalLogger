/*
 * util.c
 *
 * Created: 24.10.2014 17:29:31
 *  Author: Martin
 */ 

#include "defines.h"
#include "lib/i2cmaster.h"
#include "global.h"
#include "logging.h"

#include <util/delay.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/power.h>

uint8_t readVcc() {
	power_adc_enable();
	uint16_t result;
	// Read 1.1V reference against AVcc
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	ADCSRA = (1<<ADPS1) | (1<<ADPS0) | (1<<ADEN);
	_delay_ms(2); // Wait for Vref to settle
	
	ADCSRA |= _BV(ADSC); // First Conversion
	while (bit_is_set(ADCSRA,ADSC));
	result = ADC;
	_delay_ms(2);
	
	ADCSRA |= _BV(ADSC); // Convert
	while (bit_is_set(ADCSRA,ADSC));
	result = ADC;
	/*result = ADCL;
	result |= ADCH<<8;*/
	result = 11264 / result; // Back-calculate AVcc in mV
	ADCSRA = 0;
	ADMUX = 0;
	return (uint8_t)result;
}


void setupRTC()
{
	i2c_start(I2C_WRITE(DS1337));
	i2c_write(0xb);
	
	i2c_write(_BV(7));				//Alarm 2 once per minute
	i2c_write(_BV(7));
	i2c_write(_BV(7));
	
	i2c_write(_BV(1) | _BV(2));		//enable interrupt2, set intcn
	i2c_write(0);					//clear all flags
	i2c_stop();
}

void setupIO()
{
	DDRB = _BV(7);		//LED
	DDRC = 0;
	DDRD = 0;
	
	PORTD = _BV(PD0) | _BV(PD1);	//Btn	Right, TX
	PORTB = _BV(PB6);	//Btn	Left
	
	PCICR = _BV(PCIE0) | _BV(PCIE2);
	PCMSK2 = _BV(PCINT16);
	PCMSK0 = _BV(PCINT6);
	
	
	OCR1A = 10000;//12500;		//100ms
	
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;//_BV(FOC1A);
	TIMSK1 = _BV(OCIE1A);
	
	TCCR0A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	TIMSK0 = _BV(TOIE0);
	
	
	//Set up USART
	UBRR0 = 3; //3;								//baud 250.000 : 3   baud 9600: 103
	UCSR0A = _BV(U2X0);						//double baud
	UCSR0C =_BV(UCSZ01) | _BV(UCSZ00);		//8 bit mode
	
	
	NextEEPROMPage = eeprom_read_word(NextPageAddress);
	IntervalMin = eeprom_read_word(IntervalMinAddress);
	DisplayTimeoutMin = eeprom_read_word(DisplayTimeoutAddress);
	LoggingEnabled = eeprom_read_word(LoggingEnabledAddress);
	
	i2c_init();
	
	setupRTC();
	
	InitPage();
	
	EICRA = _BV(ISC01);		//int 0 falling edge
	EIMSK = _BV(INT0);
}