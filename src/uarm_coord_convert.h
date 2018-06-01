#ifndef _UARM_COORD_CONVERT_H_
#define _UARM_COORD_CONVERT_H_

#include "uarm_common.h"

#define RAD_TO_DEG (57.29578)// 180/PI



void get_current_angle(float x_step, float y_step, float z_step, float *anglea, float *angleb, float *anglec);
void angle_to_coord(float anglea, float angleb, float anglec, float *x, float *y, float *z);
void coord_to_angle(float x, float y, float z, float *anglea, float *angleb, float *anglec);
void get_current_step(float anglea, float angleb, float anglec, float *x_step, float *y_step, float *z_step);

void step_to_coord(float x_step, float y_step, float z_step, float *x, float *y, float *z);
void coord_to_step(float x, float y, float z, float *x_step, float *y_step, float *z_step);
bool is_angle_legal(float anglea, float angleb, float anglec);

void coord_arm2effect(float *x, float *y, float *z);
void coord_effect2arm(float *x, float *y, float *z);


#endif

