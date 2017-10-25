/*******************************************
/*******************************************
/***@file	uArmGroveVibrationMotor.h
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170905
/*******************************************
*******************************************/

#ifndef _UARMGROVEVIBRATIONMOTOR_H_
#define _UARMGROVEVIBRATIONMOTOR_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "GroveControl.h"


class uArmGroveVibrationMotor : public uArmGroveBase
{
public:
	uArmGroveVibrationMotor();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	void report();
	void control();
	void tick();

private:
	GroveControl _uArmGroveVibrationMotor;

};


#endif // _UARMGROVEVIBRATIONMOTOR_H_
