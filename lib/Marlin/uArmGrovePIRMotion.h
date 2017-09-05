

#ifndef _UARMGROVE_PIR_MOTION_H_
#define _UARMGROVE_PIR_MOTION_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Grovepirmotion.h"


class uArmGrovePIRMotion : public uArmGroveBase
{
public:
	uArmGrovePIRMotion();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

protected:
	Grovepirmotion _grovepirmotion;

};


#endif // _UARMGROVE_PIR_MOTION_H_
