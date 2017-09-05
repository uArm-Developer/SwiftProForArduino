

#ifndef _UARMGROVECHAINABLELED_H_
#define _UARMGROVECHAINABLELED_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "ChainableLED.h"


class uArmGroveChainableLED : public uArmGroveBase
{
public:
	uArmGroveChainableLED();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

private:
	ChainableLED _chLed;
};


#endif // _UARMGROVECHAINABLELED_H_
