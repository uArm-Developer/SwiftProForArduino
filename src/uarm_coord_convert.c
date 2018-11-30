#include "uarm_coord_convert.h"

#if defined(UARM_MINI)
	#define ARM_A 		 (142.07)// Lower arm length
	#define ARM_B 		 (142.07)// Upper arm length
	#define ARM_2AB 	 (2*ARM_A*ARM_B)// 2*A*B
	#define ARM_A2		 (ARM_A*ARM_A)// A^2
	#define ARM_B2		 (ARM_B*ARM_B)// B^2
	#define ARM_A2B2	 (ARM_A2+ARM_B2)// A^2+B^2
	#define Z_BASIC 	 (108.8)
	#define length_center_to_origin 	(13.2)
	#define gearbox_ratio 		(4.5)
	#define micro_steps 			(64.0)
	#define steps_per_angle 	(micro_steps/1.8*gearbox_ratio)
	#define ARMA_MAX_ANGLE					(135.6)
	#define ARMA_MIN_ANGLE					(0)
	#define ARMB_MAX_ANGLE					(119.9)
	#define ARMB_MIN_ANGLE					(0) 
	#define BASE_MAX_ANGLE					(90)
	#define BASE_MIN_ANGLE					(-90)
	#define ARMA_ARMB_MAX_ANGLE 		(151)
	#define ARMA_ARMB_MIN_ANGLE 		(10)
#elif defined(UARM_2500)
	#define ARM_A 		 (300.0)// Lower arm length
	#define ARM_B 		 (300.6)// Upper arm length
	#define ARM_2AB 	 (2*ARM_A*ARM_B)// 2*A*B
	#define ARM_A2		 (ARM_A*ARM_A)// A^2
	#define ARM_B2		 (ARM_B*ARM_B)// B^2
	#define ARM_A2B2	 (ARM_A2+ARM_B2)// A^2+B^2
	#define Z_BASIC 	 (184.5)
	#define length_center_to_origin 	(0.0)
	#define gearbox_ratio 		(100.0)
	#define micro_steps 			(4.0)
	#define steps_per_angle 	(micro_steps/1.8*gearbox_ratio)
	#define ARMA_MAX_ANGLE					(135.6)
	#define ARMA_MIN_ANGLE					(0)
	#define ARMB_MAX_ANGLE					(119.9)
	#define ARMB_MIN_ANGLE					(0) 
	#define BASE_MAX_ANGLE					(90)
	#define BASE_MIN_ANGLE					(-90)
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
	#define ARMA_MIN_ANGLE      		(0)
	#define ARMB_MAX_ANGLE      		(119.9)
	#define ARMB_MIN_ANGLE      		(0)	
	#define BASE_MAX_ANGLE					(90)
	#define BASE_MIN_ANGLE					(-90)
	#define ARMA_ARMB_MAX_ANGLE    	(151)
	#define ARMA_ARMB_MIN_ANGLE    	(10)
#endif

//               /\          |          /              |            \
//       ARM_A  /  \  ARM_B  |    ARM_A/               |             \ ARM_B
//           __/    \        |        /_____  AngleA   |   angleB ____\
//          | center         |                         |
//     origin

/*
	ARMA angle -> anglea
	ARMB angle -> angleb
	BASE angle -> anglec
*/
bool is_angle_legal(float anglea, float angleb, float anglec){

	if( isnan(anglea) || isnan(angleb) || isnan(anglec) ){
		//DB_PRINT_STR("value is nan\r\n");
		return false;
	}
	if( (anglea>ARMA_MAX_ANGLE) || (anglea<ARMA_MIN_ANGLE) ){
		//DB_PRINT_STR("angle_a error\r\n");
		return false;
	}
	if( (angleb>ARMB_MAX_ANGLE) || (angleb<ARMB_MIN_ANGLE) ){
		//DB_PRINT_STR("angle_b error\r\n");
		return false;
	}
	if( (180-anglea-angleb > ARMA_ARMB_MAX_ANGLE) || (180-anglea-angleb < ARMA_ARMB_MIN_ANGLE) ){
		//DB_PRINT_STR("angle_ab error\r\n");
		return false;
	}
	if( (anglec>BASE_MAX_ANGLE) || (anglec<BASE_MIN_ANGLE) ){
		//DB_PRINT_STR("angle_c error\r\n");
		return false;		
	}
	
	return true;
}



void get_current_angle(float x_step, float y_step, float z_step, float *anglea, float *angleb, float *anglec){
	*anglea = (uarm.init_arml_angle + x_step/steps_per_angle) ;
	*angleb = (uarm.init_armr_angle + y_step/steps_per_angle) ;
	*anglec = (uarm.init_base_angle - 90 + z_step/steps_per_angle);
}

void angle_to_coord(float anglea, float angleb, float anglec, float *x, float *y, float *z){
	float radiana = anglea / RAD_TO_DEG; 		// <! angle to radin
	float radianb = angleb / RAD_TO_DEG;
	float radianc = anglec / RAD_TO_DEG;

	float _stretch = cos(radiana) * ARM_A + cos(radianb) * ARM_B;
  float _height = sin(radiana) * ARM_A - sin(radianb) * ARM_B;

  *x = (_stretch + length_center_to_origin) * cos(radianc);
  *y = (_stretch + length_center_to_origin) * sin(radianc);
  *z = _height + Z_BASIC;
}

void coord_to_angle(float x, float y, float z, float *anglea, float *angleb, float *anglec){		


/*	DB_PRINT_STR("coord xyz:");
	DB_PRINT_FLOAT(x);DB_PRINT_STR(" ");
	DB_PRINT_FLOAT(y);DB_PRINT_STR(" ");
	DB_PRINT_FLOAT(z);DB_PRINT_STR("\r\n"); 	*/



	double _stretch = sqrt(x * x + y * y) - length_center_to_origin;  //_stretch = sqrt(x^2+y^2)
  double _height = z - Z_BASIC;

	// get angle A
	double xx = _stretch * _stretch + _height * _height;
  double xxx = ARM_A2 - ARM_B2 + xx;

	
  *anglea = acos((_stretch * xxx - _height * sqrt(4.0 * ARM_A2 * xx - xxx * xxx)) / (xx * 2.0 * ARM_A)) * RAD_TO_DEG;

  // get angle B
 	xxx = ARM_B2 -ARM_A2 + xx;
  *angleb = acos((_stretch * xxx + _height * sqrt(4.0 * ARM_B2 * xx -xxx * xxx)) / (xx * 2.0 * ARM_B)) * RAD_TO_DEG;
  if( _height > (ARM_A *sin(*anglea/RAD_TO_DEG)) ){
    *angleb = -*angleb;
  }

  // get the base rotation angle
  
	if( x == 0 ){
		if(y > 0){
			*anglec = 90;
		}else{
			*anglec = -90;
		}
	}else if(x <0){
		*anglec = NAN;	// <! cann't arrive
	}else{
		*anglec = atan(y / x) * RAD_TO_DEG;
	}	
	

/*	DB_PRINT_STR("coord abc:");
	DB_PRINT_FLOAT(*anglea);DB_PRINT_STR(" ");
	DB_PRINT_FLOAT(*angleb);DB_PRINT_STR(" ");
	DB_PRINT_FLOAT(*anglec);DB_PRINT_STR("\r\n");*/
}

void get_current_step(float anglea, float angleb, float anglec, float *x_step, float *y_step, float *z_step){
  *x_step = (int)((anglea - uarm.init_arml_angle) * steps_per_angle);
  *y_step = (int)((angleb - uarm.init_armr_angle) * steps_per_angle);
  *z_step = (int)((anglec - uarm.init_base_angle + 90) * steps_per_angle);	
}

void step_to_coord(float x_step, float y_step, float z_step, float *x, float *y, float *z){
	float anglea, angleb, anglec;
	get_current_angle( x_step, y_step, z_step, &anglea, &angleb, &anglec );
	angle_to_coord( anglea, angleb, anglec, x, y, z );
}


void coord_to_step(float x, float y, float z, float *x_step, float *y_step, float *z_step){
	float anglea, angleb, anglec;
	coord_to_angle( x, y, z, &anglea, &angleb, &anglec );
	get_current_step( anglea, angleb, anglec, x_step, y_step, z_step );
}

void coord_arm2effect(float *x, float *y, float *z){
	float anglec;

	if( *x == 0 ){
		if(*y > 0){
			anglec = 90 / RAD_TO_DEG;
		}else{
			anglec = -90 / RAD_TO_DEG;
		}
	}else if(*x < 0){
		anglec = NAN;	// <! cann't arrive
	}else{
		anglec = atan(*y / *x);
	}	

	*x += uarm.param.front_offset * cos(anglec); 
	*y += uarm.param.front_offset * sin(anglec);
	*z -= uarm.param.high_offset;	

}

void coord_effect2arm(float *x, float *y, float *z){
	float anglec;

	if( *x == 0 ){
		if(*y > 0){
			anglec = 90 / RAD_TO_DEG;
		}else{
			anglec = -90 / RAD_TO_DEG;
		}
	}else if(*x < 0){
		anglec = NAN;	// <! cann't arrive
	}else{
		anglec = atan(*y / *x);
	}	

	*x -= uarm.param.front_offset * cos(anglec); 
	*y -= uarm.param.front_offset * sin(anglec);
	*z += uarm.param.high_offset;		
}


