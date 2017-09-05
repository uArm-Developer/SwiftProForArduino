

#ifndef _UARMGROVE_BUTTON_H_
#define _UARMGROVE_BUTTON_H_

#include <Arduino.h>
#include "Marlin.h"


#include "uArmGroveBase.h"


class uArmGroveButton : public uArmGroveBase
{
public:
	uArmGroveButton();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	void report();
	void control();
	void tick();

private:
	uArmButton _button;
	uint8_t _buttonPin;

};


#endif // _UARMGROVE_BUTTON_H_