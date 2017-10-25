/************************************************
/************************************************
/***@file	uArmGroveAirQualitySensor.h
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170907
/************************************************
************************************************/


#ifndef _UARMGROVE_AIR_QUALITY_SENSOR_H_
#define _UARMGROVE_AIR_QUALITY_SENSOR_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"


class uArmGroveAirQualitySensor : public uArmGroveBase
{
public:
	uArmGroveAirQualitySensor();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

};


#endif // _UARMGROVE_AIR_QUALITY_SENSOR_H_
