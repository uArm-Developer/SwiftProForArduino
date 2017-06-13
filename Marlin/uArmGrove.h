/**
  ******************************************************************************
  * @file	uArmGrove.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#ifndef _UARMGROVE_H_
#define _UARMGROVE_H_

#include <Arduino.h>
#include "Marlin.h"

enum GrovePort
{
	GROVE_PORT_DEFAULT = 0,	
	
	GROVE_PORT_D0,	// D8
	GROVE_PORT_D1,	// D9
	GROVE_PORT_D2,
	GROVE_PORT_A0,	// A13
	GROVE_PORT_A1,
	GROVE_PORT_I2C0,	// I2C
	GROVE_PORT_I2C1,
	
	GROVE_PORT_COUNT
};


enum GroveType
{
	GROVE_TYPE_NONE = 0,
	
	GROVE_COLOR_SENSOR = 10,
	GROVE_GESTURE_SERSOR = 11,
	GROVE_ULTRASONIC	= 12,

	GROVE_FAN = 13,
	
	GROVE_TYPE_COUNT
};

void initGroveModule(GroveType type, GrovePort port);

void setGroveModuleReportInterval(GroveType type, long interval);

void setGroveModuleValue(GroveType type, long value);


#endif // _UARMGROVE_H_
