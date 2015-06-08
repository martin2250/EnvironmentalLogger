/*
 * Settings.h
 *
 * Created: 05.04.2015 14:15:54
 *  Author: Martin
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <inttypes.h>

extern uint16_t IntervalMin;
extern uint16_t NextEEPROMPage;
extern uint16_t DisplayTimeoutMin;
extern uint16_t LoggingEnabled;

typedef struct
{
	unsigned int Buttons : 2;
	unsigned int Timer1Active : 1;
	unsigned int LCD_On : 1;
	unsigned int RTCAlarm : 1;
	unsigned int LoadTransmitBuffer : 1;
	unsigned int Transmitting : 1;
	unsigned int MeasurementDone : 1;
}LoggerStatus;

extern LoggerStatus Status;

void LCDOffBtnHandler(void);
extern funcPtr CurrentButtonHandler;

typedef struct
{
	unsigned int ShowStatus : 1;
	unsigned int Measure : 1;	
}MeasurementHandlers;

extern MeasurementHandlers CurrentMeasurementHandlers;

#endif /* SETTINGS_H_ */