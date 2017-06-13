/*
  Ultrasonic.h - Ultrasonic driver Library
  2011 Copyright (c) Seeed Technology Inc.  All right reserved.
  For Ultrasonic v1.0 firmware.

  Original Author: LG
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Grovefan.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Grovefan::Grovefan(int pin)
{
	_pin = pin;
}

void Grovefan::setPin(int pin)
{
	_pin = pin;
}

void Grovefan::off(void)
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	
}
void Grovefan::on(void)
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, HIGH);
}

void Grovefan::speed(uint8_t   speed )
{
	pinMode(_pin, OUTPUT);
	analogWrite(_pin, speed);
}