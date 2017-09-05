
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Grovefan.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

Grovefan::Grovefan()
{

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
