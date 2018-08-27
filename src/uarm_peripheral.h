#ifndef _UARM_PERIPHERAL_H_
#define _UARM_PERIPHERAL_H_

#include "uarm_common.h"

extern char hardware_version[8]; 


void end_effector_init(void);
void end_effector_set_angle(float angle);
void end_effector_deinit(void);
float end_effector_get_angle(void);
void end_effector_check_limit(void);
void end_effect_adjust_angle(void);



void beep_tone(unsigned long duration, double frequency);

void gcode_cmd_delay(int ms);
void cycle_report_start(int ms);
void cycle_report_stop(void);

void read_sys_param(void);
void save_sys_param(void);
void cycle_report_coord(void);
void read_hardware_version(void);

void pump_on(void);
void pump_off(void);
uint8_t get_pump_status(void);

void gripper_relesae(void);
void gripper_catch(void);
uint8_t get_gripper_status(void);

void laser_on(void);
void laser_off(void);
uint8_t get_laser_status(void);



uint8_t get_limit_switch_status(void);
uint8_t get_power_status(void);

void check_motor_positon(void);




	
#endif
