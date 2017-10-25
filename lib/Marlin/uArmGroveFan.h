

#ifndef _UARMGROVEFAN_H_
#define _UARMGROVEFAN_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "Grovefan.h"


class uArmGroveFan : public uArmGroveBase
{
public:
	uArmGroveFan();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	void report();
	void control();
	void tick();

private:
	//Grovefan _grovefan;

};


#endif // _UARMGROVEFAN_H_
