/**
  ******************************************************************************
  * @file	uArmCalibration.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-15
  ******************************************************************************
  */

#ifndef _UARMCALIBRATION_H_
#define _UARMCALIBRATION_H_

#include "Marlin.h"
#include "uArmAPI.h"
#include "uArmParams.h"
#include "uArmDebug.h"
#include "uArmIIC.h"
#include "X_IIC.h"
#include "Y_IIC.h"
#include "Z_IIC.h"

float get_current_angle(uint8_t index);
void init_reference_angle_value();
void update_reference_angle_value(uint16_t value[NUM_AXIS]);
void update_reference_angle_value_B(uint16_t value[NUM_AXIS]);


uint16_t get_current_angle_adc(uint8_t index);
void set_current_pos_as_reference();



#endif // _UARMCALIBRATION_H_
