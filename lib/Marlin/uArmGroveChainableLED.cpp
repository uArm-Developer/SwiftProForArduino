/**
 * uArm Swift Pro Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Marlin.
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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


#include "uArmGroveChainableLED.h"





uArmGroveChainableLED::uArmGroveChainableLED()
{

}

bool uArmGroveChainableLED::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	uint8_t num = 0;
	if (code_seen('V'))
	{
		num = code_value_byte();
		_chLed.setNumber(num );
		debugPrint("chainaleLed %d\r\n", num);
	}
	else
	{
		MYSERIAL.println("E: no number of LEDs provided.");
		return false;
	}

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;	


	debugPrint("chainaleLed setPins %d, %d\r\n", clk_pin, dat_pin);
	_chLed.setPin(clk_pin, dat_pin);


	return true;
}



void uArmGroveChainableLED::report()
{

}

void uArmGroveChainableLED::control()
{
	if (code_seen('V'))
	{
		uint8_t index = code_value_byte();
		uint8_t r = 0, g = 0, b = 0;
		
		if(code_seen('R'))
		{
			r = code_value_byte();
		}
		if(code_seen('G'))
		{
			g = code_value_byte();
		}		
		if(code_seen('B'))
		{
			b = code_value_byte();
		}	

		debugPrint("chainaleLed control %d, %d, %d, %d\r\n", index, r, g, b);
		_chLed.setColorRGB(index, r, g, b);
	}
	else
	{
		// wrong format
	}
}

void uArmGroveChainableLED::tick()
{
}

