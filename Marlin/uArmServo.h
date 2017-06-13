/**
  ******************************************************************************
  * @file	uArmServo.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-06-13
  ******************************************************************************
  */

#ifndef _UARMSERVO_H_
#define _UARMSERVO_H_

#include <Arduino.h>
#include "Marlin.h"

void servo_write(double value, bool remember = false);
void servo_tick();


#endif // _UARMSERVO_H_
