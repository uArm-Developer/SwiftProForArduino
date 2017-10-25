/*******************************************
/*******************************************
/***@file	uArmGrove6AxisSensor.h
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#ifndef _UARMGROVE_6AXIS_SENSOR_H_
#define _UARMGROVE_6AXIS_SENSOR_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Accelerometer_Compass_LSM303D.h"


class uArmGrove6AxisSensor : public uArmGroveBase
{
public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	void report();
	void control();
	void tick();

protected:	
	LSM303D _uArmGrove6AxisSensor;
	int 	_accel[3];				// we'll store the raw acceleration values here
	int 	_mag[3];					// raw magnetometer values stored here
	float 	_realAccel[3];			// calculated acceleration values here
	float 	_heading, _titleHeading;

};


#endif // _UARMGROVE_6AXIS_SENSOR_H_
