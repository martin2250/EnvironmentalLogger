/*
* global.c
*
* Created: 05.04.2015 15:00:22
*  Author: Martin
*/
#include "defines.h"
#include <inttypes.h>
#include "global.h"
#include "menu.h"
#include "showStatus.h"

uint16_t IntervalMin = 1;
uint16_t NextEEPROMPage = 0;	//first unused eeprom page (write then increment)
uint16_t DisplayTimeoutMin = 3;
uint16_t LoggingEnabled = 0;

LoggerStatus Status =
{
	0,		//Buttons
	0,		//Timer1Active
	0,		//LCD On
	0,
	0,
	0
};

funcPtr CurrentButtonHandler = LCDOffBtnHandler;
MeasurementHandlers CurrentMeasurementHandlers =
{
	0,
	0
};

void LCDOffBtnHandler()
{
	if(Status.Buttons == (ButtonR | ButtonL))
	{
		CurrentButtonHandler = MenuButtonPress;
		MenuButtonPress();
	}
	else
	{
		ShowStatus();
	}
}