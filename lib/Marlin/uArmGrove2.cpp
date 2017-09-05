/**
  ******************************************************************************
  * @file	uArmGrove.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#include "uArmGrove2.h" 
#include "uArmGroveChainableLED.h"
#include "uArmGroveSlidingPotentiometer.h"
#include "uArmGroveButton.h"
#include "uArmGroveUltrasonic.h"
#include "uArmGroveLCD1602.h"
#include "uArmGroveelectromagnet.h"
#include "uArmGroveTH.h"
#include "uArmGrovePIRMotion.h"
#include "uArmGroveFan.h"
#include "uArmGroveColorSensor.h"
#include "uArmGroveGestureSensor.h"


#define PORT_NUM_MAX	14

uArmGroveBase* pModule[PORT_NUM_MAX] = {NULL};

uint16_t PortMask[GROVE_TYPE_COUNT] = {
	0,
	0x28,	//GROVE_CHAINABLE_LED = 1
	0x38,	//GROVE_BUTTON = 2
	0x6,	//GROVE_SLIDING_POTENTIOMETER = 3
	0,
	
	0,
	0,
	0,
	0,
	0,

	0x1,	//GROVE_COLOR_SENSOR = 10
	0x1,	//GROVE_GESTURE_SERSOR = 11
	0x328,	//GROVE_ULTRASONIC = 12
	0x310,	//GROVE_FAN = 13
	0x328,	//GROVE_ELECTROMAGNET = 14

	0x1,	//GROVE_TEMPERATURE_HUMIDITY_SENSOR = 15
	0X328,	//GROVE_PIR_MOTION_SENSOR = 16
	1	//GROVE_RGBLCD = 17

};

#define GROVE_CLK_PIN 	0
#define GROVE_DAT_PIN	1

uint8_t port_pin_map[PORT_NUM_MAX][2] = {
	{0,    0},	//port0 I2C
	{14,   41},	//port1 A
	{15,   47},	//port2 A
	{17,   16},	//port3 D/UART

	{10,   51},	//port4 D/PWM
	{43,   45},	//port5 D
	{0xff, 0xff},	//
	{0xff, 0xff},	//

	{8,    0xff},	//port8 D
	{9,    0xff},	//port9 D
	{0xff, 0xff},	//
	{0xff, 0xff},	//

	{0xff, 0xff},	//
	{13,   0xff},	//port13 A
};


void uArmGroveTick()
{
	for (int i = 0; i < PORT_NUM_MAX; i++)
	{
		if (pModule[i] != NULL)
			pModule[i]->tick();
	}
}

uint8_t initGroveModule2(uint8_t portNum, GroveType type, unsigned char replybuf[])
{
	uint8_t strNum[12] = {0};

	if (portNum > PORT_NUM_MAX - 1)
	{
		sprintf(replybuf, "port num should less than %d\r\n", PORT_NUM_MAX);
		return E_FAIL;
	}

	if (type > GROVE_TYPE_COUNT - 1)
	{
		sprintf(replybuf, "module num should less than %d\r\n", GROVE_TYPE_COUNT);
		return E_FAIL;		
	}

	if (!(PortMask[type] & (1 << portNum)))
	{
		sprintf(replybuf, "For module %d, valid port(s) is(are): ", type);
		for (int i = 0; i < PORT_NUM_MAX; i++)
		{
			if (PortMask[type] & (1 << i))
			{
				itoa( i, strNum, 10);
				strcat(replybuf, strNum);
				strcat(replybuf, " ");
				//MYSERIAL.println(i);
			}
		}
		strcat(replybuf, "\r\n");
		return E_FAIL;			
	}

	if (pModule[portNum] != NULL)
	{

		delete pModule[portNum];
		pModule[portNum] = NULL;
	}

	switch (type)
	{
	case GROVE_CHAINABLE_LED:
		pModule[portNum] = new uArmGroveChainableLED();
		break;

	case GROVE_BUTTON:
		pModule[portNum] = new uArmGroveButton();
		break;

	case GROVE_SLIDING_POTENTIOMETER:
		pModule[portNum] = new uArmGroveSlidingPotentiometer();
		break;

	case GROVE_ULTRASONIC:
		pModule[portNum] = new uArmGroveUltrasonic();
		break;

	case GROVE_RGBLCD:
		pModule[portNum] = new uArmGroveLCD1602();
		break;

	case GROVE_ELECTROMAGNET:
		pModule[portNum] = new uArmGroveElectromagnet();
		break;

	case GROVE_TEMPERATURE_HUMIDITY_SENSOR:
		pModule[portNum] = new uArmGroveTH();
		break;

	case GROVE_PIR_MOTION_SENSOR:
		pModule[portNum] = new uArmGrovePIRMotion();
		break;

	case GROVE_FAN:
		pModule[portNum] = new uArmGroveFan();
		break;

	case GROVE_COLOR_SENSOR:
		pModule[portNum] = new uArmGroveColorSensor();
		break;

	case GROVE_GESTURE_SERSOR:
		pModule[portNum] = new uArmGroveGestureSensor();
		break;

	}

	if (pModule[portNum] != NULL)
	{
		uint8_t clk_pin, dat_pin;
		clk_pin = port_pin_map[portNum][GROVE_CLK_PIN];
		dat_pin = port_pin_map[portNum][GROVE_DAT_PIN];
		
		if (pModule[portNum]->init(portNum, clk_pin, dat_pin))
		{
			return E_OK;
		}
		else
		{
			strcpy(replybuf, "init failed\r\n");
		}
	}
	else
	{
		strcpy(replybuf, "Memory allocation failed\r\n");
	}
	
	return E_FAIL;
}

void deinitGroveModule(uint8_t portNum)
{
	if (portNum > PORT_NUM_MAX - 1)
	{
		return;
	}

	if (pModule[portNum] != NULL)
	{

		delete pModule[portNum];
		pModule[portNum] = NULL;
	}


}

uint8_t controlGroveModule(uint8_t portNum, unsigned char replybuf[])
{
	if (portNum > PORT_NUM_MAX - 1)
	{
		sprintf(replybuf, "port num should less than %d\r\n", PORT_NUM_MAX);
		return E_FAIL;
	}

	if (pModule[portNum] == NULL)
	{

		sprintf(replybuf, "port %d not initialized\r\n", portNum);
		return E_FAIL;
	}

	pModule[portNum]->control();
	return E_OK;
}

struct GroveReport_t
{
	long interval;
	long timestamp;
};


GroveReport_t groveReport[PORT_NUM_MAX] = {0};

void GroveReportRun()
{
	for (int i = 0; i < PORT_NUM_MAX; i++)
	{

		long curTimeStamp = millis();

		// exist
		if (groveReport[i].interval != 0)
		{

			if((unsigned long)(curTimeStamp - groveReport[i].timestamp) >= groveReport[i].interval)
			{
				groveReport[i].timestamp = curTimeStamp;


				if (pModule[i] != NULL)
				{
					pModule[i]->report();
				}

			}

		}
	
	}
}

uint8_t setGroveModuleReportInterval2(uint8_t portNum, long interval, unsigned char replybuf[])
{
	if (portNum > PORT_NUM_MAX - 1)
	{
		sprintf(replybuf, "port num should less than %d\r\n", PORT_NUM_MAX);
		return E_FAIL;
	}

	if (pModule[portNum] == NULL)
	{

		sprintf(replybuf, "port %d not initialized\r\n", portNum);
		return E_FAIL;
	}	

	if (interval <= 0)
	{
		groveReport[portNum].interval = 0;
		return E_OK;
	}

	groveReport[portNum].interval = interval;
	groveReport[portNum].timestamp = millis();

	return E_OK;
}

