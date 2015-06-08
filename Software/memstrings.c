/*
 * memstrings.c
 *
 * Created: 05.04.2015 15:02:23
 *  Author: Martin
 */ 

#include <avr/pgmspace.h>


//Menu Items (Must contain filler to align value [optional])
const char ExitString[] PROGMEM =		"Exit menu";
const char TransmitString[] PROGMEM =	"Transmit Data";
const char EnabledString[] PROGMEM =	"Enabled:   ";
const char IntervalString[] PROGMEM =	"Interval:  ";
const char TimeString[] PROGMEM =		"Time:  ";
const char DateString[] PROGMEM =		"Date:  ";
const char TimeoutString[] PROGMEM =	"Timeout:   ";
const char EEPROMString[] PROGMEM =		"Page Index:";

const char TransmittingString[] PROGMEM = "  Transmitting";

const char ResetEEPROMQuestion[] PROGMEM = "Clear EEPROM?";
const char AbortString[] PROGMEM = "Aborted";
const char ResetEEPROMDoneString[] PROGMEM = "EEPROM cleared!";
const char MemoryFullString[] PROGMEM = "Memory full!";