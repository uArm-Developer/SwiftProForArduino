#ifndef _UARM_COMMON_H_
#define _UARM_COMMON_H_

#include "grbl.h"

#include "uarm_debug.h"
#include "uarm_angle.h"
#include "uarm_timer.h"
#include "uarm_peripheral.h"

// The Atmega2560 has 4KB EEPROM.
//#define EEPROM_ADDR_ANGLE_REFER		 	3072U
#define EEPROM_ADDR_ANGLE_REFER		 	820U

#define EEPROM_ADDR_PARAM 	 				3200U
#define EEPROM_MODE_ADDR						900U
#define EEPROM_HEIGHT_ADDR					910U
#define EEPROM_FRONT_ADDR						920U



enum uarm_work_mode_e {
	WORK_MODE_NORMAL = 0,
	WORK_MODE_LASER,
	WORK_MODE_PRINTER,
	WORK_MODE_PEN,
};

#define DEFAULT_NORMAL_HEIGHT		74.55
#define DEFAULT_NORMAL_FRONT		56.65

#define DEFAULT_LASER_HEIGHT		51.04
#define DEFAULT_LASER_FRONT			64.4

#define DEFAULT_3DPRINT_HEIGHT	74.43
#define DEFAULT_3DPRINT_FRONT		56.5

#define DEFAULT_PEN_HEIGHT			43
#define DEFAULT_PEN_FRONT				69.5 	

#define DEFAULT_GRIPPER_HEIGHT	51.5
#define DEFAULT_GRIPPER_FRONT		105 	

struct key_param_t {
	enum uarm_work_mode_e work_mode;
	float front_offset;
	float high_offset;	
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
	
};

extern struct uarm_state_t uarm;


#endif

