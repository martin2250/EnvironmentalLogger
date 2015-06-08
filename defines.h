/*
 * defines.h
 *
 * Created: 23.10.2014 17:10:00
 *  Author: Martin
 */ 


#ifndef DEFINES_H_
#define DEFINES_H_

#ifndef F_CPU
	#define F_CPU 8000000UL
#endif

#define DS1337 0x68
#define EEPROM_LOW 0x50
#define EEPROM_HIGH 0x54
#define HYT939 0x28

#define ON PORTB |= _BV(7);
#define OFF PORTB &= ~_BV(7);

#define ButtonR 2
#define	ButtonL 1

#define Timer1TCCRB (_BV(CS11) | _BV(CS10)) //64 prescaler -> 128ms bei OCR1A = 16k
#define Timer0TCCRB (_BV(CS02) | _BV(CS00))

typedef void (*funcPtr)(void);
typedef void (*charFuncPtr)(char*);


#define NextPageAddress (((uint16_t*)0) + 2)
#define IntervalMinAddress (((uint16_t*)0) + 3)
#define DisplayTimeoutAddress (((uint16_t*)0) + 4)
#define LoggingEnabledAddress (((uint16_t*)0) + 5)

#endif /* DEFINES_H_ */