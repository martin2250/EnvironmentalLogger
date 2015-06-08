/*
 * menuItems.h
 *
 * Created: 05.04.2015 22:38:18
 *  Author: Martin
 */ 


#ifndef MENUITEMS_H_
#define MENUITEMS_H_

typedef struct
{
	const char* Text;
	funcPtr Event;
	void* Value;
	unsigned int IsFunction : 1;
}MenuEntry;

MenuEntry MenuEntries[] =
{
	{
		ExitString,
		LcdOff,
		0,
		0
	},
	{
		TransmitString,
		StartTransmission,
		0,
		0
	},
	{
		EnabledString,
		menuToggleLogging,
		&LoggingEnabled,
		0
	},
	{
		TimeString,
		menuSetTime,
		GetTimeString,
		1
	},
	{
		DateString,
		menuSetDate,
		GetDateString,
		1
	},
	{
		IntervalString,
		menuSetInterval,
		&IntervalMin,
		0
	},
	{
		TimeoutString,
		menuSetTimeout,
		&DisplayTimeoutMin,
		0
	},
	{
		EEPROMString,
		menuResetEEPROM,
		&NextEEPROMPage,
		0
	}
};



#endif /* MENUITEMS_H_ */