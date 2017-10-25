/*******************************************
/*******************************************
/***@file	uArmGrove6AxisSensor.cpp
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#include "uArmGrove6AxisSensor.h"


#define SPI_CS 10


bool uArmGrove6AxisSensor::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	char result[128];
	char rtn = 0;
    rtn = _uArmGrove6AxisSensor.initI2C();
    //rtn = _uArmGrove6AxisSensor.initSPI(SPI_CS);
    if(rtn != 0)  // Initialize the LSM303, using a SCALE full-scale range
	{
		msprintf(result, "LSM303D is not found\r\n");
		reportString(result);
		return false;
	}
	else
	{
		msprintf(result, "LSM303D is found\r\n");
		reportString(result);
	}

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	return true;
}

void uArmGrove6AxisSensor::report()
{
	char result[128];

	_uArmGrove6AxisSensor.getAccel(_accel);
	while(!_uArmGrove6AxisSensor.isMagReady());			// wait for the magnetometer readings to be ready
	_uArmGrove6AxisSensor.getMag(_mag);		// get the magnetometer values, store them in _mag

	for (int i=0; i<3; i++)
	{
		_realAccel[i] = _accel[i] / pow(2, 15) * ACCELE_SCALE;  // calculate real acceleration values, in units of g
	}
	_heading = _uArmGrove6AxisSensor.getHeading(_mag);
	_titleHeading = _uArmGrove6AxisSensor.getTiltHeading(_mag, _realAccel);

	//debugPrint("realAccel %f %f %f\r\n", _realAccel[0], _realAccel[1], _realAccel[2]);
	//debugPrint("heading %f titleHeading %f\r\n", _heading, _titleHeading);

	msprintf(result, "@%d P%d N%d X%f Y%f Z%f H%f T%f\r\n",
			REPORT_TYPE_GROVE2, _portNum, GROVE_6AXIS_SENSOR,
			_realAccel[0], _realAccel[1], _realAccel[2],
			_heading, _titleHeading);

	reportString(result);
}

void uArmGrove6AxisSensor::control()
{
	
}

void uArmGrove6AxisSensor::tick()
{
}

