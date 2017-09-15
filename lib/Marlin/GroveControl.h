/*******************************************
/*******************************************
/***@file	GroveControl.h
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170905
/*******************************************
*******************************************/

#ifndef _GROVECONTROL_H_
#define _GROVECONTROL_H_

class GroveControl
{
	public:
		GroveControl();

		void setPin(int pin);
		void on(void);
		void off(void);
		void speed(uint8_t speed);
		
	private:
		int _pin;
};

#endif	//  _GROVECONTROL_H_
