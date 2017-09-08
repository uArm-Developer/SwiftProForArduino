/**
  ******************************************************************************
  * @file	uArmPump.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-05-25
  ******************************************************************************
  */

#ifndef _UARMPUMP_H_
#define _UARMPUMP_H_

#include <Arduino.h>
#include "Marlin.h"

typedef enum _pump_state_t
{
	PUMP_STATE_OFF,
	PUMP_STATE_ON,
	PUMP_STATE_VALVE_ON,
	PUMP_STATE_VALVE_OFF,

	
	PUMP_STATE_COUNT
} pump_state_t;

void pump_tick();
void pump_set_state(pump_state_t state);
pump_state_t pump_get_state();


#endif // _UARMPUMP_H_
