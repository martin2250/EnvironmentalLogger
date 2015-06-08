/*
* logging.c
*
* Created: 06.04.2015 13:51:41
*  Author: Martin
*/

#include "defines.h"
#include <inttypes.h>
#include "time.h"
#include "logging.h"
#include "util.h"
#include "24AA1025.h"
#include "global.h"
#include "HYT939.h"
#include "lib/i2cmaster.h"
#include <avr/eeprom.h>

uint8_t CurrentPageBuffer[128];

void AddMeasurement()
{
	if(CurrentPageBuffer[0] == 0)
	{		
		CurrentPageBuffer[1] = IntervalMin & 0x3F;
		uint8_t bat = readVcc();
		CurrentPageBuffer[1] |= bat << 6;
		CurrentPageBuffer[2] = (bat >> 2) & 0xF;
		updateTime();
		CurrentPageBuffer[2] |= TimeMonth << 4;
		CurrentPageBuffer[3] = TimeYear;
		CurrentPageBuffer[4] = TimeDay;
		CurrentPageBuffer[5] = TimeHour;
		CurrentPageBuffer[6] = TimeMinute;
		CurrentPageBuffer[7] = TimeSecond;
	}
	
	CurrentPageBuffer[8 + (CurrentPageBuffer[0] * 3)] = temp_raw >> 6;
	CurrentPageBuffer[9 + (CurrentPageBuffer[0] * 3)] = (temp_raw << 2) & 0xF0;
	
	CurrentPageBuffer[9 + (CurrentPageBuffer[0] * 3)] |= hum_raw >> 10 & 0xF;
	CurrentPageBuffer[10 + (CurrentPageBuffer[0] * 3)] = (hum_raw >> 2) & 0xFF;
	
	CurrentPageBuffer[0]++;
}

void InitPage()
{
	for(uint8_t i = 0; i < 128; i++)
	{
		CurrentPageBuffer[i] = 0;
	}
}

void StorePage()
{
	if(CurrentPageBuffer[0] != 0)
	{
		uint8_t* DataPtr = (uint8_t *)(&CurrentPageBuffer);
		
		startEEPROMAddress(NextEEPROMPage);
		NextEEPROMPage++;
		eeprom_write_word(NextPageAddress, NextEEPROMPage);
		
		
		for(uint8_t i = 0; i < 128; i++)
		{
			i2c_write(DataPtr[i]);
		}
		
		i2c_stop();
	}
	
	InitPage();
	
	if(NextEEPROMPage >= 1024)
	{
		LoggingEnabled = 0;
	}
}