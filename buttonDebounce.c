/*
 * buttonDebounce.c
 *
 * Created: 05.04.2015 20:16:42
 *  Author: Martin
 */

#include "defines.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"
#include <avr/power.h>

#include <util/delay.h>

#define ButtonR_pressed() bit_is_clear(PIND, 0)
#define ButtonL_pressed() bit_is_clear(PINB, 6)

uint8_t GetBtns()
{
	return (ButtonR_pressed() << 1) | ButtonL_pressed();	//tested, works
}


ISR(TIMER1_COMPA_vect)
{
	TCCR1B = 0;
	Status.Timer1Active = 0;
	
	Status.Buttons = GetBtns();
}

ISR(PCINT0_vect)
{
	power_timer1_enable();
	TCCR1B = 0;
	TCNT1 = 0;
	
	if(GetBtns())
	{
		TCCR1B = Timer1TCCRB;
		Status.Timer1Active = 1;
	}
	else
	{
		Status.Timer1Active = 0;
	}
}

ISR(PCINT2_vect, ISR_ALIASOF(PCINT0_vect));