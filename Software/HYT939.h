/*
 * HYT939.h
 *
 * Created: 06.04.2015 12:04:57
 *  Author: Martin
 */ 


#ifndef HYT939_H_
#define HYT939_H_

void HYT_request();
void HYT_read(void);

extern uint16_t temp_raw;
extern uint16_t hum_raw;

#endif /* HYT939_H_ */