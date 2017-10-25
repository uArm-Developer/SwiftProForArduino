/************************************************
/************************************************
/***@file	uArmGroveVibrationMotor.cpp
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170905
/************************************************
************************************************/

#include "uArmGroveVibrationMotor.h"


uArmGroveVibrationMotor::uArmGroveVibrationMotor()
{
}

bool uArmGroveVibrationMotor::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;	

	_uArmGroveVibrationMotor.setPin(_clk_pin);

	return true;	
}

void uArmGroveVibrationMotor::report()
{
}

void uArmGroveVibrationMotor::control()
{
	uint8_t value;		
	
	if (code_seen('V'))
	{
		value = code_value_byte();	

		if (value)
		{
			_uArmGroveVibrationMotor.on();
		}
		else
		{
			_uArmGroveVibrationMotor.off();
		}
	}		
}

void uArmGroveVibrationMotor::tick()
{
}

