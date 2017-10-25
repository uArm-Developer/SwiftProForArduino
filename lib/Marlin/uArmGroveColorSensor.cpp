/**
 * uArm Swift Pro Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Marlin.
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
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


#include "uArmGroveColorSensor.h"






void uArmGroveColorSensor::getRawData_noDelay(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c)
{
  *c = _tcs->read16(TCS34725_CDATAL);
  *r = _tcs->read16(TCS34725_RDATAL);
  *g = _tcs->read16(TCS34725_GDATAL);
  *b = _tcs->read16(TCS34725_BDATAL);
}


uArmGroveColorSensor::uArmGroveColorSensor()
{
	_tcs = new Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_16X); // TCS34725_GAIN_1X

}

uArmGroveColorSensor::~uArmGroveColorSensor()
{
	delete _tcs;
}

bool uArmGroveColorSensor::init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin)
{
	if (_tcs == NULL)
	{
		return false;
	}

	if (_tcs->begin()) {
		_tcs->write8(TCS34725_PERS, TCS34725_PERS_NONE); 
		_tcs->setInterrupt(true); 		
	} else {
		MYSERIAL.println("No TCS34725 found ... check your connections");
		MYSERIAL.println("use P0(I2C)");
		return false;
	}

	_portNum = portNum;
	_clk_pin = clk_pin;
	_dat_pin = dat_pin;
	
	return true;
}

void uArmGroveColorSensor::report()
{
	char result[128];
	uint16_t red, green, blue, c;

	getRawData_noDelay(&red, &green, &blue, &c);

	_tcs->clearInterrupt();

	red /= 255;
	green /= 255;
	blue /= 255;

	red = constrain(red,0,255);
	green = constrain(green,0,255);
	blue = constrain(blue,0,255);


	msprintf(result, "@%d P%d N%d R%d G%d B%d\r\n", REPORT_TYPE_GROVE2, _portNum, GROVE_COLOR_SENSOR, red, green, blue);
	reportString(result);
}

void uArmGroveColorSensor::control()
{
	
}

void uArmGroveColorSensor::tick()
{
}

