/**
  ******************************************************************************
  * @file	uArmLed.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2016-10-17
  ******************************************************************************
  */

#ifndef _UARMLED_H_
#define _UARMLED_H_

#include <Arduino.h>
#include "Marlin.h"

class uArmLed
{
	typedef enum 
	{
		INIT,
		OFF,
		ON,
		BLINK_OFF,
		BLINK_ON,

		STATE_COUNT
		
	} LEDState;

public:
	uArmLed();

	// polarity = 1: HIGH=led on
	// polarity = 0: LOW =led on
	void setPin(unsigned char pin, unsigned char polarity = 0);

	void on(unsigned char bright = 255);

	void off();
	void blink(unsigned int period = 500, unsigned char bright = 255);



	void tick();

private:
	void tickInc();

private:
	unsigned char mPin;
	unsigned int mPeriod; // ms
	LEDState mState;
	unsigned int mTicks;
	unsigned char mBright;
	unsigned char mPolarity;
};

#endif // _UARMLED_H_
