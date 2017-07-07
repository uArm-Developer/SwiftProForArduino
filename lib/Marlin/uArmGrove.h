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

enum GroveType
{
	GROVE_TYPE_NONE = 0,
	
	GROVE_COLOR_SENSOR = 10,
	GROVE_GESTURE_SERSOR = 11,
	GROVE_ULTRASONIC	= 12,
	
	GROVE_TYPE_COUNT
};

void initGroveModule(GroveType type);

void setGroveModuleReportInterval(GroveType type, long interval);

#endif // _UARMGROVE_H_
