/**
 * uArm Swift Pro Firmware
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "uArmGroveServo.h"



uArmGroveServo::uArmGroveServo()
{
	_servo_index = 0;
	_write_state = 0;
	_wait_count = 0;	

}

bool uArmGroveServo::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;	

	if (code_seen('V'))
	{
		uint8_t index = code_value_byte();

		if (index < 1 || index > 2)
			return false;

		_servo_index = index;
	}
	else
	{
		_servo_index = 1;
	}

	pinMode(_dat_pin, OUTPUT);
	servo[_servo_index].attach(_dat_pin, 600, 2400);
	servo[_servo_index].detach();

	return true;	
}

void uArmGroveServo::report()
{
	
}

void uArmGroveServo::control()
{
	uint16_t angle;		

	debugPrint("uArmGroveServo control\r\n", angle);

	
	if (code_seen('V'))
	{
		angle = code_value_ushort();	

		debugPrint("uArmGroveServo angle is %d\r\n", angle);

		servo[_servo_index].attach(_dat_pin);
		servo[_servo_index].write(angle);
		_wait_count = 0;
		_write_state = 1;
		
	}	
	
}

void uArmGroveServo::tick()
{
	if (_write_state)
	{
		_wait_count++;
		if (_wait_count > 20)
		{
			_wait_count = 0;
			_write_state = 0;
			servo[_servo_index].detach();		
		}
	}
}


