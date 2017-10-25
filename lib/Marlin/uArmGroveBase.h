

#ifndef _UARMGROVEBASE_H_
#define _UARMGROVEBASE_H_

#include <Arduino.h>
#include "Marlin.h"

extern uint8_t code_value_byte();


class uArmGroveBase
{
public:
	uArmGroveBase(){}

public:
	virtual bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);
	virtual void report();
	virtual void control();
	virtual void tick();

protected:
	uint8_t _portNum, _clk_pin, _dat_pin;
		
};

extern float code_value_float();
extern uint8_t code_value_byte();
extern uint16_t code_value_ushort();
extern void code_value_string(char * buf, uint16_t buf_len);


#endif // _UARMGROVEBASE_H_
