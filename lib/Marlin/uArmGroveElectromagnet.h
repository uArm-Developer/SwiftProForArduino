

#ifndef _UARMGROVE_ELECTROMAGNET_H_
#define _UARMGROVE_ELECTROMAGNET_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Groveelectromagnet.h"


class uArmGroveElectromagnet : public uArmGroveBase
{
public:
	uArmGroveElectromagnet();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

private:
	Groveelectromagnet _groveelectromagnet;

};


#endif // _UARMGROVE_ELECTROMAGNET_H_
