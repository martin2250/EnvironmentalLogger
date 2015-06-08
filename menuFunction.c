/*
* menuFunction.c
*
* Created: 05.04.2015 22:22:19
*  Author: Martin
*/
#include "defines.h"
#include "global.h"
#include "memstrings.h"
#include "lib/hd44780.h"
#include "lib/i2cmaster.h"
#include <avr/pgmspace.h>
#include "time.h"
#include "logging.h"
#include "menu.h"

void LcdOff()
{
	Status.LCD_On = 0;
	CurrentButtonHandler = LCDOffBtnHandler;
}

uint8_t SetDigits[6];

const uint8_t SetDigitMaxTime[6] PROGMEM = {2, 9, 5, 9, 5, 9};
const uint8_t SetDigitMaxDate[6] PROGMEM = {3, 9, 1, 9, 9, 9};

uint8_t DigitPtr;

uint8_t SeperatorChar;
uint8_t DateTimeSel;
const char* SetDateTimeTitle;
const uint8_t* SetDateTimeMax;

void SetDateTimeDraw()
{
	lcd_clrscr();
	lcd_puts_P(SetDateTimeTitle);
	
	lcd_putc(SetDigits[0] + '0');
	lcd_putc(SetDigits[1] + '0');
	lcd_putc(SeperatorChar);
	lcd_putc(SetDigits[2] + '0');
	lcd_putc(SetDigits[3] + '0');
	lcd_putc(SeperatorChar);
	lcd_putc(SetDigits[4] + '0');
	lcd_putc(SetDigits[5] + '0');
	
	lcd_goto(40);
	
	for(uint8_t filler = 7 + DigitPtr + (DigitPtr > 1) + (DigitPtr > 3);filler > 0; filler--)
	lcd_putc(' ');
	
	lcd_putc('^');
}

void SetDateTimeBtnHandler()
{
	if(Status.Buttons == ButtonL)
	{
		if(SetDigits[DigitPtr] > 0)
		SetDigits[DigitPtr]--;
	}
	
	if(Status.Buttons == ButtonR)
	{
		if(SetDigits[DigitPtr] < pgm_read_byte(SetDateTimeMax + DigitPtr))
		SetDigits[DigitPtr]++;
	}
	
	if(Status.Buttons == (ButtonL | ButtonR))
	{
		if(DigitPtr < 5)
		{
			DigitPtr++;
		}
		else
		{
			if(DateTimeSel)
			{
				//Save Date
				i2c_start(I2C_WRITE(DS1337));
				i2c_write(0x04);	//startAddress
				i2c_write(SetDigits[0] << 4 | SetDigits[1]);
				i2c_write(SetDigits[2] << 4 | SetDigits[3]);
				i2c_write(SetDigits[4] << 4 | SetDigits[5]);
				i2c_stop();
			}
			else
			{
				//Save Time
				i2c_start(I2C_WRITE(DS1337));
				i2c_write(0x00);	//startAddress
				i2c_write(SetDigits[4] << 4 | SetDigits[5]);
				i2c_write(SetDigits[2] << 4 | SetDigits[3]);
				i2c_write(SetDigits[0] << 4 | SetDigits[1]);
				i2c_stop();
			}
			CurrentButtonHandler = LCDOffBtnHandler;
			Status.LCD_On = 0;
		}
	}
	
	Status.Buttons = 0;
	SetDateTimeDraw();
}

void SetDateTimeInit(DateTimeDigit* LoadDigits)
{
	CurrentButtonHandler = SetDateTimeBtnHandler;
	DigitPtr = 0;
	
	SetDigits[0] = LoadDigits[0].Tens;
	SetDigits[1] = LoadDigits[0].Ones;
	SetDigits[2] = LoadDigits[1].Tens;
	SetDigits[3] = LoadDigits[1].Ones;
	SetDigits[4] = LoadDigits[2].Tens;
	SetDigits[5] = LoadDigits[2].Ones;
	
	SetDateTimeDraw();
}

void menuSetTime()
{
	SeperatorChar = ':';
	DateTimeSel = 0;
	SetDateTimeTitle = TimeString;
	SetDateTimeMax = SetDigitMaxTime;
	
	updateTime();
	
	SetDateTimeInit(TimeDigits);
}

void menuSetDate()
{
	SeperatorChar = '.';
	DateTimeSel = 1;
	SetDateTimeTitle = DateString;
	SetDateTimeMax = SetDigitMaxDate;
	
	updateTime();
	
	SetDateTimeInit(DateDigits);
}


const char* SetNumberTitleString;
uint16_t SetNumberMin;
uint16_t SetNumberMax;
uint16_t SetNumberTemp;
funcPtr SetNumberDone;

void SetNumberPrint()
{
	lcd_clrscr();
	lcd_puts_P(SetNumberTitleString);
	lcd_printNum(SetNumberTemp, 0, 4);
}

void SetNumberBtnHandler()
{
	if(Status.Buttons == (ButtonL | ButtonR))
	{
		SetNumberDone();
		
		Status.LCD_On = 0;
		CurrentButtonHandler = LCDOffBtnHandler;
	}
	else if(Status.Buttons == ButtonL)
	{
		if(SetNumberTemp != SetNumberMin)
		{
			SetNumberTemp--;
			SetNumberPrint();
		}
	}
	else if(Status.Buttons == ButtonR)
	{
		if(SetNumberTemp != SetNumberMax)
		{
			SetNumberTemp++;
			SetNumberPrint();
		}
	}
	
	Status.Buttons = 0;
}

#include <avr/eeprom.h>

void SetNumberIntervalDone()
{
	if(IntervalMin != SetNumberTemp)
	{
		IntervalMin = SetNumberTemp;
		
		eeprom_write_word(IntervalMinAddress, IntervalMin);
		StorePage();
	}
}

void menuSetInterval()
{
	SetNumberTemp = IntervalMin;
	SetNumberMin = 1;
	SetNumberMax = 60;
	SetNumberTitleString = IntervalString;
	
	CurrentButtonHandler = SetNumberBtnHandler;
	SetNumberDone = SetNumberIntervalDone;
	
	SetNumberPrint();
}

void SetNumberTimeoutDone()
{
	if(DisplayTimeoutMin != SetNumberTemp)
	{
		DisplayTimeoutMin = SetNumberTemp;
		
		eeprom_write_word(DisplayTimeoutAddress, DisplayTimeoutMin);
	}
}

void menuSetTimeout()
{
	SetNumberTemp = DisplayTimeoutMin;
	SetNumberMin = 1;
	SetNumberMax = 5;
	SetNumberTitleString = TimeoutString;
	
	CurrentButtonHandler = SetNumberBtnHandler;
	SetNumberDone = SetNumberTimeoutDone;
	
	SetNumberPrint();
}


void menuResetEEPROMBtnHandler()
{
	lcd_clrscr();
	
	if(Status.Buttons == (ButtonL | ButtonR))
	{
		NextEEPROMPage = 0;
		eeprom_write_word(NextPageAddress, NextEEPROMPage);
		
		lcd_puts_P(ResetEEPROMDoneString);
	}
	else
	{
		lcd_puts_P(AbortString);
	}
	
	Status.Buttons = 0;
	CurrentButtonHandler = LcdOff;
}

void menuResetEEPROM()
{
	lcd_clrscr();
	lcd_puts_P(ResetEEPROMQuestion);
	
	CurrentButtonHandler = menuResetEEPROMBtnHandler;
}

void menuToggleLogging()
{
	if(LoggingEnabled)
	{
		StorePage();
		LoggingEnabled = 0;
		eeprom_write_word(LoggingEnabledAddress, LoggingEnabled);
	}
	else
	{
		if(NextEEPROMPage >= 1024)
		{
			lcd_clrscr();
			lcd_puts_P(MemoryFullString);
		}
		else
		{
			LoggingEnabled = 1;
			eeprom_write_word(LoggingEnabledAddress, LoggingEnabled);
		}		
	}
	Status.LCD_On = 0;
}