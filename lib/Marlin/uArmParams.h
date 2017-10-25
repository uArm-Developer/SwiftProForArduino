/**
  ******************************************************************************
  * @file	uArmParams.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-15
  ******************************************************************************
  */

#ifndef _UARMPARAMS_H_
#define _UARMPARAMS_H_






#define TICK_INTERVAL	50

#define EEPROM_X_ANGLE_ADDR	100
#define EEPROM_Y_ANGLE_ADDR	110
#define EEPROM_Z_ANGLE_ADDR	120

// e2prom device
#define EEPROM_ON_CHIP			0
#define EEPROM_EXTERN_USER		1
#define EEPROM_EXTERN_SYSTEM	2

// e2prom data type
#define DATA_TYPE_BYTE          1
#define DATA_TYPE_INTEGER       2
#define DATA_TYPE_FLOAT         4

#define EXTERNAL_EEPROM_USER_ADDRESS 0xa0
#define EXTERNAL_EEPROM_SYS_ADDRESS 0xa2

#define SERVO_9G_MAX    460
#define SERVO_9G_MIN    98


#define EEPROM_REFERENCE_VALUE_ADDR		800

#define EEPROM_REFERENCE_VALUE_B_FLAG_ADDR		818
#define EEPROM_REFERENCE_VALUE_B_ADDR		820


#define EEPROM_MODE_ADDR	900
#define EEPROM_HEIGHT_ADDR	910
#define EEPROM_FRONT_ADDR	920

#define EEPROM_WRITE_TEST_ADDR	1992

#define EEPROM_ACCELERATION_FLAG_ADDR 1020
#define EEPROM_PRINT_ACCELERATION_ADDR 1030
#define EEPROM_RETRACT_ACCELERATION_ADDR 1050
#define EEPROM_TRAVEL_ACCELERATION_ADDR 1070
#define EEPROM_MAX_XY_JERK_ADDR 1090
#define EEPROM_MAX_Z_JERK_ADDR 1100

#define EEPROM_ACCELERATION_FLAG 0XC9


#define MODE_LASER_ACCELERATION 		25
#define MODE_LASER_TRAVEL_ACCELERATION  25
#define MODE_LASER_MAX_XY_JERX          0.5
#define MODE_LASER_MAX_Z_JERX 			0.5

#define MODE_3D_PRINT_ACCELERATION 		  25
#define MODE_3D_PRINT_TRAVEL_ACCELERATION 25
#define MODE_3D_PRINT_MAX_XY_JERX 		  0.5
#define MODE_3D_PRINT_MAX_Z_JERX 		  0.5

#define MODE_PEN_ACCELERATION 			25
#define MODE_PEN_TRAVEL_ACCELERATION 	25
#define MODE_PEN_MAX_XY_JERX 			0.5
#define MODE_PEN_MAX_Z_JERX 			0.5

#define MODE_NORMAL_ACCELERATION 		200
#define MODE_NORMAL_TRAVEL_ACCELERATION 200
#define MODE_NORMAL_MAX_XY_JERX 		5
#define MODE_NORMAL_MAX_Z_JERX			5


#define MATH_PI 			3.141592653589793238463
#define MATH_TRANS  		57.2958    
#define MATH_L1 			106.6	//90.00	
#define MATH_L2 			13.2		//21.17	
#define MATH_LOWER_ARM 		142.07	//148.25	
#define MATH_UPPER_ARM 		158.81	//158.8	//160.2 	
//#define MATH_FRONT_HEADER 	69//(29.4+38.6)//43.75)	//25.00// the distance between wrist to the front point we use
#define MATH_UPPER_LOWER 	MATH_UPPER_ARM/MATH_LOWER_ARM

// gripper or pump status
#define STOP            		0
#define WORKING          		1
#define GRABBING        		2

#define PUMP_GRABBING_CURRENT 	70  

#define RESULT_BUFFER_SIZE 128

#define E_OK	0
#define E_PARAMETERS_WRONG	21
#define E_OUT_OF_RANGE	22
#define E_BUFFER_FULL	23
#define E_NO_POWER		24
#define E_FAIL			25
#define E_ROUTINE_UNAVAILABLE	26


#define LOWER_ARM_MAX_ANGLE      135.6
#define LOWER_ARM_MIN_ANGLE      0
#define UPPER_ARM_MAX_ANGLE      119.9
#define UPPER_ARM_MIN_ANGLE      0
#define LOWER_UPPER_MAX_ANGLE    151
#define LOWER_UPPER_MIN_ANGLE    10

#define BASE_HEIGHT		71
#define POLAR_MODULE_LENGTH_MIN_ABOVE_BASE	48
#define POLAR_MODULE_LENGTH_MIN				72


#define BT_NAME_MAX_LEN		11

#define UNLOCK_NUM	22765

#define REPORT_TYPE_POS		3

#define REPORT_MOVING_STOP	9


#define REPORT_TYPE_GROVE	10
#define REPORT_TYPE_GROVE2	11


#define SERVO_HW_FIX_VER	2
#define PUMP_HW_FIX_VER		1


#endif // _UARMPARAMS_H_
