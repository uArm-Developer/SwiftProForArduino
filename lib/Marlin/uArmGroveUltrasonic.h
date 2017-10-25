

#ifndef _UARMGROVEULTRASONIC_H_
#define _UARMGROVEULTRASONIC_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Ultrasonic.h"

class uArmGroveUltrasonic : public uArmGroveBase
{
public:
	uArmGroveUltrasonic();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

protected:
	Ultrasonic _ultrasonic;
};


#endif // _UARMGROVEULTRASONIC_H_
