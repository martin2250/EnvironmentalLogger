/*
* Logger.c
*
* Created: 20.10.2014 20:54:14
*  Author: Martin
*/
#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/power.h>

#include "lib/hd44780.h"
#include "lib/i2cmaster.h"
#include "util.h"
#include "menu.h"
#include "global.h"
#include "time.h"
#include "sendData.h"
#include "HYT939.h"
#include "showStatus.h"
#include "logging.h"


int main(void)
{
	cli();
	
	setupIO();
	
	sei();
	
	uint8_t TimeToNextMeasurement = IntervalMin;
	uint8_t DisplayTime = DisplayTimeoutMin;
	
	while(1)
	{
		//RTC 1 Min ALarm
		#pragma region RTC 1 Min ALarm

		if(Status.RTCAlarm)
		{
			if(Status.LCD_On)
			{				
				if(DisplayTime == 0)
				{
					Status.LCD_On = 0;
					CurrentButtonHandler = LCDOffBtnHandler;
				}
				
				DisplayTime--;
			}
			
			i2c_start(I2C_WRITE(DS1337));
			i2c_write(0xf);
			i2c_write(0);
			i2c_stop();
			
			if(LoggingEnabled && --TimeToNextMeasurement == 0)
			{
				TimeToNextMeasurement = IntervalMin;
				HYT_request();
				CurrentMeasurementHandlers.Measure = 1;
				
				if(CurrentPageBuffer[0] == 40)
				{
					StorePage();
				}
			}
			
			Status.RTCAlarm = 0;
		}
		
		#pragma endregion RTC 1 Min ALarm
		
		//Measurement Completed
		#pragma region Measurement Completed
		
		if(Status.MeasurementDone)
		{
			Status.MeasurementDone = 0;
			
			HYT_read();
			
			if(CurrentMeasurementHandlers.Measure)
			{
				AddMeasurement();
			}
			if(CurrentMeasurementHandlers.ShowStatus)
			{
				StatusAddMeasurements();
			}
			
			CurrentMeasurementHandlers.Measure = 0;
			CurrentMeasurementHandlers.ShowStatus = 0;
		}
		
		#pragma endregion Measurement Completed
		
		//Buttons
		#pragma region Buttons

		if(Status.Buttons)
		{
			DisplayTime = DisplayTimeoutMin;
			
			if(!Status.LCD_On)
			{
				lcd_init();
				Status.LCD_On = 1;
			}
			
			CurrentButtonHandler();
		
			Status.Buttons = 0;
		}
		
		#pragma endregion Buttons
		
		if(Status.LoadTransmitBuffer)
		{
			NextPage();
		}
		
		if(!Status.LCD_On)
			lcd_off();
		
		TimeUpToDate = 0;
		
		if(Status.Timer1Active | Status.Transmitting)
		{
			set_sleep_mode(SLEEP_MODE_IDLE);
			sleep_mode();
		}
		else
		{
			power_all_disable();
						
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_mode();
		}
	}
}

ISR(INT0_vect) //RTC
{
	Status.RTCAlarm = 1;
}

ISR(INT1_vect)	//power loss
{
	
}