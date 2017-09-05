

#ifndef _UARMGROVE_TH_H_
#define _UARMGROVE_TH_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Grove_TH02_dev.h"


class uArmGroveTH : public uArmGroveBase
{
public:
	uArmGroveTH();

public:
	bool uArmGroveTH::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

protected:
	TH02_dev grove_TH;


};


#endif // _UARMGROVE_TH_H_
