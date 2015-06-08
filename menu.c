/*
 * menu.c
 *
 * Created: 27.10.2014 17:12:49
 *  Author: Martin
 */ 
#include "defines.h"
#include <avr/pgmspace.h>
#include "memstrings.h"
#include "menu.h"
#include "global.h"
#include "lib/hd44780.h"
#include "menuFunction.h"
#include "sendData.h"
#include "time.h"



#include "menuItems.h"

#define MenuLength (sizeof(MenuEntries) / sizeof(MenuEntries[0]))

uint8_t DisplayPosition = 0;
uint8_t CursorPosition = 0;
uint8_t IsInitialized = 0;

void PrintLine(uint8_t index)
{
	if(index == CursorPosition)
	lcd_putc(0x7E);	// ->
	else
	lcd_putc(' ');
	
	lcd_puts_P(MenuEntries[index].Text);
	
	if(MenuEntries[index].Value)
	{
		if(MenuEntries[index].IsFunction)
		{
			char buffer[9];
			((charFuncPtr)MenuEntries[index].Value)(buffer);
			lcd_puts(buffer);
		}
		else
		{
			lcd_printNum(*((uint16_t*)MenuEntries[index].Value), 0, 4);
		}
	}
}

void PrintMenu()
{
	lcd_clrscr();

	PrintLine(DisplayPosition);
	lcd_goto(40);
	PrintLine(DisplayPosition + 1);
}

void MenuUp()
{
	if(CursorPosition > 0)
		CursorPosition--;
	
	if(DisplayPosition > CursorPosition)
		DisplayPosition = CursorPosition;
		
	PrintMenu();
}

void MenuDown()
{
	if(CursorPosition + 1 < MenuLength)
		CursorPosition++;
		
	if(DisplayPosition < CursorPosition - 1)
		DisplayPosition = CursorPosition - 1;
	
	PrintMenu();
}

void MenuButtonPress()
{
	if(!IsInitialized)
	{
		PrintMenu();
		IsInitialized = 1;
		Status.Buttons = 0;
		return;
	}
	if(Status.Buttons == (ButtonL | ButtonR))
	{
		if(MenuEntries[CursorPosition].Event)
		{
			MenuEntries[CursorPosition].Event();
			
			DisplayPosition = 0;
			CursorPosition = 0;
			IsInitialized = 0;
		}
		Status.Buttons = 0;
	}
	else if(Status.Buttons == ButtonL)
	{
		MenuDown();
		Status.Buttons = 0;
	}
	else if(Status.Buttons == ButtonR)
	{
		MenuUp();
		Status.Buttons = 0;
	}
}
