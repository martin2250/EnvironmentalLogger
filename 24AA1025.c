/*
 * EEPROM.c
 *
 * Created: 06.04.2015 11:30:30
 *  Author: Martin
 */ 

#include "defines.h"
#include "lib/i2cmaster.h"

void startEEPROMAddress(uint16_t page)
{
	if(page & 0x0200)		//>511?
	i2c_start(I2C_WRITE(EEPROM_HIGH));
	else
	i2c_start(I2C_WRITE(EEPROM_LOW));
	
	page &= 0x01FF;		//make <512
	
	page *= 128;			//multiplay by page size
	
	i2c_write(page >> 8);		//write high address
	i2c_write(page & 0xFF);		//write low address
}