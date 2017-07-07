/**
  ******************************************************************************
  * @file	uArmCalibration.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-15
  ******************************************************************************
  */

#include "uArmCalibration.h" 
#include "macros.h"


uint16_t reference_angle_value[NUM_AXIS] = {95, 1429, 1998, 0};

float reference_angle[NUM_AXIS] = {90.0, 32.4, 90.6, 0.0};


void init_reference_angle_value()
{
	//read value from eeprom
	reference_angle_value[X_AXIS] = getE2PROMData(EEPROM_ON_CHIP, EEPROM_REFERENCE_VALUE_ADDR, DATA_TYPE_INTEGER);
	reference_angle_value[Y_AXIS] = getE2PROMData(EEPROM_ON_CHIP, EEPROM_REFERENCE_VALUE_ADDR+2, DATA_TYPE_INTEGER);
	reference_angle_value[Z_AXIS] = getE2PROMData(EEPROM_ON_CHIP, EEPROM_REFERENCE_VALUE_ADDR+4, DATA_TYPE_INTEGER);	

	debugPrint("read ref value: %d, %d, %d\r\n", reference_angle_value[0], reference_angle_value[1], reference_angle_value[2]);
}

void update_reference_angle_value(uint16_t value[NUM_AXIS])
{

	reference_angle_value[X_AXIS] = value[X_AXIS];
	reference_angle_value[Y_AXIS] = value[Y_AXIS];
	reference_angle_value[Z_AXIS] = value[Z_AXIS];


	setE2PROMData(EEPROM_ON_CHIP, EEPROM_REFERENCE_VALUE_ADDR, DATA_TYPE_INTEGER, reference_angle_value[X_AXIS]);
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_REFERENCE_VALUE_ADDR+2, DATA_TYPE_INTEGER, reference_angle_value[Y_AXIS]);
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_REFERENCE_VALUE_ADDR+4, DATA_TYPE_INTEGER, reference_angle_value[Z_AXIS]);	

	debugPrint("set ref value: %d, %d, %d\r\n", reference_angle_value[0], reference_angle_value[1], reference_angle_value[2]);
}

uint16_t get_current_angle_adc(uint8_t index)
{
	uint16_t value = 0;

	switch(index)
	{
	case X_AXIS:
		value =  X_IIC_Read_OneByte((0x36<<1),0x0e);   
		value <<= 8;
		value |= X_IIC_Read_OneByte((0x36<<1),0x0f); 	
		break;

	case Y_AXIS:
		value =  Y_IIC_Read_OneByte((0x36<<1),0x0e);   
		value <<= 8;
		value |= Y_IIC_Read_OneByte((0x36<<1),0x0f); 	
		break;

	case Z_AXIS:
		value =  Z_IIC_Read_OneByte((0x36<<1),0x0e);   
		value <<= 8;
		value |= Z_IIC_Read_OneByte((0x36<<1),0x0f); 	
		break;

	case E_AXIS:
		//servo
		value = map(getAnalogPinValue(SERVO_HAND_ROT_ANALOG_PIN), SERVO_9G_MIN, SERVO_9G_MAX, 0, 180);;
		break;

	default:
		value = 0;
	}

	return value;
}

float get_current_angle(uint8_t index)
{
	if (index > E_AXIS)
		return 0;

	if (index == E_AXIS)
		return get_current_angle_adc(index);

	float angle = 0.0;
	uint16_t cur_value = get_current_angle_adc(index);
	uint16_t diff = 0;

	if (cur_value > reference_angle_value[index])
	{
		diff = cur_value - reference_angle_value[index];
		angle = reference_angle[index] + diff * 360.0 / 4096;
	}
	else if (cur_value < reference_angle_value[index])
	{
		diff = reference_angle_value[index] - cur_value;
		angle = reference_angle[index] - diff * 360.0 / 4096;
	}
	else
	{
		angle = reference_angle[index];
	}

	debugPrint("cur_value = %d\r\n", cur_value);
	debugPrint("reference_value = %d\r\n", reference_angle_value[index]);

	debugPrint("angle = %f\r\n", angle);

	float origin_angle = angle;
	float min = 0, max = 0;
	
	if (angle > 360)
	{
		angle -= 360;
	}

	if (angle < 0)
	{
		angle += 360;
	}	
	debugPrint("angle2 = %f\r\n", angle);

	switch (index)
	{
	case X_AXIS:
		min = 0;
		max = 180;
		break;
		
	case Y_AXIS:
		min = LOWER_ARM_MIN_ANGLE;
		max = LOWER_ARM_MAX_ANGLE;
		break;
		
	case Z_AXIS:
		min = UPPER_ARM_MIN_ANGLE;
		max = UPPER_ARM_MAX_ANGLE;
		break;
	
	}

	if (angle >= min && angle <= max)
	{
		return angle;
	}
	else if (diff > 2048)
	{
		if (origin_angle < 0)
		{
			return max;
		}
		else
		{
			return min;
		}
	}
	else
	{
		if (origin_angle < 0)
		{
			return min;
		}
		else
		{
			return max;
		}
	}

	//return angle;
}




