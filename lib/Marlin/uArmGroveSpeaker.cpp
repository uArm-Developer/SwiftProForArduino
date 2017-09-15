/*******************************************
/*******************************************
/***@file	uArmGroveSpeaker.cpp
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#include "uArmGroveSpeaker.h"


uArmGroveSpeaker::uArmGroveSpeaker()
{
}

bool uArmGroveSpeaker::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	_uArmGroveSpeaker.setPin(_clk_pin);

	return true;
}

void uArmGroveSpeaker::report()
{
}

void uArmGroveSpeaker::control()
{
	char result[128];
	uint8_t value;

	if (code_seen('V'))
	{
		value = code_value_byte();

		debugPrint("value:%d\r\n", value);
/*
		if (value > 0 && value < 8)
		{
			_uArmGroveSpeaker.speed(BassTab[value]);
		}
		else
		{
			_uArmGroveSpeaker.off();
			if (value != 0)
			{
				msprintf(result, "Please enter 0 to 6 \r\n");
				reportString(result);
			}

		}
*/
		if(value != 0)
			_uArmGroveSpeaker.speed(1000);
		else
			_uArmGroveSpeaker.off();
	}
}

void uArmGroveSpeaker::tick()
{
}

