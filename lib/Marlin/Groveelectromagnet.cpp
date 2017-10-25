
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Groveelectromagnet.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Groveelectromagnet::Groveelectromagnet()
{
}

void Groveelectromagnet::setPin(int pin)
{
	_pin = pin;
}

void Groveelectromagnet::off(void)
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);
	
}
void Groveelectromagnet::on(void)
{
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, HIGH);
}
