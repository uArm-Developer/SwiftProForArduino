#ifndef _UARM_COMMON_H_
#define _UARM_COMMON_H_

#include "grbl.h"

#include <stdio.h>
#include <Arduino.h>

#include "uarm_debug.h"
#include "uarm_angle.h"
#include "uarm_timer.h"
#include "uarm_peripheral.h"

// The Atmega2560 has 4KB EEPROM.
//#define EEPROM_ADDR_ANGLE_REFER		 	3072U

//#define UARM_MINI
#define UARM_2500


#define EEPROM_ADDR_ANGLE_REFER		 				820U
#define EEPROM_OLD_ADDR_ANGLE_REFER		 		800U
#define EEPROM_ANGLE_REFER_FLAG						818U
#define EEPROM_BT_MAC_ADDR								1002U

#define EEPROM_ADDR_PARAM 	 							3200U				// refer to v3.2.0 firmware
#define EEPROM_MODE_ADDR									900U
#define EEPROM_HEIGHT_OFFSET_ADDR					910U
#define EEPROM_FRONT_OFFSET_ADDR					920U
#define EEPROM_EFFECT_ANGLE_OFFSET_ADDR 	3202U

// <! 600 bytes
#define EEPROM_ADDR_ANGLE_CAL_MAP					1U


enum uarm_work_mode_e {
	WORK_MODE_NORMAL = 0,
	WORK_MODE_LASER,
	WORK_MODE_PRINTER,
	WORK_MODE_PEN,
	WORK_MODE_STEPER_FLAT,									// <! flat steper mode
	WORK_MODE_STEPER_STANDARD,							// <! standard steper mode
	WORK_MODE_TOUCH_PEN,
	WORK_MODE_TEST,
};

#if defined(UARM_2500)
	#define DEFAULT_NORMAL_HEIGHT 	74.55
	#define DEFAULT_NORMAL_FRONT		56.65

	#define DEFAULT_LASER_HEIGHT		51.04
	#define DEFAULT_LASER_FRONT 		64.4

	#define DEFAULT_3DPRINT_HEIGHT	74.43
	#define DEFAULT_3DPRINT_FRONT 	56.5

	#define DEFAULT_PEN_HEIGHT			43
	#define DEFAULT_PEN_FRONT 			69.5	

	#define DEFAULT_STEP_FLAT_HEIGHT		39
	#define DEFAULT_STEP_FLAT_FRONT 		76.5

	#define DEFAULT_STEP_STANDARD_HEIGHT		27.5
	#define DEFAULT_STEP_STANDARD_FRONT 		75.5

	#define DEFAULT_ROUND_PEN_HEIGHT		77.2
	#define DEFAULT_ROUND_PEN_FRONT 		54.5

	#define DEFAULT_TEST_HEIGHT 		25.49
	#define DEFAULT_TEST_FRONT			44.5

#else
	#define DEFAULT_NORMAL_HEIGHT		74.55
	#define DEFAULT_NORMAL_FRONT		56.65

	#define DEFAULT_LASER_HEIGHT		51.04
	#define DEFAULT_LASER_FRONT			64.4

	#define DEFAULT_3DPRINT_HEIGHT	74.43
	#define DEFAULT_3DPRINT_FRONT		56.5

	#define DEFAULT_PEN_HEIGHT			43
	#define DEFAULT_PEN_FRONT				69.5 	

	#define DEFAULT_STEP_FLAT_HEIGHT   	39
	#define DEFAULT_STEP_FLAT_FRONT		 	76.5

	#define DEFAULT_STEP_STANDARD_HEIGHT   	73.4
	#define DEFAULT_STEP_STANDARD_FRONT		 	76

	#define DEFAULT_ROUND_PEN_HEIGHT   	77.2
	#define DEFAULT_ROUND_PEN_FRONT		 	54.5

	#define DEFAULT_TEST_HEIGHT   	25.49
	#define DEFAULT_TEST_FRONT		 	44.5
#endif





struct key_param_t {
	enum uarm_work_mode_e work_mode;
	float front_offset;
	float high_offset;
	float effect_angle_offset;
};

struct uarm_state_t {
	struct key_param_t param;
	
	double init_arml_angle;
	double init_armr_angle;
	double init_base_angle;	

	int32_t target_step[3];

	float coord_x;
	float coord_y;
	float coord_z;
	
	char motor_state_bits;
	volatile bool gcode_delay_flag;
	volatile bool cycle_report_flag;
	volatile bool run_done_report_flag;
	volatile bool run_flag;

	bool effect_origin_check;
	bool effect_ldie;
	bool beep_ldie;
	bool power_state;
	bool motor_position_check;
	
};

extern struct uarm_state_t uarm;


#endif

