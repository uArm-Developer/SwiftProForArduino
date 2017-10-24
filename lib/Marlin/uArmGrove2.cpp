/**
  ******************************************************************************
  * @file	uArmGrove.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#include "uArmGrove2.h" 


#define PORT_NUM_MAX	14

uArmGroveBase* pModule[PORT_NUM_MAX] = {NULL};

uint16_t PortMask[GROVE_TYPE_COUNT] = {
	0,
	0x38,	//GROVE_CHAINABLE_LED
	0x38,	//GROVE_BUTTON
	6,		//GROVE_SLIDING_POTENTIOMETER
	0x338,	//GROVE_VIBRATION_MOTOR
	0x6,	//GROVE_LIGHT_SENSOR
	0x6,	//GROVE_ANGLE_SENSOR
	0x6,	//GROVE_AIR_QUALITY_SENSOR
	0x6,	//GROVE_SOUND_SENSOR
	1,		//GROVE_6AXIS_SENSOR
	1,		//GROVE_COLOR_SENSOR
	1,		//GROVE_GESTURE_SERSOR
	0x310,	//GROVE_ULTRASONIC
	0x310,	//GROVE_FAN
	0x338,	//GROVE_ELECTROMAGNET
	1,		//GROVE_TEMPERATURE_HUMIDITY_SENSOR
	0x338,	//GROVE_PIR_MOTION_SENSOR
	1,		//GROVE_RGBLCD
	0x338,	//GROVE_LINE_FINDER
	0x338,	//GROVE_IR_DISTANCE
	0x6,	//GROVE_EMG_DETECTOR
	1		//GROVE_OLED12864
};

#define GROVE_CLK_PIN 	0
#define GROVE_DAT_PIN	1

uint8_t port_pin_map[PORT_NUM_MAX][2] = {
	{0,    0},		//port0 I2C
	{14,   41},		//port1 A
	{15,   47},		//port2 A
	{39,   29},		//port3 D

	{10,   51},		//port4 D/PWM
	{43,   45},		//port5 D
	{0xff, 0xff},	//not used
	{0xff, 0xff},	//not used

	{8,    0xff},	//port8 D
	{9,    0xff},	//port9 D
	{0xff, 0xff},	//not used
	{0xff, 0xff},	//not used

	{0xff, 0xff},	//not used
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

	case GROVE_VIBRATION_MOTOR:
		pModule[portNum] = new uArmGroveVibrationMotor();
		break;

	case GROVE_LIGHT_SENSOR:
		pModule[portNum] = new uArmGroveLightSensor();
		break;

	case GROVE_ANGLE_SENSOR:
		pModule[portNum] = new uArmGroveAngleSensor();
		break;

	case GROVE_AIR_QUALITY_SENSOR:
		pModule[portNum] = new uArmGroveAirQualitySensor();
		break;

	case GROVE_SOUND_SENSOR:
		pModule[portNum] = new uArmGroveSoundSensor();
		break;

	case GROVE_6AXIS_SENSOR:
		pModule[portNum] = new uArmGrove6AxisSensor();
		break;

	case GROVE_LINE_FINDER:
		pModule[portNum] = new uArmGroveLineFinder();
		break;

	case GROVE_IR_DISTANCE:
		pModule[portNum] = new uArmGroveIRDistance();
		break;

	case GROVE_EMG_DETECTOR:
		pModule[portNum] = new uArmGroveEMGDetector();
		break;

	case GROVE_OLED12864:
		pModule[portNum] = new uArmGroveOLED12864();
		break;

/*
	case GROVE_SERVO:
		pModule[portNum] = new uArmGroveServo();
		break;
*/

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

