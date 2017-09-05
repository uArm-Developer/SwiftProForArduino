

#ifndef _UARMGROVECOLORSENSOR_H_
#define _UARMGROVECOLORSENSOR_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Adafruit_TCS34725.h"


class uArmGroveColorSensor : public uArmGroveBase
{
public:
	uArmGroveColorSensor();
	~uArmGroveColorSensor();
	void getRawData_noDelay(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c);

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	void report();
	void control();
	void tick();

protected:	
	Adafruit_TCS34725* _tcs;
};


#endif // _UARMGROVECOLORSENSOR_H_
