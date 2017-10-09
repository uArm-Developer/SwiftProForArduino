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
	_mode = MODE_16_8;
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


	if (code_seen('M'))
	{
		uint8_t mode = code_value_byte();

		if (mode < MODE_COUNT)
		{
			if (_mode != mode)
			{
				_mode = mode;
			}
		}
	}


	if (code_seen('V'))
	{
		row = code_value_byte();
		if (row >= 8)
			return;

	}
	else
	{
		// clear screen
		_OLED12864.clearDisplay();
		return;
	}


	

	if (code_seen('S'))
	{
		code_value_string(data, 64);
	    for (uint8_t i = 0; i < strlen(data)/2; i++)
	    {
	        uint8_t high = toupper(data[2 * i]) - 0x30;
	        if (high > 9)
	        	high -= 7;

			if (high > 15) high = 15;

	        uint8_t low = toupper(data[2 * i + 1]) - 0x30;
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



	switch (_mode)
	{
	case MODE_16_8:
	{
		// map originData to screen 1px -> 64px 
		for (int i = 0; i < 8; i++)
		{
			memset(data, 0, 128);
			_OLED12864.setTextXY(i, 0);

			for (int j = 0; j < 128; j++)
			{
				uint8_t bit = 7 - (j % 64 / 8);
				if (originData[2*i+j/64] & (1 << bit))
				{
					data[j] = 0xff;
				}
				else
				{
					data[j] = 0;
				}
			}
		
			
			_OLED12864.drawData(data, 128);
		}
		break;
	}

	case MODE_32_16:
	{
		if (row >= 2) return;

		uint8_t startRow = row * 4;

		// map originData to screen 1px -> 16px

		for (int i = 0; i < 4; i++)
		{
			memset(data, 0, 128);
			_OLED12864.setTextXY(startRow+i, 0);
			
			for (int j = 0; j < 4; j++)
			{
				src[0] = originData[i*8 + j];
				src[1] = originData[i*8 + 4 + j]; 

				uint8_t start_index = 32 * j;
				
				for (int k = 0; k < 8; k++)
				{
					uint8_t bit = 7 - k;
					uint8_t high_data = (src[1] & ( 1 << bit)) ? 0xf : 0;
					uint8_t low_data = (src[0] & ( 1 << bit)) ? 0xf : 0;
					
					data[start_index+4*k] = (high_data << 4) | low_data;
					data[start_index+4*k+1] = data[start_index+4*k];
					data[start_index+4*k+2] = data[start_index+4*k];
					data[start_index+4*k+3] = data[start_index+4*k];
					
				}
			}


			_OLED12864.drawData(data, 128);
		}
		break;
	}

	case MODE_64_32:
	{
		_OLED12864.setTextXY(row, 0);		   //Set the cursor to Xth Page, Yth Column  


		memset(data, 0, 128);


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
		break;
	}

	default:
		break;


	}
	
}

void uArmGroveOLED12864::tick()
{
}

