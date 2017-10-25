/*******************************************
/*******************************************
/***@file	uArmGroveAngleSensor.cpp
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170906
/*******************************************
*******************************************/

#include "uArmGroveAngleSensor.h"


uArmGroveAngleSensor::uArmGroveAngleSensor()
{
}

bool uArmGroveAngleSensor::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	//debugPrint("clk = %d, dat = %d\r\n", clk_pin, dat_pin);

	return true;
}

void uArmGroveAngleSensor::report()
{
	char result[128];

	uint16_t value = getAnalogPinValue(_clk_pin);
	//debugPrint("value=%d\r\n", value);

	msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, _portNum, GROVE_ANGLE_SENSOR, value);
	reportString(result);
}

void uArmGroveAngleSensor::control()
{
}

void uArmGroveAngleSensor::tick()
{
}

