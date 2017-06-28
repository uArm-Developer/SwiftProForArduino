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

enum GrovePortType
{
	GROVE_PORT_DIGITAL,	
	GROVE_PORT_ANALOG,
	GROVE_PORT_I2C,
	
	GROVE_PORT_TYPE_COUNT
};


enum GroveType
{
	GROVE_TYPE_NONE = 0,
	
	GROVE_COLOR_SENSOR = 10,
	GROVE_GESTURE_SERSOR = 11,
	GROVE_ULTRASONIC	= 12,

	GROVE_FAN = 13,
	GROVE_ELECTROMAGNET = 14,
	GROVE_TEMPERATURE_HUMIDITY_SENSOR = 15,
	GROVE_PIR_MOTION_SENSOR = 16,	
	
	GROVE_TYPE_COUNT
};

void initGroveModule(GroveType type, GrovePortType portType, unsigned char pin);

void setGroveModuleReportInterval(GroveType type, long interval);

void setGroveModuleValue(GroveType type, long value);


#endif // _UARMGROVE_H_
