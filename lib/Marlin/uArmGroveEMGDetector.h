/*******************************************
/*******************************************
/***@file	uArmGroveEMGDetector.h
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	201709011
/*******************************************
*******************************************/

#ifndef _UARMGROVE_EMG_DETECTOR_H_
#define _UARMGROVE_EMG_DETECTOR_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"


class uArmGroveEMGDetector : public uArmGroveBase
{
public:
	uArmGroveEMGDetector();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

};


#endif // _UARMGROVE_EMG_DETECTOR_H_
