#ifndef _UARM_COORD_CONVERT_H_
#define _UARM_COORD_CONVERT_H_

#include "uarm_common.h"

#define RAD_TO_DEG (57.29578)// 180/PI

#if defined(UARM_MINI)
	#define ARM_A 		 (157.0)// Lower arm length
	#define ARM_B 		 (157.0)// Upper arm length
	#define ARM_2AB 	 (2*ARM_A*ARM_B)// 2*A*B
	#define ARM_A2		 (ARM_A*ARM_A)// A^2
	#define ARM_B2		 (ARM_B*ARM_B)// B^2
	#define ARM_A2B2	 (ARM_A2+ARM_B2)// A^2+B^2
	#define Z_BASIC 	 (116.55)
	#define length_center_to_origin 	(13.2)
	#define gearbox_ratio 		(4.5)
	#define micro_steps 			(32.0)
	#define steps_per_angle 	(micro_steps/1.8*gearbox_ratio)
	#define ARMA_MAX_ANGLE					(100.0)
	#define ARMA_MIN_ANGLE					(0)
	#define ARMB_MAX_ANGLE					(110.0) //110
	#define ARMB_MIN_ANGLE					(1)
	#define BASE_MAX_ANGLE					(90)
	#define BASE_MIN_ANGLE					(-90)
	#define ARMA_ARMB_MAX_ANGLE 		(137)
	#define ARMA_ARMB_MIN_ANGLE 		(18)

#elif defined(UARM_2500)
	#define ARM_A 		 (250.0)// Lower arm length
	#define ARM_B 		 (250.0)// Upper arm length
	#define ARM_2AB 	 (2*ARM_A*ARM_B)// 2*A*B
	#define ARM_A2		 (ARM_A*ARM_A)// A^2
	#define ARM_B2		 (ARM_B*ARM_B)// B^2
	#define ARM_A2B2	 (ARM_A2+ARM_B2)// A^2+B^2
	#define Z_BASIC 	 (189.0)
	#define length_center_to_origin 	(0.0)
	#define gearbox_ratio 		(100.0)
	#define micro_steps 			(4.0)
	#define steps_per_angle 	(micro_steps/1.8*gearbox_ratio)
	#define ARMA_MAX_ANGLE					(135.6)
	#define ARMA_MIN_ANGLE					(0)
	#define ARMB_MAX_ANGLE					(119.9)
	#define ARMB_MIN_ANGLE					(0) 
	#define BASE_MAX_ANGLE					(88)
	#define BASE_MIN_ANGLE					(-88)
	#define ARMA_ARMB_MAX_ANGLE 		(151)
	#define ARMA_ARMB_MIN_ANGLE 		(10)
#else
	#define ARM_A      (142.07)// Lower arm length
	#define ARM_B      (158.81)// Upper arm length
	#define ARM_2AB    (2*ARM_A*ARM_B)// 2*A*B
	#define ARM_A2     (ARM_A*ARM_A)// A^2
	#define ARM_B2     (ARM_B*ARM_B)// B^2
	#define ARM_A2B2   (ARM_A2+ARM_B2)// A^2+B^2
	#define Z_BASIC		 (107.4)
	#define length_center_to_origin   (13.2)
	#define gearbox_ratio     (4.5)
	#define micro_steps       (32.0)
	#define steps_per_angle   (micro_steps/1.8*gearbox_ratio)
	#define ARMA_MAX_ANGLE      		(135.6)
	#define ARMA_MIN_ANGLE      		(-4.0)
	#define ARMB_MAX_ANGLE      		(119.9)
	#define ARMB_MIN_ANGLE      		(-4.0)	
	#define BASE_MAX_ANGLE					(90)
	#define BASE_MIN_ANGLE					(-90)
	#define ARMA_ARMB_MAX_ANGLE    	(151)
	#define ARMA_ARMB_MIN_ANGLE    	(10)
#endif


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

