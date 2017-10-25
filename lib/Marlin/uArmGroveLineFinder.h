/*******************************************
/*******************************************
/***@file	uArmGroveLineFinder.h
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#ifndef _UARMGROVE_LINE_FINDER_H_
#define _UARMGROVE_LINE_FINDER_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"

class uArmGroveLineFinder : public uArmGroveBase
{
public:
	uArmGroveLineFinder();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

private:
	uint8_t _value;
};


#endif // _UARMGROVE_LINE_FINDER_H_
