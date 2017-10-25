/**
  ******************************************************************************
  * @file	uArmButton.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2016-10-17
  ******************************************************************************
  */

#include "uArmButton.h" 



uArmButton::uArmButton()
{
	mPin = 0xff;
	mState = IDLE;
//	mEvent = EVENT_NONE;


	pButtonEventCB = NULL;
	pButtonLongPressedCB = NULL;
	pIsButtonPressedCB = NULL;
	pButtonDownCB = NULL;

	_paramIsPressed = NULL;
	_paramClick = NULL;
	_prarmLongPress = NULL;	
	_paramDown = NULL;
}


void uArmButton::setClickedCB(ButtonEventCB_t clickedCB, void *param)
{
	pButtonEventCB = clickedCB;
	_paramClick = param;
}

void uArmButton::setLongPressedCB(ButtonLongPressedCB_t longPressedCB, void *param)
{
	pButtonLongPressedCB = longPressedCB;
	_prarmLongPress = param;
}

void uArmButton::setIsButtonPressedCB(IsButtonPressedCB_t isButtonPressed, void *param)
{
	pIsButtonPressedCB = isButtonPressed;
	_paramIsPressed = param;
}


/*
bool uArmButton::clicked()
{
	return (mEvent == EVENT_CLICK);
}


bool uArmButton::longPressed()
{
	return (mEvent == EVENT_LONG_PRESS);
}




void uArmButton::clearEvent()
{
	mEvent = EVENT_NONE;
}
*/


bool uArmButton::isPressed()
{

	if (pIsButtonPressedCB == NULL)
		return false;

	if (pIsButtonPressedCB(_paramIsPressed))
		return true;

	return false;
}

void uArmButton::setButtonDownCB(ButtonEventCB_t downCB, void *param)
{
	_paramDown = param;
	pButtonDownCB = downCB;
}

void uArmButton::tick()
{
	switch (mState)
	{
	case IDLE:
		if (isPressed())
		{
			mTicks = 0;
			mState = HALF_PRESSED;
		}
		break;

	case HALF_PRESSED:
		if (isPressed())
		{
			swift_buzzer.tone(100, 4000);
			
			if (pButtonDownCB != NULL)
			{
				pButtonDownCB(_paramDown);
			}

			mState = PRESSED;
		}
		else
		{
			mState = IDLE;
		}	
		break;

	case PRESSED:
		if (isPressed())
		{

			mTicks++;
		}
		else
		{
			mState = RELEASE;
		}		
		break;

	case RELEASE:

		if (mTicks >= (1000/TICK_INTERVAL))
		{
			//mEvent = EVENT_LONG_PRESS;
			
			if (pButtonLongPressedCB != NULL)
			{
				pButtonLongPressedCB(_prarmLongPress);
			}
			
		}
		else
		{
			//mEvent = EVENT_CLICK;
			
			if (pButtonEventCB != NULL)
			{
				pButtonEventCB(_paramClick);
			}
					
		}

		mTicks = 0;
		mState = IDLE;
		break;

	default:
		break;

	}
}