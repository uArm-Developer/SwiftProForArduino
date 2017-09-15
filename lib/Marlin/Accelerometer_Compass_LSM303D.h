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

#ifndef Accelerometer_Compass_LSM303D_H
#define Accelerometer_Compass_LSM303D_H

#include <Arduino.h>

#define ACCELE_SCALE 2  // accelerometer full-scale, should be 2, 4, or 8

#define X 0
#define Y 1
#define Z 2

class LSM303D
{
	public:

		char initI2C();
        char initSPI(char cspin);
        char config();
		unsigned char read(unsigned char address);
		void write(unsigned char data, unsigned char address);
		char isMagReady();
		void getMag(int * rawValues);
		void getAccel(int * rawValues);
		float getHeading(int * magValue);
		float getTiltHeading(int * magValue, float * accelValue);
	private:
        unsigned char _mode;
        unsigned char _cs;
};



#endif

