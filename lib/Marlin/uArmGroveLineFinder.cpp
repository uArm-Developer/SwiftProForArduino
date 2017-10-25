/*******************************************
/*******************************************
/***@file	uArmGroveLineFinder.cpp
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#include "uArmGroveLineFinder.h"


uArmGroveLineFinder::uArmGroveLineFinder()
{
}

bool uArmGroveLineFinder::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	pinMode(_clk_pin, INPUT);

	return true;
}

void uArmGroveLineFinder::report()
{
	char result[128];

	_value = digitalRead(_clk_pin) ? 0 : 1;

	msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, _portNum, GROVE_LINE_FINDER, _value);
	reportString(result);
}

void uArmGroveLineFinder::control()
{
}

void uArmGroveLineFinder::tick()
{
}
