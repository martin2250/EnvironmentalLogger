/*
 * time.h
 *
 * Created: 06.04.2015 09:19:06
 *  Author: Martin
 */ 


#ifndef TIME_H_
#define TIME_H_

uint8_t TimeYear, TimeMonth, TimeDay, TimeHour, TimeMinute, TimeSecond;

typedef struct
{
	unsigned int Ones : 4;
	unsigned int Tens : 4;
}DateTimeDigit;

extern DateTimeDigit DateDigits[3];
extern DateTimeDigit TimeDigits[3];
extern uint8_t TimeUpToDate;

void updateTime();
void GetTimeString(char* buffer);
void GetDateString(char* buffer);

#endif /* TIME_H_ */