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


#include "uArmGroveLCD1602.h"


uArmGroveLCD1602::uArmGroveLCD1602()
{

}

bool uArmGroveLCD1602::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;	

	_grovergb_lcd.begin(16,2);

	return true;
}

void uArmGroveLCD1602::report()
{
}


void uArmGroveLCD1602::control()
{
	uint8_t type = 0;
	uint8_t cmdtype = 0;
	long redvalue = 0;
	long greenvalue = 0;
	long bluevalue = 0;

	long value = 0;	

	char stringDisplay[17] = {0};	


	if(code_seen('V'))
	{
		cmdtype = code_value_byte();

		if (code_seen('S'))
		{
			code_value_string(stringDisplay, LCD_TEXT_LEN);
		}	
		
		switch (cmdtype) {
				
			case GROVE_CMD_TYPE_DISPLAYROW1:
			case GROVE_CMD_TYPE_DISPLAYROW2:
				debugPrint("lcdtext:%s\r\n",stringDisplay);
				//clear the row text
				for(int i=0;i<16;i++)
				{
			       	_grovergb_lcd.setCursor(i,cmdtype);	
					_grovergb_lcd.write(" ");
			    }
				
			    delay(10);	
				
			    //display the text
				for(int i=0;i<strlen(stringDisplay);i++)
				{
			       	_grovergb_lcd.setCursor(i,cmdtype);	
					_grovergb_lcd.write(stringDisplay[i]);
			    }
				
				break;													

		default:
			break;
			
		}	
	}
	else if(code_seen('T'))
	{
		cmdtype = code_value_byte();
		

		
		switch (cmdtype) {			
										
			case GROVE_LCD_TYPE_NODISPLAY:
				_grovergb_lcd.noDisplay();
				break;	
			case GROVE_LCD_TYPE_DISPLAY:
				_grovergb_lcd.display();
				break;				
			case GROVE_LCD_TYPE_CLEAR:
				_grovergb_lcd.clear();
				break;								
		
		default:
			break;
			
		}

	}
	else if(code_seen('R') || code_seen('G') || code_seen('B'))
	{
		if(code_seen('R'))
		{
			redvalue = code_value_byte();
		}
		if(code_seen('G'))
		{
			greenvalue = code_value_byte();
		}		
		if(code_seen('B'))
		{
			bluevalue = code_value_byte();
		}
		
		
		_grovergb_lcd.setRGB(redvalue, greenvalue, bluevalue);		
	}
	else
	{
		// format wrong
	}	
}

void uArmGroveLCD1602::tick()
{
}

