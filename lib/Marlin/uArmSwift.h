/**
  ******************************************************************************
  * @file	uArmSwift.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-15
  ******************************************************************************
  */

#ifndef _UARMSWIFT_H_
#define _UARMSWIFT_H_

#include <Arduino.h>
#include "Marlin.h"
#include "uArmParams.h"
#include "uArmDebug.h"
#include "uArmIIC.h"
#include "X_IIC.h"
#include "Y_IIC.h"
#include "Z_IIC.h"
#include "uArmCalibration.h"
#include "uArmButton.h"
#include "uArmLed.h"





void swift_init();

void swift_run();

bool is_fan_enable();
void set_fan_function(bool disable);
void laser_on(uint8_t power);

void laser_off();


bool is_heater_enable();
void set_heater_function(bool enable);


void update_current_pos();

void set_block_running(bool running);

bool get_block_running();

void reportString(String string);
void reportButtonEvent(unsigned char buttonId, unsigned char event);
void rotate_frontend_motor();

void uarm_gcode_G0();

void uarm_gcode_G1();

void uarm_gcode_M2000();

void uarm_gcode_M2120();
void uarm_gcode_M2122();

uint8_t uarm_gcode_M2200(char reply[]);

void uarm_gcode_M2201();
void uarm_gcode_M2202();
uint8_t uarm_gcode_M2203(char reply[]);

void uarm_gcode_M2210();
uint8_t uarm_gcode_M2211(char reply[]);
void uarm_gcode_M2212();
void uarm_gcode_M2213();

uint8_t uarm_gcode_M2220(char reply[]);
uint8_t uarm_gcode_M2221(char reply[]);

uint8_t uarm_gcode_M2222(char reply[]);

void uarm_gcode_M2231();
void uarm_gcode_M2232();
void uarm_gcode_M2233();
void uarm_gcode_M2234();



void uarm_gcode_M2240();
void uarm_gcode_M2241();

uint8_t uarm_gcode_M2245(char reply[]);


void uarm_gcode_M2300();
void uarm_gcode_M2301();
void uarm_gcode_M2302();
void uarm_gcode_M2303();
uint8_t uarm_gcode_M2304(char reply[]);
uint8_t uarm_gcode_M2305(char reply[]);
uint8_t uarm_gcode_M2306(char reply[]);
uint8_t uarm_gcode_M2307(char reply[]);



void uarm_gcode_M2400();
uint8_t uarm_gcode_M2401(char reply[]);
void uarm_gcode_M2410();
void uarm_gcode_M2411();

uint8_t uarm_gcode_M2420(char reply[]);
void uarm_gcode_M2500();


uint8_t uarm_gcode_P2200(char reply[]);
uint8_t uarm_gcode_P2201(char reply[]);
uint8_t uarm_gcode_P2202(char reply[]);
uint8_t uarm_gcode_P2203(char reply[]);
uint8_t uarm_gcode_P2204(char reply[]);
uint8_t uarm_gcode_P2205(char reply[]);
uint8_t uarm_gcode_P2206(char reply[]);

uint8_t uarm_gcode_P2220(char reply[]);
uint8_t uarm_gcode_P2221(char reply[]);
uint8_t uarm_gcode_P2231(char reply[]);
uint8_t uarm_gcode_P2232(char reply[]);
uint8_t uarm_gcode_P2233(char reply[]);
uint8_t uarm_gcode_P2234(char reply[]);

uint8_t uarm_gcode_P2240(char reply[]);
uint8_t uarm_gcode_P2241(char reply[]);
uint8_t uarm_gcode_P2242(char reply[]);

uint8_t uarm_gcode_P2245(char reply[]);
uint8_t uarm_gcode_P2400(char reply[]);


#ifdef SWIFT_TEST_MODE
uint8_t uarm_gcode_P2250(char reply[]);
#endif // SWIFT_TEST_MODE





#endif // _UARMSWIFT_H_
