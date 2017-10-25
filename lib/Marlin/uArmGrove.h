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

#define LCD_TEXT_LEN 30

enum GrovePortType
{
	GROVE_PORT_DIGITAL,	
	GROVE_PORT_ANALOG,
	GROVE_PORT_I2C,
	
	GROVE_PORT_TYPE_COUNT
};


enum GroveType
{
	GROVE_TYPE_NONE 					= 0,

	GROVE_CHAINABLE_LED 				= 1,
	GROVE_BUTTON 						= 2,
	GROVE_SLIDING_POTENTIOMETER 		= 3,
	GROVE_VIBRATION_MOTOR 				= 4,
	GROVE_LIGHT_SENSOR 					= 5,
	GROVE_ANGLE_SENSOR 					= 6,
	GROVE_AIR_QUALITY_SENSOR			= 7,
	GROVE_SOUND_SENSOR					= 8,
	GROVE_6AXIS_SENSOR					= 9,
	GROVE_COLOR_SENSOR 					= 10,
	GROVE_GESTURE_SERSOR 				= 11,
	GROVE_ULTRASONIC					= 12,
	GROVE_FAN 							= 13,
	GROVE_ELECTROMAGNET 				= 14,
	GROVE_TEMPERATURE_HUMIDITY_SENSOR 	= 15,
	GROVE_PIR_MOTION_SENSOR 			= 16,
	GROVE_RGBLCD 						= 17,
	GROVE_LINE_FINDER 					= 18,
	GROVE_IR_DISTANCE	 				= 19,
	GROVE_EMG_DETECTOR					= 20,
	GROVE_OLED12864						= 21,
	//GROVE_SERVO							= 22,

	GROVE_TYPE_COUNT
};


enum GrovelcdcmdType
{
	GROVE_LCD_TYPE_NODISPLAY ,
	GROVE_LCD_TYPE_DISPLAY ,
	GROVE_LCD_TYPE_CLEAR ,

	GROVE_LCD_TYPE_COUNT
};

enum GrovelcdstringType
{
	GROVE_CMD_TYPE_DISPLAYROW1 ,
	GROVE_CMD_TYPE_DISPLAYROW2 ,
	GROVE_CMD_TYPE_COLOR ,


	GROVE_CMD_TYPE_COUNT
};

void initGroveModule(GroveType type, GrovePortType portType, unsigned char pin);

void setGroveModuleReportInterval(GroveType type, long interval);

void setGroveModuleValue(GroveType type, long value);

void setGroveLCDModuleValue(GroveType type,GrovelcdcmdType cmd,long value);

void setGroveLCDModuleString(GroveType type,GrovelcdstringType cmd,char string[]);

void setGroveLCDModuleRGB(GroveType type,GrovelcdcmdType cmd,long value);


#endif // _UARMGROVE_H_
