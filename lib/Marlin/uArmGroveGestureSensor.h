

#ifndef _UARMGROVEGESTURESENSOR_H_
#define _UARMGROVEGESTURESENSOR_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"


class uArmGroveGestureSensor : public uArmGroveBase
{
public:
	uArmGroveGestureSensor();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	void report();
	void control();
	void tick();

};


#endif // _UARMGROVEGESTURESENSOR_H_
