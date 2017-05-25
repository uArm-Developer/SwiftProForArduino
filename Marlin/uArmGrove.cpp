/**
  ******************************************************************************
  * @file	uArmGrove.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#include "uArmGrove.h" 
#include "GroveColorSensor.h"
#include "paj7620.h" 

GroveColorSensor colorSensor;

void GestureReport()
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
					gesture = GES_RIGHT_FLAG;
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
		msprintf(result, "@%d V%d", GROVE_COLOR_SENSOR, gesture);
		reportString(result);
	}

}

void GroveColorReport()
{
	char result[128];
	int red, green, blue;
	

	colorSensor.readRGB(&red, &green, &blue);		//Read RGB values to variables.


	colorSensor.clearInterrupt();

	msprintf(result, "@%d R%d G%d B%d", GROVE_COLOR_SENSOR, red, green, blue);
	reportString(result);
}

void initGroveModule(GroveType type)
{
	uint8_t error = 0;

	switch (type) {
	case GROVE_COLOR_SENSOR:
		debugPrint("initGroveModule\n");
		colorSensor.init();
		break;
	
	case GROVE_GESTURE_SERSOR:
		error = paj7620Init();			// initialize Paj7620 registers
		if (error) 
		{
			debugPrint("INIT ERROR,CODE: %d\r\n", error);
		}

		break;

	default:
		break;
	}
}

void setGroveModuleReportInterval(GroveType type, long interval)
{
	switch (type) {
	case GROVE_COLOR_SENSOR:
		if (interval <= 0)
		{
			// turn off report, remove from list
			//colorSensor.ledStatus = 0;
			removeReportService(type);
		}
		else
		{
			// add to report list
			//colorSensor.ledStatus = 1;	
			debugPrint("setGroveModuleReportInterval\n");
			addReportService(type, interval, GroveColorReport);
		}
		break;

	case GROVE_GESTURE_SERSOR:
		if (interval <= 0)
		{
			// turn off report, remove from list
			//colorSensor.ledStatus = 0;
			removeReportService(type);
		}
		else
		{
			// add to report list
			//colorSensor.ledStatus = 1;	
			debugPrint("setGroveModuleReportInterval\n");
			addReportService(type, interval, GestureReport);
		}		
		break;
		
	default:
		break;
	}	
}