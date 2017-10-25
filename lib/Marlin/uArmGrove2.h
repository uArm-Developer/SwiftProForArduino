/**
  ******************************************************************************
  * @file	uArmGrove.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-06
  ******************************************************************************
  */

#ifndef _UARMGROVE2_H_
#define _UARMGROVE2_H_

#include <Arduino.h>
#include "Marlin.h"
#include "uArmGrove.h"
#include "uArmGroveBase.h"

#include "uArmGroveChainableLED.h"
#include "uArmGroveSlidingPotentiometer.h"
#include "uArmGroveButton.h"
#include "uArmGroveUltrasonic.h"
#include "uArmGroveLCD1602.h"
#include "uArmGroveElectromagnet.h"
#include "uArmGroveTH.h"
#include "uArmGrovePIRMotion.h"
#include "uArmGroveFan.h"
#include "uArmGroveColorSensor.h"
#include "uArmGroveGestureSensor.h"
#include "uArmGroveVibrationMotor.h"
#include "uArmGroveLightSensor.h"
#include "uArmGroveAngleSensor.h"
#include "uArmGroveAirQualitySensor.h"
#include "uArmGroveSoundSensor.h"
#include "uArmGrove6AxisSensor.h"
#include "uArmGroveLineFinder.h"
#include "uArmGroveIRDistance.h"
#include "uArmGroveEMGDetector.h"
#include "uArmGroveOLED12864.h"
//#include "uArmGroveServo.h"



uint8_t initGroveModule2(uint8_t portNum, GroveType type, unsigned char replybuf[]);
void deinitGroveModule(uint8_t portNum);
uint8_t controlGroveModule(uint8_t portNum, unsigned char replybuf[]);

void uArmGroveTick();
void GroveReportRun();


uint8_t setGroveModuleReportInterval2(uint8_t portNum, long interval, unsigned char replybuf[]);



#endif // _UARMGROVE2_H_
