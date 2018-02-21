/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
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



#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
  #error "Oops!  Make sure you have 'Arduino Mega' selected from the 'Tools -> Boards' menu."
#endif

#ifndef BOARD_NAME
  #define BOARD_NAME "Swift 1.0"
#endif

#define LARGE_FLASH true


#define SERVO0_PIN       6

//#define SERVO1_PIN          6
//#define SERVO2_PIN          5
//#define SERVO3_PIN          4

#define X_MIN_PIN           27
#ifndef X_MAX_PIN
  //#define X_MAX_PIN         41
#endif
#define Y_MIN_PIN          42
//#define Y_MAX_PIN          25
#define Z_MIN_PIN          29
//#define Z_MAX_PIN          29
#ifndef Z_MIN_PROBE_PIN
  //#define Z_MIN_PROBE_PIN  32
#endif

#define X_STEP_PIN         54	//S
#define X_DIR_PIN          55	//S
#define X_ENABLE_PIN       38	//S

#define Y_STEP_PIN         60	//S
#define Y_DIR_PIN          61	//S	
#define Y_ENABLE_PIN       56	//S

#define Z_STEP_PIN         46	//S
#define Z_DIR_PIN          48	//S
#define Z_ENABLE_PIN       62	//S

#define E0_STEP_PIN        26	//S
#define E0_DIR_PIN         28	//S
#define E0_ENABLE_PIN      24	//S

#define E1_STEP_PIN        36
#define E1_DIR_PIN         34
#define E1_ENABLE_PIN      30

#define SDSS               53	// sd card cs pin
#define LED_PIN            2	// R 13

#define LED_G			 	3	//G
#define LED_B				5   //B

#define BEEPER_PIN 44

#define KEY_MENU_PIN			//PE6
#define KEY_PLAY_PIN			// PE7
#define KEY_STATE_PIN		11

#define E2PROM_SCL						// 12 PB7
#define E2PROM_SDA						// 13 PB6

#define X_SCL						// PD6
#define X_SDA						// PD5

#define Y_SCL			34			// PC3
#define Y_SDA			36			// PC1

#define Z_SCL						// PH7
#define Z_SDA						// PH2

#define SERVO_HAND_ROT_ANALOG_PIN   A3

#define   POWER_DETECT  A5

// Use the RAMPS 1.4 Analog input 5 on the AUX2 connector
#define FILWIDTH_PIN        -1 // 47	//5 // ANALOG NUMBERING

// define digital pin 4 for the filament runout sensor. Use the RAMPS 1.4 digital input 4 on the servos connector
#define FIL_RUNOUT_PIN      49	// 4

//#define PS_ON_PIN          12	// PS POWER 
#define TEMP_0_PIN         13   // ANALOG NUMBERING
#define TEMP_1_PIN         -1	// 14   // ANALOG NUMBERING
#define TEMP_BED_PIN       15 	//15   // ANALOG NUMBERING

#define HEATER_0_PIN     9


#define FAN_PIN        	8
#define HEATER_BED_PIN 	39

#define VALVE_EN		4
//#define PUMP_EN	//PG4


#define PUMP_FEEDBACK	A9

//#define GRIPPER		 pC7
#define GRIPPER_FEEDBACK	22
//#define HW_PIN_0	


#if ENABLED(Z_PROBE_SLED)
  #define SLED_PIN           -1
#endif




  #define MOSFET_D_PIN  -1






#ifndef FAN_PIN
  #define FAN_PIN 4      // IO pin. Buffer needed
#endif


