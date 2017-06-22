/**
  ******************************************************************************
  * @file	uArmServo.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-06-13
  ******************************************************************************
  */

#include "uArmServo.h" 
#include "servo.h"

#define SERVO_VALUE_DIFF	1

#define SERVO_STATE_IDLE 0
#define SERVO_STATE_WAIT 1
#define SERVO_STATE_WRITE 2

#define SERVO_WAIT_TICKS	4

static int servo_state = SERVO_STATE_IDLE;

extern Servo servo[NUM_SERVOS];

void _servo_write(double value)
{
	servo[0].attach(SERVO0_PIN);
	servo[0].write(value);
	servo_state = SERVO_STATE_WRITE;	
}

extern uint16_t get_current_angle_adc(uint8_t index);
void servo_write(double value, bool remember = false)
{
	static double last_value = -10;
	static double last_write_value = get_current_angle_adc(3);

	if (!remember)
	{
		_servo_write(value);
		last_write_value = value;
	}
	else 
	{
		if (abs(last_value - value) < 0.0001 && abs(value - last_write_value) > SERVO_VALUE_DIFF)
		{	
			_servo_write(value);
			last_write_value = value;
		}

		last_value = value;
	}

	
}

void servo_tick()
{
	static int servo_tick_count = 0;

	switch (servo_state)
	{
	case SERVO_STATE_WRITE:
		servo_tick_count = 0;
		servo_state = SERVO_STATE_WAIT;
		break;

	case SERVO_STATE_WAIT:
		servo_tick_count++;

		if (servo_tick_count >= SERVO_WAIT_TICKS)
		{
			servo_tick_count = 0;
			servo[0].detach();
			servo_state = SERVO_STATE_IDLE;
		}
		break;
	
	case SERVO_STATE_IDLE:
	default:
		// do nothing
		break;
	}
}



