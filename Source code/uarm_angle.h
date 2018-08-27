#ifndef _UARM_ANGLE_H_
#define _UARM_ANGLE_H_

#include "uarm_common.h"

enum angle_channel_e {
	CHANNEL_ARML = 0,
	CHANNEL_ARMR,
	CHANNEL_BASE,
};

// <! iic lowlevel 
void angle_sensor_init(void);
float calculate_current_angle(enum angle_channel_e channel);
void get_refer_value(uint16_t *value);


void single_point_reference(void);
void multi_point_reference(uint8_t param);
bool calculate_refer_write_eeprom(void);



#endif
