/*******************************************
/*******************************************
/***@file	uArmGroveAngleSensor.h
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170906
/*******************************************
*******************************************/

#ifndef _UARMGROVE_ANGLE_SENSOR_H_
#define _UARMGROVE_ANGLE_SENSOR_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"


class uArmGroveAngleSensor : public uArmGroveBase
{
public:
	uArmGroveAngleSensor();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

};


#endif // _UARMGROVE_ANGLE_SENSOR_H_
