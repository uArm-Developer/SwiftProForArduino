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






uArmGroveOLED12864::uArmGroveOLED12864()
{
}

bool uArmGroveOLED12864::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	
	_OLED12864.init();
	_OLED12864.clearDisplay();


	_OLED12864.setInverseDisplay();	   //Set display to normal mode (i.e non-inverse mode)
	_OLED12864.setHorizontalMode(); 	   //Set addressing mode to Page Mode



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
	uint8_t data[128] = {0};
	uint8_t originData[32] = {0};
	uint8_t row = 0;
	uint8_t col = 0;
	uint8_t src[4] = {0};




	if (code_seen('V'))
	{
		row = code_value_byte();
		if (row >= 8)
			return;

	}
	else
	{
		_OLED12864.clearDisplay();
		return;
	}

	_OLED12864.setTextXY(row, 0);		   //Set the cursor to Xth Page, Yth Column  
	

	if (code_seen('S'))
	{
		code_value_string(data, 64);
	    for (uint8_t i = 0; i < strlen(data)/2; i++)
	    {
	        char high = toupper(data[2 * i]) - 0x30;
	        if (high > 9)
	        	high -= 7;

			if (high > 15) high = 15;

	        char low = toupper(data[2 * i + 1]) - 0x30;
	        if (low > 9)
	        	low -= 7;
			if (low > 15) low = 15;

	        originData[i] = high * 16 + low;

	        debugPrint("originData[%d]=%d\r\n", i, originData[i]);
	    }
	}	
	else
	{
		return;
	}


	for (int i = 0; i < 128; i++)
		data[i] = 0;

	// map originData to screen 1px -> 4px
	for (int i = 0; i < 8; i++)
	{
		src[0] = originData[0 + i];
		src[1] = originData[8 + i];
		src[2] = originData[16 + i];
		src[3] = originData[24 + i];

		for (int j = 0; j < 8; j++)
		{
			uint8_t index = 16*i+2*j;
			uint8_t bit = (7 - j);

			for (int k = 0; k < 4; k++)
			{
				if (src[k] & (1 << bit))
				{
					data[index] |= (3 << (k*2));
					data[index+1] |= (3 << (k*2));
				}
			}
		}
	}
	
	
	_OLED12864.drawData(data, 128);

	
}

void uArmGroveOLED12864::tick()
{
}

