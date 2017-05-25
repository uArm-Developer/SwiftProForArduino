/**
  ******************************************************************************
  * @file	uArmBuzzer.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2016-09-30
  * @license GNU
  * copyright(c) 2016 UFactory Team. All right reserved
  ******************************************************************************
  */

#include "uArmBuzzer.h" 

uArmBuzzer swift_buzzer;



uArmBuzzer::uArmBuzzer()
{
	mOn = false;
}

void uArmBuzzer::setPin(unsigned char pin)
{
	mPin = pin;
}



void uArmBuzzer::tone(unsigned long duration, unsigned int frequency)
{
	if (duration <= 0)
		return;

	mOn = true;
	pinMode(mPin, OUTPUT);
	mDuration = duration;
	mStartTime = millis();
	::tone(mPin, frequency, duration);

}


void uArmBuzzer::stop()
{
	noTone(mPin);	
}


void uArmBuzzer::tick()
{
	if (mOn && (long)((millis() - mStartTime)) >= mDuration)
	{
		noTone(mPin); 
		mOn = false;
	}
}

bool uArmBuzzer::on()
{
	return mOn;
}