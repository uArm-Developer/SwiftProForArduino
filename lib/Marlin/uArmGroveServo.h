

#ifndef _UARMGROVESREVO_H_
#define _UARMGROVESREVO_H_

#include <Arduino.h>
#include "Marlin.h"

#include "servo.h"
#include "uArmGroveBase.h"

extern Servo servo[];

class uArmGroveServo : public uArmGroveBase
{
public:
	uArmGroveServo();

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	void report();
	void control();
	void tick();

private:
	uint8_t _servo_index;
	uint8_t _write_state;
	uint16_t _wait_count;
};


#endif // _UARMGROVESREVO_H_