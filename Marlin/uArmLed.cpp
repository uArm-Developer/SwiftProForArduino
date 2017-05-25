/**
  ******************************************************************************
  * @file	uArmLed.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2016-10-17
  ******************************************************************************
  */

#include "uArmLed.h" 

uArmLed::uArmLed()
{
	mPin = 0xff;
	mState = INIT;
	mTicks = 0;
}

void uArmLed::setPin(unsigned char pin, unsigned char polarity)
{
	mPin = pin;
	mState = OFF;
	mTicks = 0;
	mPeriod = 500;
	mBright = 255;
	mPolarity = polarity;
}

void uArmLed::on(unsigned char bright)
{
	//digitalWrite(mPin, LOW); 
	if (mPolarity)
	{
		analogWrite(mPin, bright);
	}
	else
	{
		analogWrite(mPin, 255-bright);
	}
	mState = ON;
}

void uArmLed::off()
{
	//digitalWrite(mPin, HIGH); 
	if (mPolarity)
	{
		analogWrite(mPin, 0);
	}
	else
	{
		analogWrite(mPin, 255);
	}
	mState = OFF;
	mTicks = 0;
}



void uArmLed::blink(unsigned int period, unsigned char bright)
{
	if (mPeriod <= 10)
		return ;

	on(bright); 
	mState = BLINK_ON;
	mTicks = 1;
	mPeriod = period;
	mBright = bright;
}

void uArmLed::tickInc()
{
	mTicks++;

	if (mTicks >= (mPeriod/TICK_INTERVAL))
	{
		mTicks = 0;

		if (mState == BLINK_ON)
		{	
			if (mPolarity)
			{
				analogWrite(mPin, mBright); 
			}
			else
			{
				analogWrite(mPin, 255 - mBright); 
			}
			mState = BLINK_OFF;
		}
		else
		{
			if (mPolarity)
			{
				analogWrite(mPin, 0); 
			}
			else
			{
				analogWrite(mPin, 255); 
			}
			mState = BLINK_ON;
		}
	}
}

void uArmLed::tick()
{
	switch (mState)
	{	
	case BLINK_OFF:
	case BLINK_ON:
		tickInc();
		break;

	default:
		break;

	}
}