/*
 * menustrings.h
 *
 * Created: 05.04.2015 12:23:30
 *  Author: Martin
 */ 

#ifndef MENUSTRINGS_H_
#define MENUSTRINGS_H_

#include "avr/pgmspace.h"

extern const char ExitString[] PROGMEM;
extern const char TransmitString[] PROGMEM;
extern const char EnabledString[] PROGMEM;
extern const char IntervalString[] PROGMEM;
extern const char TimeString[] PROGMEM;
extern const char DateString[] PROGMEM;
extern const char TimeoutString[] PROGMEM;
extern const char EEPROMString[] PROGMEM;



extern const char TransmittingString[] PROGMEM;

extern const char ResetEEPROMQuestion[] PROGMEM;
extern const char AbortString[] PROGMEM;
extern const char ResetEEPROMDoneString[] PROGMEM;
extern const char MemoryFullString[] PROGMEM;

#endif /* MENUSTRINGS_H_ */