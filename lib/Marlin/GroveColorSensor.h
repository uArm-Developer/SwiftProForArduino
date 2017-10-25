/****************************************************************************/	
//	Hardware: Grove - I2C Color Sensor
//  Arduino IDE: Arduino-1.6
//  
//  Refactored version of the library by FrankieChu - www.seeedstudio.com
//	
/******************************************************************************/

#ifndef GROVECOLORSENSOR
#define GROVECOLORSENSOR


#include <Arduino.h>

#include "GroveRegisters.h"

class GroveColorSensor
{
public:

	// Color Sensor LED Status
	int ledStatus;
	// Default constructor
	GroveColorSensor();

	void init();
	// Constructor with parameters
	GroveColorSensor(
		  const int& triggerMode
		, const int& interruptSource
		, const int& interruptMode
		, const int& gainAndPrescaler
		, const int& sensorAddress);

	void readRGB();
	void readRGB(int *red, int *green, int *blue);
	void calculateCoordinate();
	void clearInterrupt();
	
private:	 
	 
	// Set trigger mode. Including free mode, manually mode, single synchronization mode or so.
	void setTimingReg();
	// Set interrupt source
	void setInterruptSourceReg();
	// Set interrupt mode
	void setInterruptControlReg();
	// Set gain value and pre-scaler value
	void setGain();
	// Start ADC of the colour sensor
	void setEnableADC();
	
	// Used for storing the colour data
	int readingdata_[8];
	int green_;
	int red_;
	int blue_;
	int clear_;
	
	int triggerMode_;	
	int interruptSource_;
	int interruptMode_;
	int gainAndPrescaler_;
	int sensorAddress_;
	
};

#endif