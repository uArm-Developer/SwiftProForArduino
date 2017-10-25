/**
  ******************************************************************************
  * @file	uArmButton.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2016-10-17
  ******************************************************************************
  */

#ifndef _UARMBUTTON_H_
#define _UARMBUTTON_H_

#include <Arduino.h>

#include "Marlin.h"


#define EVENT_DOWN			0
#define EVENT_CLICK			1
#define EVENT_LONG_PRESS	2

class uArmButton
{

	typedef enum {
		INIT,
		IDLE,
		HALF_PRESSED,
		PRESSED,
		RELEASE,
		LONGPRESSED,

		STATE_COUNT

	} BUTTON_STATE;

public:
	uArmButton();


	bool clicked();
	bool longPressed();

	//void clearEvent();
	void tick();

	bool isPressed();



public:
	typedef void (*ButtonEventCB_t)(void*);
	typedef void (*ButtonLongPressedCB_t)(void*);
	typedef bool (*IsButtonPressedCB_t)(void*);


	void setButtonDownCB(ButtonEventCB_t downCB, void *param);
	void setClickedCB(ButtonEventCB_t clickedCB, void *param);
	void setLongPressedCB(ButtonLongPressedCB_t longPressedCB, void *param);
	void setIsButtonPressedCB(IsButtonPressedCB_t isButtonPressed, void *param);
private:
	ButtonEventCB_t pButtonEventCB;
	ButtonEventCB_t pButtonDownCB;
	ButtonLongPressedCB_t pButtonLongPressedCB;
	IsButtonPressedCB_t pIsButtonPressedCB;


private:
	unsigned char mPin;
	unsigned int mTicks;

	//unsigned char mEvent;

	BUTTON_STATE mState;

	void *_paramIsPressed;
	void *_paramClick;
	void *_paramDown;
	void *_prarmLongPress;
};



#endif // _UARMBUTTON_H_
