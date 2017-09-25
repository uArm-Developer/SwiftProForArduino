/**
  ******************************************************************************
  * @file	uArmAPI.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-01-21
  ******************************************************************************
  */

#include "uArmAPI.h" 
#include "Grovergb_lcd.h"

static float front_end_offset = 0.0;
static float height_offset = 0.0;

UserMode_t user_mode = USER_MODE_NORMAL;

#define MAC_FLAG_ADDRESS 1000
#define MAC_LEN	12
#define MAC_FLAG_SET	0XCE
static unsigned char mMacStr[MAC_LEN+1] = {0};
static unsigned char mHWSubVersion;

/*!
   \brief get mac of bt which as unique code of HW
 */
unsigned char* getMac()
{
	return mMacStr;
}

void init_hardware()
{
	// PG4 output
	DDRG |= 0x10;	// PUMP_D5_N

	// PC7 input
	//DDRC &= 0x7F;

	//PORTC |= 0X80;
	DDRC |= 0x80;

	// tip PG3 input
	DDRG &= 0xF7;	// 
	PORTG |= 0X08;

	pinMode(GRIPPER_FEEDBACK, INPUT);
	pinMode(VALVE_EN, OUTPUT);
	pinMode(KEY_STATE_PIN, INPUT);
	
}

/*!
   \brief read mac string from BT if not stored in e2prom.
 */
void getMacAddr()
{
	// get BT MAC
	// if not set mac flag, get mac from bt
	delay(10);
	unsigned char macFlag = getE2PROMData(EEPROM_ON_CHIP, MAC_FLAG_ADDRESS, DATA_TYPE_BYTE);
	debugPrint("mac flag =%d\r\n", macFlag);
	// if flag set then read it from EEPROM
	if (macFlag == MAC_FLAG_SET)
	{
		delay(10);
		//iic_readbuf(mMacStr, EEPROM_ON_CHIP, MAC_ADDRESS, MAC_LEN);// write data
	
		for (int i = 0; i < MAC_LEN; i++)
		{
			mMacStr[i] = EEPROM.read(MAC_FLAG_ADDRESS+2+i);
		}
		mMacStr[MAC_LEN] = '\0';
		debugPrint("mac=%s\r\n", mMacStr);
	}
	else
	{
		unsigned char readBuf[128];
		int i = 0;
		// get from BT
		Serial1.write("AT+ADDR?");

		delay(100);

		// set firmware flag
		Serial1.write("AT+UUID0xFFE3");

		// wait for reply
		delay(100);

		// get data
		while (Serial1.available())
		{
			readBuf[i++] = Serial1.read();
		}

		readBuf[i] = '\0';
		debugPrint("%s\r\n", readBuf);

		// expect data: OK+ADDR:00FF00FF00FF
		// parse datas
		char *s = strstr(readBuf, "OK+ADDR:");

		if (s != NULL)
		{
			//found
			strncpy(mMacStr, s+8, MAC_LEN);
			mMacStr[MAC_LEN] = '\0';
			debugPrint("mac=%s\r\n", mMacStr);

			delay(10);
			// store in EEPROM
			//iic_writebuf(mMacStr, EXTERNAL_EEPROM_SYS_ADDRESS, MAC_ADDRESS, MAC_LEN);
			for (int i = 0; i < MAC_LEN; i++)
			{
				EEPROM.write(MAC_FLAG_ADDRESS+2+i, mMacStr[i] );
			}
			delay(10);
			// set mac flag
			setE2PROMData(EEPROM_ON_CHIP, MAC_FLAG_ADDRESS, DATA_TYPE_BYTE, MAC_FLAG_SET);
		}

	}	
}

/*!
   \brief Get hardware subversion defined by 4 GPIOs.
 */
void getHWVersions()
{

	DDRJ &= 0x87;	//input



	mHWSubVersion = 0;
	unsigned char bit = 1;
	// get hw version
	if (PINJ & 0x40)
	{
		mHWSubVersion |= bit;
	}

	bit <<= 1;
	if (PINJ & 0x20)
	{
		mHWSubVersion |= bit;
	}

	bit <<= 1;
	if (PINJ & 0x10)
	{
		mHWSubVersion |= bit;
	}

	bit <<= 1;
	if (PINJ & 0x08)
	{
		mHWSubVersion |= bit;
	}

	debugPrint("subversion:%d\r\n", mHWSubVersion);

}

/*!
   \brief clear mac flag stored in E2PROM, it will read mac from Bluetooth when power up.
 */
void clearMacFlag()
{
	setE2PROMData(EEPROM_ON_CHIP, MAC_FLAG_ADDRESS, DATA_TYPE_BYTE, 0);
	mMacStr[0] = '\0';
}


void swift_api_init()
{
	Serial1.begin(115200);
	//Serial2.begin(115200);

	getMacAddr();
	getHWVersions();

	init_hardware();
	
}

/*!
   \brief Get the working mode set befored or set to default.
 */
void init_user_mode()
{
	uint8_t mode = read_user_mode();

	if (isnan(mode) || mode >= USER_MODE_COUNT)
	{
		mode = USER_MODE_NORMAL;
		save_user_mode(mode);
	}


	float height = read_height_offset();

	if (isnan(height) || height > 150 || height < 0)
	{
		switch(mode)
		{
		case USER_MODE_LASER:
			height = DEFAULT_LASER_HEIGHT;
			break;
		
		case USER_MODE_3D_PRINT:
			height = DEFAULT_3DPRINT_HEIGHT;	
			break;
		
		case USER_MODE_PEN:
			height = DEFAULT_PEN_HEIGHT;	
			break;		
		
		case USER_MODE_NORMAL:	
		default:
			height = DEFAULT_NORMAL_HEIGHT;	
			break;
		}

		save_height_offset(height);

	}

	float front = read_front_offset();

	if (isnan(front) || front > 150 || front < 0)
	{
		switch(mode)
		{
		case USER_MODE_LASER:
			front = DEFAULT_LASER_FRONT;
			break;
		
		case USER_MODE_3D_PRINT:
			front = DEFAULT_3DPRINT_FRONT;	
			break;

		case USER_MODE_PEN:
			front = DEFAULT_PEN_FRONT;	
			break;			
		
		case USER_MODE_NORMAL:	
		default:
			front = DEFAULT_NORMAL_FRONT;	
			break;
		}

		save_front_offset(front);

	}	

	debugPrint("acceleration_flag:%d\n",read_acceleration_flag());

	if((read_acceleration_flag() != EEPROM_ACCELERATION_FLAG)) 
	{
		set_acceleration(mode);
		save_acceleration_flag(EEPROM_ACCELERATION_FLAG);
	}
	else
	{
		planner.acceleration = read_print_acceleration();
		planner.travel_acceleration = read_travel_acceleration();
		planner.max_xy_jerk = read_max_xy_jerx();
		planner.max_z_jerk = read_max_z_jerx();
	}
	
	debugPrint("acceleration P:%f T:%f max_xy_jerk:%f max_z_jerk:%f\r\n",planner.acceleration,planner.travel_acceleration,planner.max_xy_jerk,planner.max_z_jerk);

	user_mode = mode;
	front_end_offset = front;
	height_offset = height;

	set_fan_function(user_mode == USER_MODE_3D_PRINT);
	set_heater_function(user_mode == USER_MODE_3D_PRINT);

	debugPrint("Mode=%d, height=%f, front=%f\r\n", mode, height, front);
	
}

/*!
   \brief Set acceleration by working mode.
 */
void set_acceleration(UserMode_t mode)
{
	if (mode >= USER_MODE_COUNT)
		return;


		switch(mode)
		{
		case USER_MODE_LASER:

			planner.acceleration = MODE_LASER_ACCELERATION;
			planner.travel_acceleration = MODE_LASER_TRAVEL_ACCELERATION;
			planner.max_xy_jerk = MODE_LASER_MAX_XY_JERX;
			planner.max_z_jerk = MODE_LASER_MAX_Z_JERX;
			
			break;

		case USER_MODE_3D_PRINT:

			planner.acceleration = MODE_3D_PRINT_ACCELERATION;
			planner.travel_acceleration = MODE_3D_PRINT_TRAVEL_ACCELERATION;
			planner.max_xy_jerk = MODE_3D_PRINT_MAX_XY_JERX;
			planner.max_z_jerk = MODE_3D_PRINT_MAX_Z_JERX;		
			break;

		case USER_MODE_PEN:

			planner.acceleration = MODE_PEN_ACCELERATION;
			planner.travel_acceleration = MODE_PEN_TRAVEL_ACCELERATION;
			planner.max_xy_jerk = MODE_PEN_MAX_XY_JERX;
			planner.max_z_jerk = MODE_PEN_MAX_Z_JERX;		
			break;		

		case USER_MODE_NORMAL:	
		default:

			planner.acceleration = MODE_NORMAL_ACCELERATION;
			planner.travel_acceleration = MODE_NORMAL_TRAVEL_ACCELERATION;
			planner.max_xy_jerk = MODE_NORMAL_MAX_XY_JERX;
			planner.max_z_jerk = MODE_NORMAL_MAX_Z_JERX;	
			break;
		}

	save_P_T_acceleration(planner.acceleration,planner.travel_acceleration);
	delay(10);
	save_max_xy_jerk(planner.max_xy_jerk);
	delay(10);
	save_max_z_jerk(planner.max_z_jerk);
	debugPrint("acceleration set P:%f T:%f max_xy_jerk:%f max_z_jerk:%f\r\n",planner.acceleration,planner.travel_acceleration,planner.max_xy_jerk,planner.max_z_jerk);


}


/*!
   \brief Set working mode and set parameters according to the mode.
 */
void set_user_mode(UserMode_t mode)
{
	if (mode >= USER_MODE_COUNT)
		return;

	user_mode = mode;

	switch(mode)
	{
	case USER_MODE_LASER:
		front_end_offset = DEFAULT_LASER_FRONT;
		height_offset = DEFAULT_LASER_HEIGHT;
		set_fan_function(false);
		set_heater_function(false);
		break;

	case USER_MODE_3D_PRINT:
		front_end_offset = DEFAULT_3DPRINT_FRONT;
		height_offset = DEFAULT_3DPRINT_HEIGHT;	
		set_fan_function(true);
		set_heater_function(true);
		break;

	case USER_MODE_PEN:
		front_end_offset = DEFAULT_PEN_FRONT;
		height_offset = DEFAULT_PEN_HEIGHT;	
		set_fan_function(false);
		set_heater_function(false);
		break;		

	case USER_MODE_NORMAL:	
	default:
		front_end_offset = DEFAULT_NORMAL_FRONT;
		height_offset = DEFAULT_NORMAL_HEIGHT;	
		set_fan_function(false);
		set_heater_function(false);
		break;
	}

	set_acceleration(mode);

	save_user_mode(mode);
	save_height_offset(height_offset);
	save_front_offset(front_end_offset);
	
}

/*!
   \brief set bluetooth broadcast name
   \return true if success
 */
bool setBtName(char btName[])
{
	int strLength = strlen(btName);

	if (btName == NULL || strLength == 0)
		return false;

	if (strLength > BT_NAME_MAX_LEN)
	{
		btName[BT_NAME_MAX_LEN] = '\0';
	}

	// bluetooth is working
	if (PINE & 0x04)
	{
		return false;
	}

	Serial1.write("AT+NAME");
	Serial1.write(btName);

	delay(100);

	
	// set firmware flag
	Serial1.write("AT+UUID0xFFE3");

	delay(100);

	unsigned char readBuf[128];
	int i = 0;

	// get data
	while (Serial1.available())
	{
		readBuf[i++] = Serial1.read();
	}

	readBuf[i] = '\0';
	debugPrint("%s\r\n", readBuf);

	// expect data: OK+Set:
	// parse datas
	char *s = strstr(readBuf, "OK+Set:");


	return (s != NULL);

}


/*!
   \brief Get current working mode
 */
UserMode_t get_user_mode()
{
	return user_mode;
}



/*!
   \brief set front end offset(mm)
 */
void set_front_end_offset(float offset)
{
	front_end_offset = offset;
	save_front_offset(offset);
}

/*!
   \brief get front end offset(mm)
 */
float get_front_end_offset()
{
	return front_end_offset;
}

/*!
   \brief set height offset(mm)
 */
void set_height_offset(float offset)
{
	height_offset = offset;
	save_height_offset(offset);
}

/*!
   \brief get height offset(mm)
 */
float get_height_offset()
{
	return height_offset;
}

/*!
   \brief get height offset from E2PROM(mm)
 */
float read_height_offset()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_HEIGHT_ADDR, DATA_TYPE_FLOAT);
}

/*!
   \brief Save height offset to E2PROM(mm)
 */
void save_height_offset(float offset)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_HEIGHT_ADDR, DATA_TYPE_FLOAT, offset);
}

/*!
   \brief get front offset from E2PROM(mm)
 */
float read_front_offset()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_FRONT_ADDR, DATA_TYPE_FLOAT);
}

/*!
   \brief Save front offset to E2PROM(mm)
 */
void save_front_offset(float offset)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_FRONT_ADDR, DATA_TYPE_FLOAT, offset);
}

/*!
   \brief get user mode from E2PROM(mm)
 */
uint8_t read_user_mode()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_MODE_ADDR, DATA_TYPE_BYTE);
}

/*!
   \brief Save user mode to E2PROM(mm)
 */
void save_user_mode(uint8_t mode)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_MODE_ADDR, DATA_TYPE_BYTE, mode);
}

/*!
   \brief get Print acceleration from E2PROM(mm)
 */
float read_print_acceleration()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_PRINT_ACCELERATION_ADDR, DATA_TYPE_FLOAT);
}
/*!
   \brief Save Print acceleration to E2PROM(mm)
 */
void save_print_acceleration(float acceleration)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_PRINT_ACCELERATION_ADDR, DATA_TYPE_FLOAT, acceleration);
}

/*!
   \brief get Retract acceleration from E2PROM(mm)
 */
float read_retract_acceleration()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_RETRACT_ACCELERATION_ADDR, DATA_TYPE_FLOAT);
}
/*!
   \brief Save Retract acceleration to E2PROM(mm)
 */
void save_retract_acceleration(float acceleration)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_RETRACT_ACCELERATION_ADDR, DATA_TYPE_FLOAT, acceleration);
}

/*!
   \brief get Travel acceleration from E2PROM(mm)
 */
float read_travel_acceleration()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_TRAVEL_ACCELERATION_ADDR, DATA_TYPE_FLOAT);
}
/*!
   \brief Save Travel acceleration to E2PROM(mm)
 */
void save_travel_acceleration(float acceleration)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_TRAVEL_ACCELERATION_ADDR, DATA_TYPE_FLOAT, acceleration);
}


/*!
   \brief get max xy jerk from E2PROM(mm)
 */
float read_max_xy_jerx()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_MAX_XY_JERK_ADDR, DATA_TYPE_FLOAT);
}
/*!
   \brief Save max xy jerk to E2PROM(mm)
 */
void save_max_xy_jerk(float max_xy_jerk)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_MAX_XY_JERK_ADDR, DATA_TYPE_FLOAT, max_xy_jerk);
}

/*!
   \brief get max z jerk from E2PROM(mm)
 */
float read_max_z_jerx()
{
	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_MAX_Z_JERK_ADDR, DATA_TYPE_FLOAT);
}
/*!
   \brief Save max z jerk to E2PROM(mm)
 */
void save_max_z_jerk(float max_z_jerk)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_MAX_Z_JERK_ADDR, DATA_TYPE_FLOAT, max_z_jerk);
}



/*!
   \brief Save acceleration_jerk to E2PROM(mm)
 */
void save_P_T_acceleration(float p_acceleration,float t_acceleration)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_PRINT_ACCELERATION_ADDR, DATA_TYPE_FLOAT, p_acceleration);
	delay(10);
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_TRAVEL_ACCELERATION_ADDR, DATA_TYPE_FLOAT, t_acceleration);
}

/*!
   \brief read acceleration flag from E2PROM(mm)
 */
uint8_t read_acceleration_flag()
{
 	return getE2PROMData(EEPROM_ON_CHIP, EEPROM_ACCELERATION_FLAG_ADDR, DATA_TYPE_BYTE) ;
}

/*!
   \brief save acceleration flag to E2PROM(mm)
 */
void save_acceleration_flag(uint8_t flag)
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_ACCELERATION_FLAG_ADDR, DATA_TYPE_BYTE, flag);
}

/*!
   \brief clear acceleration flag from E2PROM(mm)
 */
void clear_acceleration_flag()
{
	setE2PROMData(EEPROM_ON_CHIP, EEPROM_ACCELERATION_FLAG_ADDR, DATA_TYPE_BYTE, 0);
}


unsigned char getXYZFromAngle(float& x, float& y, float& z, float rot, float left, float right)
{
	// 閿熸枻鎷稾Y骞抽敓鏂ゆ嫹閿熼叺闃熷府鎷烽敓鏂ゆ嫹�?	
	
	
	double stretch = MATH_LOWER_ARM * cos(left / MATH_TRANS) + MATH_UPPER_ARM * cos(right / MATH_TRANS) + MATH_L2 + front_end_offset;

	// 閿熸枻鎷穁閿熸枻鎷烽敓閰甸槦甯嫹閿熸枻鎷烽�?
	double height = MATH_LOWER_ARM * sin(left / MATH_TRANS) - MATH_UPPER_ARM * sin(right / MATH_TRANS) + MATH_L1;
	y = -stretch * cos(rot / MATH_TRANS);
	x = stretch * sin(rot / MATH_TRANS);
	z = height - height_offset;

	return 0;    
}

/*!
   \brief check if power plug in
 */
bool isPowerPlugIn()
{
	//debugPrint("power analog:%d\r\n", analogRead(POWER_DETECT));
	uint16_t power_adc_value = 0;

	power_adc_value = getAnalogPinValue(POWER_DETECT);

	if (power_adc_value > 100)
		return true;
	else
		return false;
}

/*!
   \brief get e2prom data
   \param device:  EEPROM_ON_CHIP, EEPROM_EXTERN_USER, EEPROM_EXTERN_SYSTEM
   \param addr: 0~2047(EEPROM_ON_CHIP), 0~65535(EEPROM_EXTERN_USER, EEPROM_EXTERN_SYSTEM)
   \param type: DATA_TYPE_BYTE, DATA_TYPE_INTEGER, DATA_TYPE_FLOAT
 */
double getE2PROMData(unsigned char device, unsigned int addr, unsigned char type)
{
   	double result = 0;

	uint8_t deviceAddr;


	union {
		float fdata;
		uint8_t data[4];
	} FData;


	switch(device)
	{

	case 0:

		switch(type)
		{
			case DATA_TYPE_BYTE:
			{
				int val = EEPROM.read(addr);
				result = val;
				break;
			}
			case DATA_TYPE_INTEGER:
			{
				int i_val = 0;
				EEPROM.get(addr, i_val);
				result = i_val;
				break;
			}
			case DATA_TYPE_FLOAT:
			{
				double f_val = 0.0f;
				EEPROM.get(addr,f_val);
				result = f_val;
				break;
			}
		}

	        break;

	case 1:
		deviceAddr = EXTERNAL_EEPROM_USER_ADDRESS;
        	break;

	case 2:
		deviceAddr = EXTERNAL_EEPROM_SYS_ADDRESS;
        	break;

	default:
		return ADDRESS_ERROR;
	}

    
	if (device == 1 || device == 2)
	{
		int num = 0;
		switch(type)
		{
		case DATA_TYPE_BYTE:
			num = 1;
			break;
		
		case DATA_TYPE_INTEGER:
			num = 2;
			break;
		
		case DATA_TYPE_FLOAT:
			num = 4;
			break;
		
		default:
			return PARAMETER_ERROR;
		}	

		unsigned char i=0;
		i = (addr % 128);

		if (i >= (129-num)) 
		{
			i = 128 - i;
			iic_readbuf(FData.data, deviceAddr, addr, i);
			delay(5);
			iic_readbuf(FData.data + i, deviceAddr, addr + i, num - i);
		}
		else
		{
			iic_readbuf(FData.data, deviceAddr, addr, num);
		}      


		switch(type)
		{
		case DATA_TYPE_BYTE:			
			result = FData.data[0];
			break;
			
		case DATA_TYPE_INTEGER:		
			result = (FData.data[0] << 8) + FData.data[1];
			break;
			
		case DATA_TYPE_FLOAT:
			result = FData.fdata;
			break;
			
		}


	}

	return result;

}

/*!
   \brief set e2prom data
   \param device:  EEPROM_ON_CHIP, EEPROM_EXTERN_USER, EEPROM_EXTERN_SYSTEM
   \param addr: 0~2047(EEPROM_ON_CHIP), 0~65535(EEPROM_EXTERN_USER, EEPROM_EXTERN_SYSTEM)
   \param type: DATA_TYPE_BYTE, DATA_TYPE_INTEGER, DATA_TYPE_FLOAT
   \param value: value to write
 */
double setE2PROMData(unsigned char device, unsigned int addr, unsigned char type, double value)
{
	uint8_t deviceAddr;

	union {
		float fdata;
		uint8_t data[4];
	} FData;

	switch(device)
	{

	case 0:    
	        switch(type)
	        {
	        case DATA_TYPE_BYTE:
	        	{
	                byte b_val;
	                b_val = byte(value);
	                EEPROM.write(addr, b_val);
	                break;
	        	}
	        case DATA_TYPE_INTEGER:
	        	{
	                int i_val = 0;
	                i_val = int(value);
	                EEPROM.put(addr, i_val);
	                break;
	        	}
	        case DATA_TYPE_FLOAT:
	        	{
	        	    float f_val = 0.0f;
	                f_val = float(value);
	                EEPROM.put(addr,f_val);
	                // Serial.println(f_val);
	                break;
	        	}
	        }
	        break;
	case 1:
		deviceAddr = EXTERNAL_EEPROM_USER_ADDRESS;
		break;

	case 2:
		deviceAddr = EXTERNAL_EEPROM_SYS_ADDRESS;
		break;

	default:
		return ADDRESS_ERROR;
	}       


	if (device == 1 || device == 2)
	{
		int num = 0;
		switch(type)
		{
		case DATA_TYPE_BYTE:
			FData.data[0] = byte(value);
			num = 1;
			break;
			
		case DATA_TYPE_INTEGER:	    
			{
				int i_val = 0;
				i_val = int(value); 
				FData.data[0] = (i_val & 0xff00) >> 8;
				FData.data[1] = i_val & 0xff;
				num = 2;
				break;
			}
		    
		case DATA_TYPE_FLOAT:
			FData.fdata = float(value);
			num = 4;
			break;
		    
		default:
			return PARAMETER_ERROR;
		}

		unsigned char i=0;
		i = (addr % 128);
		// Since the eeprom's sector is 128 byte, if we want to write 5 bytes per cycle we need to care about when there's less than 5 bytes left
		if (i >= (129-num)) 
		{
			i = 128 - i;
			iic_writebuf(FData.data, deviceAddr, addr, i);// write data
			delay(5);
			iic_writebuf(FData.data + i, deviceAddr, addr + i, num - i);// write data
		}
		//if the left bytes are greater than 5, just do it
		else
		{
			iic_writebuf(FData.data, deviceAddr, addr, num);// write data
		}      


       

	}

}

static void _sort(unsigned int array[], unsigned int len)
{
	unsigned char i=0,j=0;
	unsigned int temp = 0;

	for(i = 0; i < len; i++) 
	{
		for(j = 0; i+j < (len-1); j++) 
		{
			if(array[j] > array[j+1]) 
			{
				temp = array[j];
				array[j] = array[j+1];
				array[j+1] = temp;
			}
		}
	}	
}
/*!
   \brief get analog value of pin
   \param pin of arduino
   \return value of analog data
 */
int getAnalogPinValue(unsigned int pin)
{
	unsigned int dat[8];


	for(int i = 0; i < 8; i++)
	{
		dat[i] = analogRead(pin);
	}

	_sort(dat, 8);

	unsigned int result = (dat[2]+dat[3]+dat[4]+dat[5])/4;

	return result;    
}

/*!
   \brief pump working
 */
void pumpOn()
{

	digitalWrite(VALVE_EN, LOW); 

	PORTG |= 0x10;

	pump_set_state(PUMP_STATE_ON);
}

/*!
   \brief pump stop
 */
void pumpOff()
{

	if (pump_get_state() == PUMP_STATE_ON)
	{

		digitalWrite(VALVE_EN, HIGH); 
	
		PORTG &= 0xef;

		pump_set_state(PUMP_STATE_VALVE_ON);
	}

}

/*!
   \brief gripper work
 */
void gripperCatch()
{
	//digitalWrite(GRIPPER, LOW);
	PORTC &= ~0X80;
}

/*!
   \brief gripper stop
 */
void gripperRelease()
{
 	//digitalWrite(GRIPPER, HIGH);
 	
	PORTC |= 0X80;
}

/*!
   \brief get pump status
   \return STOP if pump is not working
   \return WORKING if pump is working but not catched sth
   \return GRABBING if pump got sth   
 */
unsigned char getPumpStatus()
{

	if (!(PORTG & 0x10))
	{
		return STOP;
	}
	else
	{
		uint16_t value = getAnalogPinValue(PUMP_FEEDBACK);
		debugPrint("pump value:%d\r\n", value);
		if (value < 10)
		{
			// error, no work
			return STOP;
		}
		else if (value <= PUMP_GRABBING_CURRENT)
		{
			return GRABBING;
		}
		else
		{
			return WORKING;
		}
	}

}

/*!
   \brief get gripper status
   \return STOP if gripper is not working
   \return WORKING if gripper is working but not catched sth
   \return GRABBING if gripper got sth   
 */
unsigned char getGripperStatus()
{

	if (PINC & 0X80)
	{
		return STOP;//NOT WORKING
	}
	else
	{

		if (digitalRead(GRIPPER_FEEDBACK))
		{
			return WORKING;
		}
		else
		{
			return GRABBING;
		}
	} 


}

/*!
   \brief get hardware subvervion
 */
unsigned char getHWSubversion()
{
	return mHWSubVersion;
}

/*!
   \brief get the status of tip 
   return: 1. the switch is on
		   0. the switch if off
 */
unsigned char getTip()
{
	return  (PING & 0X08) ? 0 : 1;

}

/*!
   \brief get the status of user defined key 
   return: 1. the key is down
		   0. the key is up
 */
unsigned char getSwitchState()
{
	return digitalRead(KEY_STATE_PIN) ? 0 : 1;
}

/*!
   \brief test e2prom available
   \param device:  EEPROM_ON_CHIP, EEPROM_EXTERN_USER, EEPROM_EXTERN_SYSTEM
   \return: true if available
 */
bool eeprom_write_test(uint8_t device)
{
	uint16_t value = 0;
	
	if (device > EEPROM_EXTERN_SYSTEM)
	{
		return false;
	}
	
	delay(10);
	setE2PROMData(device, EEPROM_WRITE_TEST_ADDR, DATA_TYPE_INTEGER, 0x5a5a);
	delay(10);
	value = getE2PROMData(device, EEPROM_WRITE_TEST_ADDR, DATA_TYPE_INTEGER);
	if (value != 0x5a5a)
	{
		return false;
	}

	delay(10);
	setE2PROMData(device, EEPROM_WRITE_TEST_ADDR, DATA_TYPE_INTEGER, 0xa5a5);
	delay(10);
	value = getE2PROMData(device, EEPROM_WRITE_TEST_ADDR, DATA_TYPE_INTEGER);
	if (value != 0xa5a5)
	{ 
		return false;
	}	
	
	return true;
}

