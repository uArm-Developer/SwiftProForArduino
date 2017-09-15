/*******************************************
/*******************************************
/***@file	uArmGroveOLED12864.h
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#ifndef _UARM_GROVE_OLED12864_H_
#define _UARM_GROVE_OLED12864_H_

#include <Arduino.h>
#include "Marlin.h"
#include "uArmGroveBase.h"
#include "SeeedOLED.h"

#define StrLen 32

class uArmGroveOLED12864 : public uArmGroveBase
{
public:
	uArmGroveOLED12864();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

private:
	
	uint8_t 	_value;
	uint8_t 	_type;
	uint8_t 	_x;
	uint8_t 	_y;
	uint8_t 	_width;
	uint8_t 	_pixel_value;
};

#endif // _UARM_GROVE_OLED12864_H_
