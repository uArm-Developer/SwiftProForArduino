

#ifndef _UARMGROVE_SLIDING_POTENTIOMETER_H_
#define _UARMGROVE_SLIDING_POTENTIOMETER_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"


class uArmGroveSlidingPotentiometer : public uArmGroveBase
{
public:
	uArmGroveSlidingPotentiometer();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

private:

};


#endif // _UARMGROVE_SLIDING_POTENTIOMETER_H_