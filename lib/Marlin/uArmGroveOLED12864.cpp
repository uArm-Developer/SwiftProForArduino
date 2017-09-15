/*******************************************
/*******************************************
/***@file	uArmGroveOLED12864.cpp
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#include "uArmGroveOLED12864.h"
#include "macros.h"
#include <ctype.h>



SeeedOLED 	_OLED12864;


uArmGroveOLED12864::uArmGroveOLED12864()
{
}

bool uArmGroveOLED12864::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	
	_OLED12864.init();
	_OLED12864.clearDisplay();


	_x 		= 0;
	_y 		= 0;
	_width 	= 0;

	_OLED12864.clearDisplay();
	_OLED12864.setInverseDisplay();	   //Set display to normal mode (i.e non-inverse mode)



	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;

	debugPrint("OLED12864 init OK\r\n");

	return true;
}

void uArmGroveOLED12864::report()
{
}

void uArmGroveOLED12864::control()
{
	/*
	char result[128];
	char string[64] = {0};		//gcode string
	uint8_t source[32] = {0};
	uint8_t display[128] = {0};
	uint8_t row = 0xFF;
	uint8_t temp2[16][8] = {0};
	*/
	
	_OLED12864.clearDisplay();
	_OLED12864.setInverseDisplay();	   //Set display to normal mode (i.e non-inverse mode)
	_OLED12864.setPageMode();		   //Set addressing mode to Page Mode
	_OLED12864.setTextXY(0,0);		   //Set the cursor to Xth Page, Yth Column  
	_OLED12864.putString("Hello World222!"); //Print the String
	return;

/*
	if (code_seen('V'))
	{
		row = code_value_byte();
		_OLED12864.setTextXY(8 * row,0);
	}

	if (code_seen('S'))
	{
		code_value_string(string, 64);
	    for (uint8_t i = 0; i < 32; i++)
	    {
	        char high = toupper(string[2 * i]) - 0x30;
	        if (high > 9)
	        	high -= 7;

	        char low = toupper(string[2 * i + 1]) - 0x30;
	        if (low > 9)
	        	low -= 7;

	        source[i] = high * 16 + low;

	        debugPrint("source[%d]=%x\r\n", i, source[i]);
	    }
	}
	else
	{
		MYSERIAL.println("Error.");
		return false;
	}

	for (uint8_t i = 0; i < 4; i++)
	{
		for (uint8_t j = 0; j < 8; j++)
		{
			uint8_t temp1[8][4] = {0};

			temp1[j][i] = source[8*i + j];
			temp2[2*j][2*i] = temp1[j][i];
			temp2[2*j + 1][2*i + 1] = temp1[j][i];
		}
	}

	for (uint8_t i = 0; i < 8; i++)
	{
		for (uint8_t j = 0; j < 16; j++)
		{
			display[16*i + j] = temp2[j][i];

	        debugPrint("display[%d]=%x\r\n", i, display[16*i + j]);
		}
	}

	_OLED12864.drawBitmap(display, sizeof(display));

	msprintf(result, "@%d P%d N%d V%d OK\r\n", REPORT_TYPE_GROVE2, _portNum, GROVE_ULTRASONIC);
	reportString(result);

	debugPrint("OLED12864 ctrl OK\r\n");
*/
}

void uArmGroveOLED12864::tick()
{
}

