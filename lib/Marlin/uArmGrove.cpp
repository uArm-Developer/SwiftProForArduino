/**
  ******************************************************************************
  * @file	uArmGrove.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#include "uArmGrove.h" 
#include "paj7620.h" 
#include "Ultrasonic.h"
#include "Grovefan.h"
#include "Adafruit_TCS34725.h"
#include "Groveelectromagnet.h"
#include "Grove_TH02_dev.h"
#include "Grovepirmotion.h"
#include "Grovergb_lcd.h"



#define DIGITAL_PIN_8	8
#define DIGITAL_PIN_9	9

#define ALALOG_PIN_13	13


#define DEFAULT_DIGITAL_PIN	DIGITAL_PIN_8
#define DEFAULT_ALALOG_PIN	ALALOG_PIN_13



Ultrasonic ultrasonic;
Grovefan grovefan;
Groveelectromagnet groveelectromagnet;
TH02_dev grove_TH;	
Grovepirmotion grovepirmotion;
Grovergb_lcd grovergb_lcd;

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_16X); // TCS34725_GAIN_1X




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
		msprintf(result, "@%d N%d V%d\r\n", REPORT_TYPE_GROVE, GROVE_GESTURE_SERSOR, gesture);
		reportString(result);
	}

}


void getRawData_noDelay(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  *c = tcs.read16(TCS34725_CDATAL);
  *r = tcs.read16(TCS34725_RDATAL);
  *g = tcs.read16(TCS34725_GDATAL);
  *b = tcs.read16(TCS34725_BDATAL);
}



void GroveColorReport()
{
	char result[128];
	uint16_t red, green, blue, c;

	getRawData_noDelay(&red, &green, &blue, &c);

	tcs.clearInterrupt();

	red /= 255;
	green /= 255;
	blue /= 255;

	red = constrain(red,0,255);
	green = constrain(green,0,255);
	blue = constrain(blue,0,255);


	msprintf(result, "@%d N%d R%d G%d B%d\r\n", REPORT_TYPE_GROVE, GROVE_COLOR_SENSOR, red, green, blue);
	reportString(result);
}



void UltrasonicReport()
{
	char result[128];

    ultrasonic.MeasureInCentimeters();

	msprintf(result, "@%d N%d V%d\r\n", REPORT_TYPE_GROVE, GROVE_ULTRASONIC, ultrasonic.RangeInCentimeters);
	reportString(result);
}

/*
*Temperature & Humidity report
*/
void GroveTHReport()
{
	char result[128];

	msprintf(result, "@%d N%d T%f H%f\r\n", REPORT_TYPE_GROVE, GROVE_TEMPERATURE_HUMIDITY_SENSOR, grove_TH.ReadTemperature(), grove_TH.ReadHumidity());
	reportString(result);
}
/*
*PIR Motion report
*/
void GrovePIRMotionReport()
{
	char result[128];

	msprintf(result, "@%d N%d V%d\r\n", REPORT_TYPE_GROVE, GROVE_PIR_MOTION_SENSOR, grovepirmotion.getstatus());
	reportString(result);
}

void initGroveModule(GroveType type, GrovePortType portType, unsigned char pin)
{
	uint8_t error = 0;

	debugPrint("initGroveModule %d\n", type);

	switch (type) {
	case GROVE_COLOR_SENSOR:
		
		//colorSensor.init();
		if (tcs.begin()) {
		    //Serial.println("Found sensor");
  			tcs.write8(TCS34725_PERS, TCS34725_PERS_NONE); 
  			tcs.setInterrupt(true);		    
		} else {
		    MYSERIAL.println("No TCS34725 found ... check your connections");
		}		
		break;
	
	case GROVE_GESTURE_SERSOR:
		error = paj7620Init();			// initialize Paj7620 registers
		if (error) 
		{
			debugPrint("INIT ERROR,CODE: %d\r\n", error);
		}

		break;

	case GROVE_ULTRASONIC:
		switch (pin)
		{
		case DIGITAL_PIN_8:
		case DIGITAL_PIN_9:
			ultrasonic.setPin(pin);
			break;

		default:
			ultrasonic.setPin(DEFAULT_DIGITAL_PIN);
			break;
		}
		break;

	case GROVE_FAN:
		switch (pin)
		{
		case DIGITAL_PIN_8:
		case DIGITAL_PIN_9:
			grovefan.setPin(pin);
			break;
		default:
			grovefan.setPin(DEFAULT_DIGITAL_PIN);
			break;			
		}
		break;		

	case GROVE_ELECTROMAGNET:
		switch (pin)
		{
		case DIGITAL_PIN_8:
		case DIGITAL_PIN_9:
			groveelectromagnet.setPin(pin);
			break;

		default:
			groveelectromagnet.setPin(DEFAULT_DIGITAL_PIN);
			break;
		}
		break;		

	case GROVE_TEMPERATURE_HUMIDITY_SENSOR:
		debugPrint("initGrove Temperature & Humidity Module\n");
		grove_TH.begin();
		break;	

	case GROVE_PIR_MOTION_SENSOR:
		switch (pin)
		{
		case DIGITAL_PIN_8:
		case DIGITAL_PIN_9:
			grovepirmotion.setPin(pin);
			break;

		default:
			grovefan.setPin(DEFAULT_DIGITAL_PIN);
			break;
		}
		break;
	case GROVE_RGBLCD:
		debugPrint("initGrove  grovergb_lcd Module\n");
		grovergb_lcd.begin(16,2);
		break;		

	default:
		break;
	}
}

void setGroveModuleReportInterval(GroveType type, long interval)
{

	if (interval <= 0)
	{
		removeReportService(type);		
	}
	else
	{

		switch (type) {
		case GROVE_COLOR_SENSOR:
			addReportService(type, interval, GroveColorReport);
			break;

		case GROVE_GESTURE_SERSOR:
			addReportService(type, interval, GestureReport);		
			break;

		case GROVE_ULTRASONIC:
			addReportService(type, interval, UltrasonicReport);	
			break;

		case GROVE_TEMPERATURE_HUMIDITY_SENSOR:
			addReportService(type, interval, GroveTHReport);	
			break;			

		case GROVE_PIR_MOTION_SENSOR:
			addReportService(type, interval, GrovePIRMotionReport);	
			break;				

		default:
			break;
		}	
	}
}

void setGroveModuleValue(GroveType type, long value)
{
	switch (type) {
	case GROVE_COLOR_SENSOR:
		break;

	case GROVE_GESTURE_SERSOR:
		break;

	case GROVE_ULTRASONIC:
		break;
		
	case GROVE_FAN:
		grovefan.speed(value);
		break;

	case GROVE_ELECTROMAGNET:
		if(value)
		{
			groveelectromagnet.on();
		}
		else
		{
			groveelectromagnet.off();
		}
		break;			

	default:
		break;
		
	}
}

void setGroveLCDModuleValue(GroveType type,GrovelcdcmdType cmd,long value)
{
	switch (cmd) {			
									
		case GROVE_LCD_TYPE_NODISPLAY:
			grovergb_lcd.noDisplay();
			break;	
		case GROVE_LCD_TYPE_DISPLAY:
			grovergb_lcd.display();
			break;				
		case GROVE_LCD_TYPE_CLEAR:
			grovergb_lcd.clear();
			break;								

	default:
		break;
		
	}
}

void setGroveLCDModuleString(GroveType type,GrovelcdstringType cmd,char string[])
{
	switch (cmd) {
			
		case GROVE_CMD_TYPE_DISPLAYROW1:
		case GROVE_CMD_TYPE_DISPLAYROW2:
			debugPrint("lcdtext:%s\r\n",string);
			//clear the row text
			for(int i=0;i<16;i++)
			{
		       		grovergb_lcd.setCursor(i,cmd-1);	
				grovergb_lcd.write(" ");
		    }
		    delay(10);	
		    //display the text
			for(int i=0;i<strlen(string);i++)
			{
		       		grovergb_lcd.setCursor(i,cmd-1);	
				grovergb_lcd.write(string[i]);
		    }
			//RGB_LCD_displaytext(cmd,strlen(string),string);
			break;													

	default:
		break;
		
	}
}

void setGroveLCDModuleRGB(GroveType type,GrovelcdcmdType cmd,long value)
{
	grovergb_lcd.setRGB(((value & 0xff0000) >> 16),((value & 0xff00) >> 8),(value & 0xff));				
}

