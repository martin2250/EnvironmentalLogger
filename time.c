/*
 * time.c
 *
 * Created: 06.04.2015 09:18:55
 *  Author: Martin
 */ 

#include "defines.h"
#include "lib/i2cmaster.h"
#include "time.h"

DateTimeDigit DateDigits[3];
DateTimeDigit TimeDigits[3];

uint8_t TimeUpToDate = 0;

uint8_t bcd2bin(DateTimeDigit bcd)
{
	return bcd.Tens * 10 + bcd.Ones;
}

void updateTime()
{
	i2c_start(I2C_WRITE(DS1337));
	i2c_write(0x00);
	i2c_start(I2C_READ(DS1337));
	
	
	for(int8_t i = 3; i > 0; i --)
	{
		uint8_t raw = i2c_readAck();
		TimeDigits[i - 1] = *((DateTimeDigit*)&raw);
	}
	
	i2c_readAck();
	
	for(uint8_t i = 0; i < 3; i++)
	{
		uint8_t raw = i2c_readAck();
		DateDigits[i] = *((DateTimeDigit*)&raw);
	}
	i2c_stop();
	
	TimeHour = bcd2bin(TimeDigits[0]);
	TimeMinute = bcd2bin(TimeDigits[1]);
	TimeSecond = bcd2bin(TimeDigits[2]);
	TimeDay = bcd2bin(DateDigits[0]);
	TimeMonth = bcd2bin(DateDigits[1]);
	TimeYear = bcd2bin(DateDigits[2]);
	
	TimeUpToDate = 1;
}

void GetTimeString(char* buffer)
{
	updateTime();
	
	buffer[0] = TimeDigits[0].Tens;
	buffer[1] = TimeDigits[0].Ones;
	buffer[3] = TimeDigits[1].Tens;
	buffer[4] = TimeDigits[1].Ones;
	buffer[6] = TimeDigits[2].Tens;
	buffer[7] = TimeDigits[2].Ones;
	
	for(uint8_t i = 0; i < 8; i++)
	buffer[i] += '0';
	
	buffer[2] = ':';
	buffer[5] = ':';
	buffer[8] = 0;
}

void GetDateString(char* buffer)
{
	updateTime();
	
	buffer[0] = DateDigits[0].Tens;
	buffer[1] = DateDigits[0].Ones;
	buffer[3] = DateDigits[1].Tens;
	buffer[4] = DateDigits[1].Ones;
	buffer[6] = DateDigits[2].Tens;
	buffer[7] = DateDigits[2].Ones;
	
	for(uint8_t i = 0; i < 8; i++)
	buffer[i] += '0';
	
	buffer[2] = '.';
	buffer[5] = '.';
	buffer[8] = 0;
}