/*
 * logging.h
 *
 * Created: 09.04.2015 14:05:13
 *  Author: Martin
 */ 


#ifndef LOGGING_H_
#define LOGGING_H_

void InitPage();
void StorePage();
void AddMeasurement();


/*
// typedef struct
// {
// 	unsigned int Temperature : 12;
// 	unsigned int Humidity : 12;
// }TempHumDataPoint;
// 
// typedef struct
// {
// 	DateTime StartTime;				//32 bits
// 	unsigned int Interval : 6;		//6			38
// 	unsigned int Samples:6;			//6			44
// 	unsigned int BatteryLevel:6;	//6			50
// 	unsigned int : 14;
// 	//------------
// 	//			64
// 	TempHumDataPoint Data[40];
// }Page;
*/

extern uint8_t CurrentPageBuffer[128];

#endif /* LOGGING_H_ */