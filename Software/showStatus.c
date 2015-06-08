/*
* showStatus.c
*
* Created: 06.04.2015 00:01:56
*  Author: Martin
*/
#include "defines.h"
#include "global.h"
#include "menuFunction.h"
#include "HYT939.h"
#include "time.h"
#include "util.h"
#include "memstrings.h"
#include "logging.h"

#include "lib/hd44780.h"

void StatusAddMeasurements()
{
	char buffer[9];
	GetDateString(buffer);
	lcd_puts(buffer);
	lcd_putc(' ');
	
	int16_t temperature = temp_raw / 10;
	temperature -= 400;
	
	if(temperature < 0)
	{
		lcd_putc('-');
		temperature = -temperature;
	}
	else
	{
		lcd_putc(' ');
	}
	
	lcd_printNum((uint16_t)temperature, 1, 4);
	lcd_putc(0xdf);								//°
	lcd_putc('C');
	
	lcd_goto(40);
	
	GetTimeString(buffer);
	buffer[5] = 0;				//don't display seconds
	lcd_puts(buffer);
	
	//lcd_putc('|');
	lcd_putc(' ');
	
	lcd_printNum(readVcc(), 1, 3);
	lcd_putc('V');
	//lcd_putc('|');
	lcd_putc(' ');
	
	uint16_t humidity = hum_raw / 16;
	
	lcd_printNum(humidity, 1, 4);
	lcd_putc('%');
}

const char OutOf42String[] PROGMEM = "/40";
const char OutOf1024String[] PROGMEM = "/1024";
const char ROMSTRING[] PROGMEM = "ROM: ";
const char RAMSTRING[] PROGMEM = "RAM: ";

void ShowStatusDisplay2Handler()
{
	lcd_clrscr();
	lcd_puts_P(RAMSTRING);
	lcd_printNum(CurrentPageBuffer[0], 0, 2);
	lcd_puts_P(OutOf42String);
	lcd_goto(41);
	lcd_puts_P(ROMSTRING);	
	lcd_printNum(NextEEPROMPage, 0, 4);
	lcd_puts_P(OutOf1024String);
	
	CurrentButtonHandler = LcdOff;
}

void ShowStatus()
{
	lcd_clrscr();
	CurrentButtonHandler = ShowStatusDisplay2Handler;
	CurrentMeasurementHandlers.ShowStatus = 1;
	HYT_request();
	Status.LCD_On = 1;
}