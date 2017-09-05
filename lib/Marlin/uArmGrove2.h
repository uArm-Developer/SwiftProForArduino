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


uint8_t initGroveModule2(uint8_t portNum, GroveType type, unsigned char replybuf[]);
void deinitGroveModule(uint8_t portNum);
uint8_t controlGroveModule(uint8_t portNum, unsigned char replybuf[]);

void uArmGroveTick();
void GroveReportRun();


uint8_t setGroveModuleReportInterval2(uint8_t portNum, long interval, unsigned char replybuf[]);



#endif // _UARMGROVE2_H_
