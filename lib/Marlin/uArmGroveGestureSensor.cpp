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


#include "uArmGroveGestureSensor.h"

#include "paj7620.h" 

uArmGroveGestureSensor::uArmGroveGestureSensor()
{

}

bool uArmGroveGestureSensor::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	uint8_t error = 0;
	
	error = paj7620Init();			// initialize Paj7620 registers
	if (error) 
	{
		MYSERIAL.print("INIT ERROR,CODE: ");
		MYSERIAL.println(error);
		return false;
	}

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	return true;
}

void uArmGroveGestureSensor::report()
{
	uint8_t data = 0, data1 = 0, error;
	char result[128];
	int gesture = 255;

	error = paj7620ReadReg(0x43, 1, &data);				// Read Bank_0_Reg_0x43/0x44 for gesture result.
	if (!error) 
	{
		switch (data) 									// When different gestures be detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
		{
			case GES_RIGHT_FLAG:
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					gesture = GES_FORWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					gesture = GES_BACKWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else
				{
					gesture = GES_RIGHT_FLAG;
				}          
				break;
			case GES_LEFT_FLAG: 
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					gesture = GES_FORWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					gesture = GES_BACKWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else
				{
					gesture = GES_LEFT_FLAG;
				}          
				break;
			case GES_UP_FLAG:
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					gesture = GES_FORWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					gesture = GES_BACKWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else
				{
					gesture = GES_UP_FLAG;
				}          
				break;
			case GES_DOWN_FLAG:
				delay(GES_ENTRY_TIME);
				paj7620ReadReg(0x43, 1, &data);
				if(data == GES_FORWARD_FLAG) 
				{
					gesture = GES_FORWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else if(data == GES_BACKWARD_FLAG) 
				{
					gesture = GES_BACKWARD_FLAG;
					delay(GES_QUIT_TIME);
				}
				else
				{
					gesture = GES_DOWN_FLAG;
				}          
				break;
			case GES_FORWARD_FLAG:
				gesture = GES_FORWARD_FLAG;
				delay(GES_QUIT_TIME);
				break;
			case GES_BACKWARD_FLAG:		  
				gesture = GES_BACKWARD_FLAG;
				delay(GES_QUIT_TIME);
				break;
			case GES_CLOCKWISE_FLAG:
				gesture = GES_CLOCKWISE_FLAG;
				break;
			case GES_COUNT_CLOCKWISE_FLAG:
				gesture = GES_COUNT_CLOCKWISE_FLAG;
				break;  
			default:
				paj7620ReadReg(0x44, 1, &data1);
				if (data1 == GES_WAVE_FLAG) 
				{
					gesture = GES_WAVE_FLAG;
				}
				break;
		}
	}

	if (gesture != 255)
	{
		msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, _portNum, GROVE_GESTURE_SERSOR, gesture);
		reportString(result);
	}
}

void uArmGroveGestureSensor::control()
{
	
}

void uArmGroveGestureSensor::tick()
{
}

