/*******************************************
/*******************************************
/***@file	GroveControl.cpp
/***@author	LUYU	
/***@email	yu.lu@ufactory.cc
/***@date	20170905
/*******************************************
*******************************************/

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "GroveControl.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

GroveControl::GroveControl()
{
}

void GroveControl::setPin(int pin)
{
	_pin = pin;
}

void GroveControl::off(void)
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
}
void GroveControl::on(void)
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, HIGH);
}

void GroveControl::speed(uint8_t speed)
{
	pinMode(_pin, OUTPUT);
	analogWrite(_pin, speed);
}

