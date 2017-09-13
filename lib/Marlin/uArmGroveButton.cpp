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


#include "uArmGroveButton.h"





void groveButtonClicked(void* port)
{
	uint8_t result[128] = {0};

	uint8_t portNum = (uint8_t)port;
	
	debugPrint("groveButtonClicked\r\n");
	
	msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, portNum, GROVE_BUTTON, EVENT_CLICK);
	reportString(result);
	
	
}

void groveButtonLongPressed(void* port)
{
	uint8_t result[128] = {0};

	uint8_t portNum = (uint8_t)port;
	
	debugPrint("groveButtonLongPressed\r\n");
	
	msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, portNum, GROVE_BUTTON, EVENT_LONG_PRESS);
	reportString(result);

}


bool groveButtonDown(void* port)
{
	uint8_t result[128] = {0};

	uint8_t portNum = (uint8_t)port;
	
	debugPrint("groveButtonDown\r\n");
	
	msprintf(result, "@%d P%d N%d V%d\r\n", REPORT_TYPE_GROVE2, portNum, GROVE_BUTTON, EVENT_DOWN);
	reportString(result);	
}

bool IsGroveButtonPressed(void* pin)
{

	uint8_t pinNum = (uint8_t)pin;

	if (pinNum == 0xff)
		return false;

	if (digitalRead(pinNum))
	{
		return true;
	}
	else
	{
		
		return false;
	}
}


uArmGroveButton::uArmGroveButton()
{
	_buttonPin = 0xff;
}

bool uArmGroveButton::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	debugPrint("button setPins %d\r\n", clk_pin);
	_buttonPin = clk_pin;
	pinMode(_buttonPin, INPUT);
	_button.setIsButtonPressedCB(IsGroveButtonPressed, (void*)_buttonPin);
	_button.setClickedCB(groveButtonClicked, (void*)_portNum);
	_button.setLongPressedCB(groveButtonLongPressed, (void*)_portNum);
	_button.setButtonDownCB(groveButtonDown, (void*)_portNum);


}



void uArmGroveButton::report()
{


}

void uArmGroveButton::control()
{
	
}

void uArmGroveButton::tick()
{
	_button.tick();
}


