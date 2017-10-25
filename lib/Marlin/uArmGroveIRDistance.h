

#ifndef _UARMGROVEIRDISTANCE_H_
#define _UARMGROVEIRDISTANCE_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"


class uArmGroveIRDistance : public uArmGroveBase
{
public:
	uArmGroveIRDistance();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();


};


#endif // _UARMGROVEIRDISTANCE_H_
