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


#include "uArmGroveSlidingPotentiometer.h"


uArmGroveSlidingPotentiometer::uArmGroveSlidingPotentiometer()
{

}

bool uArmGroveSlidingPotentiometer::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	pinMode(_dat_pin, OUTPUT);

	debugPrint("clk = %d, dat = %d\r\n", clk_pin, dat_pin);

	return true;

}



void uArmGroveSlidingPotentiometer::report()
{
	char result[128];

	uint16_t value = analogRead(_clk_pin);
	debugPrint("value=%d\r\n", value);

	msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, _portNum, GROVE_SLIDING_POTENTIOMETER, value);
	reportString(result);

}

void uArmGroveSlidingPotentiometer::control()
{
	if (code_seen('V'))
	{
		uint8_t value = code_value_byte();

		if (value)
		{
			digitalWrite(_dat_pin, HIGH);
		}
		else
		{
			digitalWrite(_dat_pin, LOW);
		}
	}
}

void uArmGroveSlidingPotentiometer::tick()
{
}


