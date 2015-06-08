/*
* sendData.c
*
* Created: 06.04.2015 10:41:25
*  Author: Martin
*/

#include "defines.h"
#include "lib/i2cmaster.h"
#include "global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "memstrings.h"
#include "24AA1025.h"
#include "lib/hd44780.h"

uint8_t TxBuffer[128];
uint8_t TxBufferPointer;
uint16_t TxPagePointer;
uint8_t TxAbort;

void TxButtonHandler()
{
	if(Status.Buttons == (ButtonL | ButtonR))
	{
		TxAbort = 1;
	}
	Status.Buttons = 0;
}

/*
Baud: 250.000
UBRR: 3
X2U = 1  (double speed)

*/

void NextPage()
{
	Status.LoadTransmitBuffer = 0;
	
	if(TxPagePointer < NextEEPROMPage && !TxAbort)
	{
		lcd_goto(0);
		lcd_printNum(TxPagePointer + 1, 0, 4);
		
		startEEPROMAddress(TxPagePointer);
		
		if(TxPagePointer & 0x0200)		//>511?
		i2c_start(I2C_READ(EEPROM_HIGH));
		else
		i2c_start(I2C_READ(EEPROM_LOW));
		
		for(uint8_t index = 0; index < 127; index++)
		{
			TxBuffer[index] = i2c_readAck();
		}
		
		TxBuffer[127] = i2c_readNak();
		
		i2c_stop();
		
		TxPagePointer++;
		TxBufferPointer = 1;
		UDR0 = TxBuffer[0];
		UCSR0B = _BV(TXEN0) | _BV(UDRIE0);		//enable tx interrupt
	}
	else
	{
		CurrentButtonHandler = LCDOffBtnHandler;
		Status.Transmitting = 0;
		Status.LCD_On = 0;
		
		UCSR0B = 0;
		
		//Finished
	}
}

void StartTransmission()
{
	Status.Transmitting = 1;
	
	TxPagePointer = 0;
	TxAbort = 0;
	
	CurrentButtonHandler = TxButtonHandler;
	
	lcd_clrscr();
	lcd_printNum(TxPagePointer + 1, 0, 4);
	lcd_putc('/');
	lcd_printNum(NextEEPROMPage, 0, 0);
	
	lcd_goto(40);
	lcd_puts_P(TransmittingString);
	
	power_usart0_enable();
	
	UCSR0B = _BV(TXEN0);					//enable transmitter
	
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = 0;
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = 0;
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = 255;
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = NextEEPROMPage >> 8;
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = NextEEPROMPage & 0xFF;
	while (!(UCSR0A & (1<<UDRE0)));
		
	NextPage();
}

ISR(USART_UDRE_vect)
{
	if(TxBufferPointer < 128)
	{
		UDR0 = TxBuffer[TxBufferPointer];
		TxBufferPointer++;
	}
	else
	{
		Status.LoadTransmitBuffer = 1;
		UCSR0B = 0;
	}
}