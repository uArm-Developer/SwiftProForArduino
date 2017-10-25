/* LSM303DLM Example Code base on LSM303DLH example code by Jim Lindblom SparkFun Electronics
   
   date: 9/6/11
   license: Creative commons share-alike v3.0
   
   Modified by:Frankie.Chu
   Modified by:Jacky.Zhang 2014-12-11: Ported to 6-Axis Accelerometer&Compass of Seeed Studio
   Modified by:Jacky.Zhang 2015-1-6: added SPI driver
   
   Summary:
   Show how to calculate level and tilt-compensated heading using
   the snazzy LSM303DLH 3-axis magnetometer/3-axis accelerometer.
   
   Firmware:
   You can set the accelerometer's full-scale range by setting
   the SCALE constant to either 2, 4, or 8. This value is used
   in the initLSM303() function. For the most part, all other
   registers in the LSM303 will be at their default value.
   
   Use the write() and read() functions to write
   to and read from the LSM303's internal registers.
   
   Use getLSM303_accel() and getLSM303_mag() to get the acceleration
   and magneto values from the LSM303. You'll need to pass each of
   those functions an array, where the data will be stored upon
   return from the void.
   
   getHeading() calculates a heading assuming the sensor is level.
   A float between 0 and 360 is returned. You need to pass it a
   array with magneto values. 
   
   getTiltHeading() calculates a tilt-compensated heading.
   A float between 0 and 360 degrees is returned. You need
   to pass this function both a magneto and acceleration array.
   
   Headings are calculated as specified in AN3192:
   http://www.sparkfun.com/datasheets/Sensors/Magneto/Tilt%20Compensated%20Compass.pdf
*/

/*
hardware & software comment

I2C mode:
1, solder the jumper "I2C EN" and the jumper of ADDR to 0x1E
2, use Lsm303d.initI2C() function to initialize the Grove by I2C

SPI mode:

1, break the jumper "I2C_EN" and the jumper ADDR to any side
2, define a pin as chip select for SPI protocol.
3, use Lsm303d.initSPI(SPI_CS) function to initialize the Grove by SPI
SPI.h sets these for us in arduino
const int SDI = 11;
const int SDO = 12;
const int SCL = 13;
*/


#include "Accelerometer_Compass_LSM303D.h"
#include <Wire.h>
#include <SPI.h>

/* LSM303 Address definitions */
#define LSM303D_ADDR  0x1E  // assuming SA0 grounded

/* LSM303 Register definitions */
#define TEMP_OUT_L		0x05
#define TEMP_OUT_H		0x06
#define	STATUS_REG_M	0x07
#define OUT_X_L_M 		0x08
#define OUT_X_H_M 		0x09
#define OUT_Y_L_M 		0x0A
#define OUT_Y_H_M 		0x0B
#define OUT_Z_L_M 		0x0C
#define OUT_Z_H_M 		0x0D
#define	WHO_AM_I		0x0F
#define	INT_CTRL_M		0x12
#define	INT_SRC_M		0x13
#define	INT_THS_L_M		0x14
#define	INT_THS_H_M		0x15
#define	OFFSET_X_L_M	0x16
#define	OFFSET_X_H_M	0x17
#define	OFFSET_Y_L_M	0x18
#define	OFFSET_Y_H_M	0x19
#define	OFFSET_Z_L_M	0x1A
#define	OFFSET_Z_H_M	0x1B
#define REFERENCE_X 	0x1C
#define REFERENCE_Y 	0x1D
#define REFERENCE_Z 	0x1E
#define CTRL_REG0 		0x1F
#define CTRL_REG1 		0x20
#define CTRL_REG2		0x21
#define CTRL_REG3 		0x22
#define CTRL_REG4 		0x23
#define CTRL_REG5 		0x24
#define CTRL_REG6 		0x25
#define CTRL_REG7 		0x26
#define	STATUS_REG_A	0x27
#define OUT_X_L_A 		0x28
#define OUT_X_H_A 		0x29
#define OUT_Y_L_A 		0x2A
#define OUT_Y_H_A 		0x2B
#define OUT_Z_L_A 		0x2C
#define OUT_Z_H_A 		0x2D
#define	FIFO_CTRL		0x2E
#define	FIFO_SRC		0x2F
#define	IG_CFG1			0x30
#define	IG_SRC1			0x31
#define	IG_THS1			0x32
#define	IG_DUR1			0x33
#define	IG_CFG2			0x34
#define	IG_SRC2			0x35
#define	IG_THS2			0x36
#define	IG_DUR2			0x37
#define	CLICK_CFG		0x38
#define	CLICK_SRC		0x39
#define	CLICK_THS		0x3A
#define	TIME_LIMIT		0x3B
#define	TIME_LATENCY	0x3C
#define	TIME_WINDOW		0x3D
#define	ACT_THS			0x3E
#define	ACT_DUR			0x3F

#define MAG_SCALE_2 	0x00//full-scale is +/-2Gauss
#define MAG_SCALE_4 	0x20//+/-4Gauss
#define MAG_SCALE_8 	0x40//+/-8Gauss
#define MAG_SCALE_12 	0x60//+/-12Gauss

byte Read  = 0B10000000;
byte Write = 0B00000000;

//I2C mode
char LSM303D::initI2C()
{
	char rtn = -1;
    
    _mode = 0;//I2C mode
	Wire.begin();  // Start up I2C, required for LSM303 communication
	rtn = config();
    
	return rtn;
}

//SPI mode
char LSM303D::initSPI(char cspin)
{
	char rtn = -1;
    
    _mode = 1;//SPI mode
    _cs = cspin;
    pinMode(_cs, OUTPUT);//initialize the chip select pins;
    SPI.begin();//start the SPI library;
    rtn = config();
    
	return rtn;
}

char LSM303D::config()
{
    char rtn = -1;
    
    if(read(WHO_AM_I) != 0x49) return rtn; // return wrong if no LSM303D was found 
    
	write(0x57, CTRL_REG1);  // 0x57 = ODR=50hz, all accel axes on
	write((3<<6)|(0<<3), CTRL_REG2);  // set full-scale
	write(0x00, CTRL_REG3);  // no interrupt
	write(0x00, CTRL_REG4);  // no interrupt
	write((4<<2), CTRL_REG5);  // 0x10 = mag 50Hz output rate
	write(MAG_SCALE_2, CTRL_REG6); //magnetic scale = +/-1.3Gauss
	write(0x00, CTRL_REG7);  // 0x00 = continouous conversion mode
	rtn = 0;
    
	return rtn;
}

unsigned char LSM303D::read(unsigned char address)
{
	char temp = 0x00;
    
    if(_mode == 0)//I2C mode
    {
        Wire.beginTransmission(LSM303D_ADDR);
        Wire.write(address);
        Wire.endTransmission();
        Wire.requestFrom(LSM303D_ADDR, 1);
        while(!Wire.available());
        temp = Wire.read();
        Wire.endTransmission();
    }
    else//SPI Mode
    {
        digitalWrite(_cs, LOW); 
        SPI.transfer(Read | address);
        temp = SPI.transfer(0x00);
        digitalWrite(_cs, HIGH);
    }
    
	return temp;
}

void LSM303D::write(unsigned char data, unsigned char address)
{
    if(_mode == 0)
    {
        Wire.beginTransmission(LSM303D_ADDR); 
        Wire.write(address);
        Wire.write(data);
        Wire.endTransmission();
    }
    else
    {
        digitalWrite(_cs, LOW);
        SPI.transfer(Write | address);
        SPI.transfer(data);
        digitalWrite(_cs, HIGH);
    }
}

char LSM303D::isMagReady()
{
	char temp;
    
	temp = read(STATUS_REG_M) & 0x03;
    
	return temp;
}

void LSM303D::getMag(int * rawValues)
{
	rawValues[X] = ((int)read(OUT_X_H_M) << 8) | (read(OUT_X_L_M));
	rawValues[Y] = ((int)read(OUT_Y_H_M) << 8) | (read(OUT_Y_L_M));
	rawValues[Z] = ((int)read(OUT_Z_H_M) << 8) | (read(OUT_Z_L_M));
}

void LSM303D::getAccel(int * rawValues)
{
	rawValues[X] = ((int)read(OUT_X_H_A) << 8) | (read(OUT_X_L_A));
	rawValues[Y] = ((int)read(OUT_Y_H_A) << 8) | (read(OUT_Y_L_A));
	rawValues[Z] = ((int)read(OUT_Z_H_A) << 8) | (read(OUT_Z_L_A));
}

float LSM303D::getHeading(int * magValue)
{
	// see section 1.2 in app note AN3192
	float heading = 180*atan2(magValue[Y], magValue[X])/PI;  // assume pitch, roll are 0

	if (heading <0)
	heading += 360;

	return heading;
}

float LSM303D::getTiltHeading(int * magValue, float * accelValue)
{
	// see appendix A in app note AN3192 
	float pitch = asin(-accelValue[X]);
	float roll = asin(accelValue[Y]/cos(pitch));

	float xh = magValue[X] * cos(pitch) + magValue[Z] * sin(pitch);
	float yh = magValue[X] * sin(roll) * sin(pitch) + magValue[Y] * cos(roll) - magValue[Z] * sin(roll) * cos(pitch);
	float zh = -magValue[X] * cos(roll) * sin(pitch) + magValue[Y] * sin(roll) + magValue[Z] * cos(roll) * cos(pitch);
	float heading = 180 * atan2(yh, xh)/PI;

	if (yh >= 0)
		return heading;
	else
		return (360 + heading);
}



