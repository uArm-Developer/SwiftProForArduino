/*******************************************
/*******************************************
/***@file	uArmGroveSoundSensor.cpp
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170907
/*******************************************
*******************************************/

#include "uArmGroveSoundSensor.h"


uArmGroveSoundSensor::uArmGroveSoundSensor()
{
}

bool uArmGroveSoundSensor::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	//debugPrint("clk = %d, dat = %d\r\n", clk_pin, dat_pin);

	return true;
}

void uArmGroveSoundSensor::report()
{
	char result[128];
	uint16_t value = 0;


	value = getAnalogPinValue(_clk_pin);

	msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, _portNum, GROVE_SOUND_SENSOR, value);
	reportString(result);
}

void uArmGroveSoundSensor::control()
{
}

void uArmGroveSoundSensor::tick()
{
}

