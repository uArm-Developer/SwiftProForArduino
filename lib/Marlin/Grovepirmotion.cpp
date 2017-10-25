
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Grovepirmotion.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Grovepirmotion::Grovepirmotion()
{

}

void Grovepirmotion::setPin(int pin)
{
	_pin = pin;
}

uint8_t Grovepirmotion::getstatus(void)
{
	pinMode(_pin, INPUT);
	return digitalRead(_pin);
}
