/**
  ******************************************************************************
  * @file	uArmAPI.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-01-21
  ******************************************************************************
  */

#ifndef _UARMAPI_H_
#define _UARMAPI_H_

#include <Arduino.h>
#include <EEPROM.h>
#include "uArmParams.h"
#include "uArmIIC.h"
#include "Marlin.h"

#define OUT_OF_RANGE		10
#define NO_SUCH_CMD			20
#define PARAMETER_ERROR		21
#define ADDRESS_ERROR		22


#define DEFAULT_NORMAL_HEIGHT	74.55
#define DEFAULT_NORMAL_FRONT	56.65

#define DEFAULT_LASER_HEIGHT	51.04
#define DEFAULT_LASER_FRONT		64.4

#define DEFAULT_3DPRINT_HEIGHT	74.43
#define DEFAULT_3DPRINT_FRONT	56.5

#define DEFAULT_PEN_HEIGHT	74.43
#define DEFAULT_PEN_FRONT	69.5 	//92.8	


typedef enum {
	USER_MODE_NORMAL,
	USER_MODE_LASER,
	USER_MODE_3D_PRINT,
	USER_MODE_PEN,

	USER_MODE_COUNT

} UserMode_t;

double getE2PROMData(unsigned char device, unsigned int addr, unsigned char type);

double setE2PROMData(unsigned char device, unsigned int addr, unsigned char type, double value);

void swift_api_init();

/*!
   \brief check if power plug in
 */
bool isPowerPlugIn();

/*!
   \brief get analog value of pin
   \param pin of arduino
   \return value of analog data
 */
int getAnalogPinValue(unsigned int pin);

/*!
   \brief Set working mode and set parameters according to the mode.
 */
void set_user_mode(UserMode_t mode);

/*!
   \brief Set acceleration by working mode.
 */
void set_acceleration(UserMode_t mode);

/*!
   \brief Get the working mode set befored or set to default.
 */
void init_user_mode();

/*!
   \brief Get current working mode
 */
UserMode_t get_user_mode();

/*!
   \brief set front end offset(mm)
 */
void set_front_end_offset(float offset);

/*!
   \brief get front end offset(mm)
 */
float get_front_end_offset();

/*!
   \brief set height end offset(mm)
 */
void set_height_offset(float offset);

/*!
   \brief get height end offset(mm)
 */
float get_height_offset();

unsigned char getXYZFromAngle(float& x, float& y, float& z, float rot, float left, float right);

/*!
   \brief pump working
 */
void pumpOn();

/*!
   \brief pump stop
 */
void pumpOff();


/*!
   \brief gripper work
 */
void gripperCatch();

/*!
   \brief gripper stop
 */
void gripperRelease();

/*!
   \brief get gripper status
   \return STOP if gripper is not working
   \return WORKING if gripper is working but not catched sth
   \return GRABBING if gripper got sth   
 */
unsigned char getGripperStatus();

/*!
   \brief get pump status
   \return STOP if pump is not working
   \return WORKING if pump is working but not catched sth
   \return GRABBING if pump got sth   
 */
unsigned char getPumpStatus();

/*!
   \brief get hardware subvervion
 */
unsigned char getHWSubversion();


/*!
   \brief get mac of bt which as unique code of HW
 */
unsigned char* getMac();

/*!
   \brief get the status of tip 
   return: 1. the switch is on
		   0. the switch if off
 */
unsigned char getTip();

/*!
   \brief get the status of user defined key 
   return: 1. the key is down
		   0. the key is up
 */
unsigned char getSwitchState();

/*!
   \brief get height offset from E2PROM(mm)
 */
float read_height_offset();

/*!
   \brief Save height offset to E2PROM(mm)
 */
void save_height_offset(float offset);

/*!
   \brief get front offset from E2PROM(mm)
 */
float read_front_offset();

/*!
   \brief Save front offset to E2PROM(mm)
 */
void save_front_offset(float offset);

/*!
   \brief get user mode from E2PROM(mm)
 */
uint8_t read_user_mode();

/*!
   \brief Save user mode to E2PROM(mm)
 */
void save_user_mode(uint8_t mode);

/*!
   \brief get Print acceleration from E2PROM(mm)
 */
float read_print_acceleration();

/*!
   \brief Save Print acceleration to E2PROM(mm)
 */
void save_print_acceleration(float acceleration);

/*!
   \brief get Retract acceleration from E2PROM(mm)
 */
float read_retract_acceleration();

/*!
   \brief Save Retract acceleration to E2PROM(mm)
 */
void save_retract_acceleration(float acceleration);

/*!
   \brief get Travel acceleration from E2PROM(mm)
 */
float read_travel_acceleration();

/*!
   \brief Save Travel acceleration to E2PROM(mm)
 */
void save_travel_acceleration(float acceleration);

/*!
   \brief cheack  data from E2PROM(mm) if null return false or true
 */
bool cheack_acceleration_jerk();

/*!
   \brief Save acceleration_jerk to E2PROM(mm)
 */
void save_P_T_acceleration(float p_acceleration,float t_acceleration);

/*!
   \brief read acceleration flag from E2PROM(mm)
 */
uint8_t read_acceleration_flag();

/*!
   \brief save acceleration flag to E2PROM(mm)
 */
void save_acceleration_flag(uint8_t flag);

/*!
   \brief clear acceleration flag from E2PROM(mm)
 */
void clear_acceleration_flag();



/*!
   \brief get max xy jerk from E2PROM(mm)
 */
float read_max_xy_jerx();

/*!
   \brief Save max xy jerk to E2PROM(mm)
 */
void save_max_xy_jerk(float max_xy_jerk);
/*!
   \brief get max z jerk from E2PROM(mm)
 */
float read_max_z_jerx();
/*!
   \brief Save max z jerk to E2PROM(mm)
 */
void save_max_z_jerk(float max_z_jerk);


/*!
   \brief clear mac flag stored in E2PROM, it will read mac from Bluetooth when power up.
 */
void clearMacFlag();

/*!
   \brief set bluetooth broadcast name
   \return true if success
 */
bool setBtName(char btName[]);

/*!
   \brief test e2prom available
   \param device:  EEPROM_ON_CHIP, EEPROM_EXTERN_USER, EEPROM_EXTERN_SYSTEM
   \return: true if available
 */
bool eeprom_write_test(uint8_t device);

/*!
   \brief read mac string from BT if not stored in e2prom.
 */
void getMacAddr();


#endif // _UARMAPI_H_
