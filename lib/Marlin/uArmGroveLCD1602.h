

#ifndef _UARMGROVE_LCD1602_H_
#define _UARMGROVE_LCD1602_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Grovergb_lcd.h"


class uArmGroveLCD1602 : public uArmGroveBase
{
public:
	uArmGroveLCD1602();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

};


#endif // _UARMGROVE_LCD1602_H_
