/**
  ******************************************************************************
  * @file	uArmSwift.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-15
  ******************************************************************************
  */

#include "uArmSwift.h" 
#include "macros.h"
#include "stepper_indirection.h"
#include "servo.h"
#include "uArmServo.h"
#include "Grovergb_lcd.h"
#include "uArmGrove2.h"


// CAUTION: E_AXIS means FrontEnd Servo not extruder0
//float current_angle[NUM_AXIS] = { 0.0 };

float angle_destination[NUM_AXIS] = { 0.0 };

extern float current_position[NUM_AXIS];
extern float destination[NUM_AXIS];

bool block_running = false;

unsigned long tickStartTime = millis(); // get timestamp;



uArmButton button_menu;
uArmButton button_play;

uArmLed led_R;
uArmLed led_G;
uArmLed led_B;

extern Servo servo[NUM_SERVOS];

const char axis_codes[NUM_AXIS] = {'X', 'Y', 'Z', 'E'};

extern void get_pos_from_polor(float pos[], float polor[]);
extern void get_current_pos_polor(float polor[], float pos[]);

static uint8_t moving_stop_report_enable = 0;

void set_block_running(bool running)
{
	uint8_t result[128] = {0};

	if (moving_stop_report_enable && !running && block_running != running)
	{
		msprintf(result, "@%d V%d\r\n", REPORT_MOVING_STOP, running);		
		reportString(result);
	}
	
	block_running = running;
}

bool get_block_running()
{
	return block_running;
}

bool is_menu_button_pressed()
{
	if (PINE & 0x80)
	{
		return false;
	}

	return true;
}

bool is_play_button_pressed()
{
	if (PINE & 0x40)
	{
		return false;
	}

	return true;	
}


void buttons_init()
{
	PORTE |= 0xC0;
	// set button pin as input
	DDRE &= 0X3F;


	button_menu.setIsButtonPressedCB(is_menu_button_pressed, NULL);
	button_play.setIsButtonPressedCB(is_play_button_pressed, NULL);
}



void swift_init()
{

	
	// setup stepper driver micro
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);

	pinMode(A10, OUTPUT);
	digitalWrite(A10, HIGH);
	pinMode(A11, OUTPUT);
	digitalWrite(A11, HIGH);
	pinMode(A12, OUTPUT);
	digitalWrite(A12, HIGH);

	DDRG |= 0x10;

	// setup encoder
	X_IIC_Init();
	Y_IIC_Init();
	Z_IIC_Init();
	delay(10);

#ifdef SWIFT_TEST_MODE
	enable_e0();
#else
	enable_all_steppers();
	if (getHWSubversion() >= SERVO_HW_FIX_VER)
	{
		servo[0].attach(SERVO0_PIN);
	}
	else
	{
		servo_write(90.0);
	}

#endif	
	delay(1000);

	init_user_mode();

	init_reference_angle_value();
	update_current_pos();

	buttons_init();

	led_R.setPin(LED_PIN);
	led_G.setPin(LED_G);
	led_B.setPin(LED_B);

	swift_buzzer.setPin(BEEPER_PIN);
	swift_buzzer.tone(260, 1000);

	reportServiceInit();
	swift_api_init();
	service.init();


}

void tickTaskRun()
{
	swift_buzzer.tick();
	button_menu.tick();
	button_play.tick();

	led_R.tick();
	led_G.tick();
	led_B.tick();

	pump_tick();

	servo_tick();

	uArmGroveTick();
}

void swift_run()
{

	reportServiceRun();

	service.run();

	GroveReportRun();
	
	if(millis() - tickStartTime >= TICK_INTERVAL)
	{
		tickStartTime = millis();
		tickTaskRun();
	}   	
}

unsigned char getXYZFromAngleOrigin(float& x, float& y, float& z, float rot, float left, float right)
{
	// 閿熸枻鎷稾Y骞抽敓鏂ゆ嫹閿熼叺闃熷府鎷烽敓鏂ゆ嫹�?	
	
	
	double stretch = MATH_LOWER_ARM * cos(left / MATH_TRANS) + MATH_UPPER_ARM * cos(right / MATH_TRANS) + MATH_L2;

	// 閿熸枻鎷穁閿熸枻鎷烽敓閰甸槦甯嫹閿熸枻鎷烽�?
	double height = MATH_LOWER_ARM * sin(left / MATH_TRANS) - MATH_UPPER_ARM * sin(right / MATH_TRANS) + MATH_L1;
	x = stretch * cos(rot / MATH_TRANS);
	y = stretch * sin(rot / MATH_TRANS);
	z = height;

	return 0;    
}

float get_current_height()
{
	float angle[3];
	float pos[3];

	if (!isPowerPlugIn())
	{
		MYSERIAL.println("No Power Connected!");
		return ;
	}

	for (int i = 0; i < 3; i++)
	{
		angle[i] = get_current_angle(i);
	}
	
	debugPrint("angle: %f, %f, %f\r\n", angle[X_AXIS], angle[Y_AXIS], angle[Z_AXIS]);

	// get current pos
	getXYZFromAngleOrigin(pos[X_AXIS], pos[Y_AXIS], pos[Z_AXIS], angle[X_AXIS], angle[Y_AXIS], angle[Z_AXIS]);

	debugPrint("cur_pos: %f, %f, %f\r\n", pos[X_AXIS], pos[Y_AXIS], pos[Z_AXIS]);

	return pos[2];	
}

void update_current_pos()
{
	float current_angle[NUM_AXIS];

	if (!isPowerPlugIn())
	{
		MYSERIAL.println("No Power Connected!");
		return ;
	}

	for (int i = 0; i < NUM_AXIS; i++)
	{
		current_angle[i] = get_current_angle(i);
	}

	
	debugPrint("cur_angles: %f, %f, %f, %f\r\n", current_angle[X_AXIS], current_angle[Y_AXIS], current_angle[Z_AXIS], current_angle[E_AXIS]);

	// get current pos
	getXYZFromAngle(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_angle[X_AXIS], current_angle[Y_AXIS], current_angle[Z_AXIS]);

	debugPrint("cur_pos: %f, %f, %f\r\n", current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS]);


	destination[X_AXIS] = current_position[X_AXIS];
	destination[Y_AXIS] = current_position[Y_AXIS];
	destination[Z_AXIS] = current_position[Z_AXIS];

	planner.set_position_mm(current_angle[X_AXIS], current_angle[Y_AXIS], current_angle[Z_AXIS]);
}

uint8_t last_G0_1_cmd = 0;
extern float code_value_float();
extern uint8_t code_value_byte();
extern uint16_t code_value_ushort();

bool fan_enable = false;
bool heater_enable = false;

bool is_heater_enable()
{
	return heater_enable;
}

void set_heater_function(bool enable)
{
	heater_enable = enable;
}



bool is_fan_enable()
{
	return fan_enable;
}


void set_fan_function(bool enable)
{
	fan_enable = enable;
}

void laser_on(uint8_t power)
{
	// turn on laser
	analogWrite(FAN_PIN, power);	
}

void laser_off()
{
	// turn off laser
	analogWrite(FAN_PIN, 0);	
}

void uarm_gcode_G0()
{
	if (get_user_mode() == USER_MODE_LASER || get_user_mode() == USER_MODE_PEN) 
	{
		while (last_G0_1_cmd != 0  && block_running) idle();

		last_G0_1_cmd = 0;

		if (get_user_mode() == USER_MODE_LASER)
		{


			
			debugPrint("laser off\r\n");
			
			laser_off();
		}
	}
}

void uarm_gcode_G1()
{
	if (get_user_mode() == USER_MODE_LASER || get_user_mode() == USER_MODE_PEN) 
	{	
		while (last_G0_1_cmd == 0  && block_running) idle();

		last_G0_1_cmd = 1;

		if (get_user_mode() == USER_MODE_LASER)
		{

			
			static uint8_t power = 255;

			if (code_seen('P'))

			{
				power = code_value_byte();
			}

			debugPrint("laser on p=%d\r\n", power);
			laser_on(power);

		}
	}	
}



void reportString(String string)
{
	MYSERIAL.print(string);
}

void reportButtonEvent(unsigned char buttonId, unsigned char event)
{
	char result[RESULT_BUFFER_SIZE];
	msprintf(result, "@4 B%d V%d\r\n", buttonId, event); 
	reportString(result);  
}

void reportPos()
{
	char result[128];
	//@3 X154.714 Y194.915 Z10.217\n
	//msprintf(result, "@3 X%f Y%f Z%f\r\n", );
	float angle[NUM_AXIS];
	float pos[NUM_AXIS];

	if (!isPowerPlugIn())
	{
		MYSERIAL.println("No Power Connected!");
		return ;
	}

	for (int i = 0; i < NUM_AXIS; i++)
	{
		angle[i] = get_current_angle(i);
	}
	
	debugPrint("angle: %f, %f, %f, %f\r\n", angle[X_AXIS], angle[Y_AXIS], angle[Z_AXIS], angle[E_AXIS]);

	// get current pos
	getXYZFromAngle(pos[X_AXIS], pos[Y_AXIS], pos[Z_AXIS], angle[X_AXIS], angle[Y_AXIS], angle[Z_AXIS]);

	debugPrint("cur_pos: %f, %f, %f\r\n", pos[X_AXIS], pos[Y_AXIS], pos[Z_AXIS]);

	msprintf(result, "@3 X%f Y%f Z%f R%f\r\n", pos[X_AXIS], pos[Y_AXIS], pos[Z_AXIS], angle[3]);

	reportString(result);	
}

void rotate_frontend_motor()
{
	float angle = 0;
	uint32_t speed = 0;



	if (code_seen('V'))
	{
		angle = code_value_float();
	}
	else
	{
		return;
	}

	if (code_seen('F'))
	{
		speed = code_value_int();		
	}
	else
	{
		//return;
	}


	if (getHWSubversion() >= SERVO_HW_FIX_VER)
	{	
		servo[0].write((int)angle);
	}
	else
	{
		servo_write((double)angle);
	}
	
}


void uarm_gcode_M2000()
{
	clear_command_queue();
	quickstop_stepper();
	print_job_timer.stop();
	thermalManager.autotempShutdown();
	wait_for_heatup = false;
	update_current_pos();
}


void uarm_gcode_M2120()
{
	float interval = 0;
	if (code_seen('V')) 
	{
		interval = code_value_float();

		interval *= 1000;

		if (interval == 0)
		{
			removeReportService(3);
		}
		else
		{
			addReportService(3, interval, reportPos);
		}
	}
}

void uarm_gcode_M2122()
{
	uint8_t value = 0;
	
	if (code_seen('V')) 
	{
		value = code_value_byte();

		moving_stop_report_enable = value ? 1 : 0;

	}
}


uint8_t uarm_gcode_M2200(char reply[])
{
	if (get_block_running())
	{
		strcpy(reply, "V1");
	}
	else
	{
		strcpy(reply, "V0");
	}

	return E_OK;
}

void uarm_gcode_M2201()
{
	if (code_seen('N'))
	{
		uint8_t index = code_value_byte();
		switch(index)
		{
		case 0:
			enable_x();
			break;

		case 1:
			enable_y();
			break;

		case 2:
			enable_z();
			break;

		case 3:
			if (getHWSubversion() >= SERVO_HW_FIX_VER)
			{			
				servo[0].attach(SERVO0_PIN);
			}
			break;

		}
	}
}

void uarm_gcode_M2202()
{
	if (code_seen('N'))
	{
		uint8_t index = code_value_byte();
		switch(index)
		{
		case 0:
			disable_x();
			break;

		case 1:
			disable_y();
			break;

		case 2:
			disable_z();
			break;

		case 3:
			servo[0].detach();
			break;

		}
	}
}

uint8_t uarm_gcode_M2203(char reply[])
{
	bool attached = false;
	if (code_seen('N'))
	{
		uint8_t index = code_value_byte();
		switch(index)
		{
		case 0:
			attached = (digitalRead(X_ENABLE_PIN) == X_ENABLE_ON);
			break;

		case 1:
			attached = (digitalRead(Y_ENABLE_PIN) == Y_ENABLE_ON);
			break;

		case 2:
			attached = (digitalRead(Z_ENABLE_PIN) == Z_ENABLE_ON);
			break;

		case 3:
			if (getHWSubversion() >= SERVO_HW_FIX_VER)
			{	
				attached = servo[0].attached();
			}
			else
			{
				attached = 0;
			}
			break;

		}
	}

	if (attached)
	{
		strcpy(reply, "V1");
	}
	else
	{
		strcpy(reply, "V0");
	}	

	return E_OK;
}

void uarm_gcode_M2210()
{
	uint16_t frequency = 1000;
	uint16_t duration = 260;

	if (code_seen('F'))
	{
		frequency = code_value_ushort();
	}

	if (code_seen('T'))
	{
		duration = code_value_float();
	}

	NOMORE(frequency, 5000);

	swift_buzzer.tone(duration, frequency);
}

uint8_t uarm_gcode_M2211(char reply[])
{


	int device = 0;
	int type = 0;
	uint32_t addr = 0;


	if (code_seen('N'))
	{
		device = code_value_ushort();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	if (code_seen('A'))
	{
		addr = code_value_ushort();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	if (code_seen('T'))
	{
		type = code_value_ushort();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}	

	double resultVal = getE2PROMData(device, addr, type);

	switch(type)
	{
		case DATA_TYPE_BYTE:
		{
			int val = resultVal;
			msprintf(reply, "V%d", val);
			break;
		}
		case DATA_TYPE_INTEGER:
		{
			int i_val = resultVal;
			msprintf(reply, "V%d", i_val);
			break;
		}
		case DATA_TYPE_FLOAT:
		{
			double f_val = resultVal;
			msprintf(reply, "V%f", f_val);
			break;
		}
	}


	return E_OK;
}

void uarm_gcode_M2212()
{
	int type = 0;
	int device = 0;
	uint32_t addr = 0;

	double value;

	if (code_seen('N'))
	{
		device = code_value_ushort();
	}
	else
	{
		return;
	}

	if (code_seen('A'))
	{
		addr = code_value_ushort();
	}
	else
	{
		return;
	}

	if (code_seen('T'))
	{
		type = code_value_ushort();
	}
	else
	{
		return;
	}	

	if (code_seen('V'))
	{
		value = code_value_float();
	}
	else
	{
		return;
	}

	setE2PROMData(device, addr, type, value);

	//replyOK(serialNum);

	return 0;
}

void uarm_gcode_M2213()
{
	uint16_t value = 0;
	if (code_seen('V'))
	{
		value = code_value_ushort();
	}
	else
	{
		return;
	}

	if (value)
	{
		service.setButtonService(true); 
	}
	else
	{
		service.setButtonService(false); 
	}	
}

extern char inverse_kinematics(const float in_cartesian[3], float angle[3]);
uint8_t uarm_gcode_M2220(char reply[])
{
	float value[3];
	float angle[3];

	if (code_seen('X'))
	{
		value[0] = code_value_float();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	if (code_seen('Y'))
	{
		value[1] = code_value_float();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	if (code_seen('Z'))
	{
		value[2] = code_value_float();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}		

	if (inverse_kinematics(value, angle) == 0)
	{

		//$n OK B50 L50 R50\n
		msprintf(reply, "B%f L%f R%f", angle[X_AXIS], angle[Y_AXIS], angle[Z_AXIS]);

		return E_OK;
	}
	else
	{
		return E_OUT_OF_RANGE;
	}
}

uint8_t uarm_gcode_M2221(char reply[])
{
	float value[3];
	float angle[3];

	if (code_seen('B'))
	{
		angle[0] = code_value_float();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	if (code_seen('L'))
	{
		angle[1] = code_value_float();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	if (code_seen('R'))
	{
		angle[2] = code_value_float();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}		

	getXYZFromAngle(value[0], value[1], value[2], angle[0], angle[1], angle[2]);

	msprintf(reply, "X%f Y%f Z%f", value[X_AXIS], value[Y_AXIS], value[Z_AXIS]);

	return E_OK;		
}

uint8_t uarm_gcode_M2222(char reply[])
{
	int i = 0;
	float pos[NUM_AXIS];
	float value[NUM_AXIS];
	float angle[NUM_AXIS];
	
	LOOP_XYZ(i)
	{
		if (code_seen(axis_codes[i]))
		{
			value[i] = code_value_float();
		}
		else
		{
			return E_PARAMETERS_WRONG;
		}
	}

	uint8_t type = 0;

	if (code_seen('S'))
	{
		type = code_value_byte() ? 1 : 0;
	}

	if (type)
	{
		get_pos_from_polor(pos, value);
	}
	else
	{
		LOOP_XYZ(i)
			pos[i] = value[i];
	}

	if (inverse_kinematics(pos, angle) == 0)
	{
		strcpy(reply, "V1");
	}
	else
	{
		strcpy(reply, "V0");
	}

	return E_OK;
	
}

void uarm_gcode_M2231()
{
	uint8_t value = 0;

	if (code_seen('V'))
	{
		value = code_value_byte();

		if (value)
		{
			pumpOn();
		}
		else
		{
			pumpOff();
		}
	}
	else
	{
		return;
	}


}



void uarm_gcode_M2232()
{
	uint8_t value = 0;

	if (code_seen('V'))
	{
		value = code_value_byte();

		if (value)
		{
			gripperCatch();
		}
		else
		{
			gripperRelease();
		}
	}
	else
	{
		return;
	}
}

void uarm_gcode_M2233()
{
	clearMacFlag();
}

void uarm_gcode_M2234()
{
	if (code_seen('V'))
	{
		uint8_t enable = code_value_byte();

		service.disableBT(enable ? false : true);
	}
	
}

void uarm_gcode_M2240()
{
	uint8_t value = 0;
	uint8_t pin = 0;

	if (code_seen('N'))
	{
		pin = code_value_byte();
	}
	else
	{
		return;
	}	

	if (code_seen('V'))
	{
		value = code_value_byte();
	}
	else
	{
		return;
	}	

	if (value)
	{
		digitalWrite(pin, HIGH);
	}	
	else
	{
		digitalWrite(pin, LOW);
	}
}

void uarm_gcode_M2241()
{
	uint8_t value = 0;
	uint8_t pin = 0;

	if (code_seen('N'))
	{
		pin = code_value_byte();
	}
	else
	{
		return;
	}	

	if (code_seen('V'))
	{
		value = code_value_byte();
	}
	else
	{
		return;
	}	

	if (value)
	{
		pinMode(pin, OUTPUT);
	}	
	else
	{
		pinMode(pin, INPUT);
	}
}


extern void code_value_string(char* buf, uint16_t buf_len);

uint8_t uarm_gcode_M2245(char reply[])
{

	// bt name no more than 11 characters
	char btName[12] = {0};
	

	if (code_seen('V'))
	{
		code_value_string(btName, 11);
	}
	else
	{
		strcpy(reply, "Format Error!");
		return E_FAIL;		
	}	

	debugPrint("btname = %s\r\n", btName);

	if (setBtName(btName))
	{
		return E_OK;
	}
	else
	{
		strcpy(reply, "Set BT name fail!");
		return E_FAIL;
	}

	
}


void uarm_gcode_M2300()
{
	uint8_t type;

	uint8_t pin = 0;

	GrovePortType portType = 0;
	
	if (code_seen('N'))
	{
		type = code_value_byte();
	}
	else
	{
		return;
	}

	if (code_seen('D'))
	{
		portType = GROVE_PORT_DIGITAL;
		pin = code_value_byte();
	}

	if (code_seen('A'))
	{
		portType = GROVE_PORT_ANALOG;
		pin = code_value_byte();
	}	

	if (code_seen('I'))
	{
		portType = GROVE_PORT_I2C;
		pin = code_value_byte();
	}		

	initGroveModule(type, portType, pin);
}

void uarm_gcode_M2301()
{
	uint8_t type;
	uint16_t time;

	if (code_seen('N'))
	{
		type = code_value_byte();
	}
	else
	{
		return;
	}

	if (code_seen('V'))
	{
		time = code_value_ushort();
	}
	else
	{
		return;
	}

	setGroveModuleReportInterval(type, time);
}

void uarm_gcode_M2302()
{
	uint8_t type;
	uint16_t value;		
	
	if (code_seen('N'))
	{
		type = code_value_byte();
	}
	else
	{
		return;
	}
	
	if (code_seen('V'))
	{
		value = code_value_ushort();			
	}
	else
	{
		return;
	}

	setGroveModuleValue(type, value);
}
extern Grovergb_lcd grovergb_lcd;

void uarm_gcode_M2303()
{
	uint8_t type = 0;
	uint8_t cmdtype = 0;
	long redvalue = 0;
	long greenvalue = 0;
	long bluevalue = 0;

	long value = 0;	

	char stringtype[17] = {0};	

	if (code_seen('N'))
	{
		type = code_value_byte();
	}
	else
	{
		return;
	}
	
	if(code_seen('S'))
	{
		cmdtype = code_value_byte();

		if (code_seen('V'))
		{
			code_value_string(stringtype, LCD_TEXT_LEN);
		}	
		
		setGroveLCDModuleString(type,cmdtype,stringtype);		
	}
	else if(code_seen('T'))
	{
		cmdtype = code_value_byte();
		if(code_seen('V'))
		{
			value = code_value_ushort();			
		}
		setGroveLCDModuleValue(type,cmdtype,value);
	}
	else if(code_seen('R') || code_seen('G') || code_seen('B'))
	{
		if(code_seen('R'))
		{
			redvalue = code_value_byte();
		}
		if(code_seen('G'))
		{
			greenvalue = code_value_byte();
		}		
		if(code_seen('B'))
		{
			bluevalue = code_value_byte();
		}
		value = ((redvalue << 16) | (greenvalue << 8) | bluevalue);	
		setGroveLCDModuleRGB(type, cmdtype, value);
	}
	else
	{
		return;
	}

}

uint8_t uarm_gcode_M2304(char reply[])
{
	uint8_t port;

	if (code_seen('P'))
	{
		port = code_value_byte();
	}
	else
	{
		strcpy(reply, "format: M2304 Pn\r\n");
		return E_FAIL;
	}	

	deinitGroveModule(port);

	return E_OK;
}


uint8_t uarm_gcode_M2305(char reply[])
{
	uint8_t type;
	uint8_t port;

	if (code_seen('P'))
	{
		port = code_value_byte();
	}
	else
	{
		strcpy(reply, "format: M2305 Pn Nn\r\n");
		return E_FAIL;
	}


	if (code_seen('N'))
	{
		type = code_value_byte();
	}
	else
	{
		strcpy(reply, "format: M2305 Pn Nn\r\n");
		return E_FAIL;
	}

	return initGroveModule2(port, type, reply);
	
}

uint8_t uarm_gcode_M2306(char reply[])
{
	uint8_t port;
	uint16_t time;

	if (code_seen('P'))
	{
		port = code_value_byte();
	}
	else
	{
		strcpy(reply, "format: M2306 Pn Vn\r\n");
		return E_FAIL;
	}	

	if (code_seen('V'))
	{
		time = code_value_ushort();
	}
	else
	{
		strcpy(reply, "format: M2306 Pn Vn\r\n");
		return E_FAIL;

	}	

	return setGroveModuleReportInterval2(port, time, reply);
}


uint8_t uarm_gcode_M2307(char reply[])
{
	uint8_t port;

	if (code_seen('P'))
	{
		port = code_value_byte();
	}
	else
	{
		strcpy(reply, "format: M2307 Pn\r\n");
		return E_FAIL;
	}	

	return controlGroveModule(port, reply);
}


void uarm_gcode_M2400()
{
	uint8_t mode = 0;

	if (code_seen('S'))
	{
		mode = code_value_byte();
		set_user_mode(mode);
		update_current_pos();
	}
	
}

uint8_t uarm_gcode_M2401(char reply[])
{
	uint16_t value[NUM_AXIS];

	if (!isPowerPlugIn())
		return E_NO_POWER;

	// need unlock code, in case misoperation
	if (code_seen('V'))
	{
		uint16_t value = code_value_ushort();

		if (value != UNLOCK_NUM)
		{
			return E_FAIL;
		}
	}
	else if (code_seen('B'))
	{
		for (int i = 0; i < 3; i++)
		{
			value[i] = get_current_angle_adc(i);

			if (value[i] > 4096)
			{
				return E_FAIL;
			} 
		}


		update_reference_angle_value_B(value);

		update_current_pos();

		strcpy(reply, "");

		return E_OK;		
	}
	else
	{
		return E_FAIL;
	}

	for (int i = 0; i < 3; i++)
	{
		value[i] = get_current_angle_adc(i);

		if (value[i] > 4096)
		{
			return E_FAIL;
		} 
	}


	update_reference_angle_value(value);

	update_current_pos();

	strcpy(reply, "");

	return E_OK;
}

void uarm_gcode_M2410()
{
	float value = 0;
	if (code_seen('S')) 
	{
		value = code_value_float();
	}
	else
	{
		if (isPowerPlugIn())
		{
			value = get_current_height();
			value += 0.3;
		}
	}

	debugPrint("height: %f\r\n", value);

	if (isPowerPlugIn())
	{
		set_height_offset(value);
		save_height_offset(value);

		update_current_pos();
	}

}

void uarm_gcode_M2411()
{
	float value = 0;
	if (code_seen('S')) 
	{
		value = code_value_float();

		if (isPowerPlugIn())
		{
			set_front_end_offset(value);

			update_current_pos();
		}		
	}	
}


void uarm_gcode_M2500()
{
	float value = 0;

	debugPrint("M2500\r\n");
	ok_to_send();
	Serial2.begin(115200);
	commSerial.setSerialPort(&Serial2);
	
}




// get current angles
uint8_t uarm_gcode_P2200(char reply[])
{
	float angle[3];

	for (int i = 0; i < 3; i++)
	{
		angle[i] = get_current_angle(i);
	}

	msprintf(reply, "B%f L%f R%f", angle[X_AXIS], angle[Y_AXIS], angle[Z_AXIS]);

	return E_OK;	
}

uint8_t uarm_gcode_P2201(char reply[])
{
	msprintf(reply, "V%s", DEVICE_NAME);

	return E_OK;
}

uint8_t uarm_gcode_P2202(char reply[])
{
	msprintf(reply, "V%s.%d", HW_VER, getHWSubversion());

	return E_OK;
}

uint8_t uarm_gcode_P2203(char reply[])
{


	msprintf(reply, "V%s", SW_VER);


	return E_OK;
}

uint8_t uarm_gcode_P2204(char reply[])
{


	msprintf(reply, "V%s", SW_VER);


	return E_OK;	
}

uint8_t uarm_gcode_P2205(char reply[])
{



	msprintf(reply, "V%s", getMac());


	return E_OK;
}

uint8_t uarm_gcode_P2206(char reply[])
{
	float angle;
	uint8_t index = 0;

	if (code_seen('N'))
	{
		index = code_value_byte();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	if (index > 3)
		return E_OUT_OF_RANGE;

	angle = get_current_angle(index);

	msprintf(reply, "V%f\r\n", angle);

	return E_OK;
}

uint8_t uarm_gcode_P2220(char reply[])
{
	msprintf(reply, "X%f Y%f Z%f", current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS]);

	return E_OK;
}

uint8_t uarm_gcode_P2221(char reply[])
{
	float polor[3];

	get_current_pos_polor(polor, current_position);

	msprintf(reply, "S%f R%f H%f", polor[X_AXIS], polor[Y_AXIS], polor[Z_AXIS]);

	return E_OK;	
}

uint8_t uarm_gcode_P2231(char reply[])
{
	msprintf(reply, "V%d", getPumpStatus());

	return E_OK;
}

uint8_t uarm_gcode_P2232(char reply[])
{
	msprintf(reply, "V%d", getGripperStatus());

	return E_OK;
}

uint8_t uarm_gcode_P2233(char reply[])
{
	msprintf(reply, "V%d", getTip());

	return E_OK;
}

uint8_t uarm_gcode_P2234(char reply[])
{
	msprintf(reply, "V%d", isPowerPlugIn() ? 1 : 0);

	return E_OK;
}


uint8_t uarm_gcode_P2240(char reply[])
{
	uint8_t index = 0;

	if (code_seen('N'))
	{
		index = code_value_byte();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	msprintf(reply, "V%d", digitalRead(index));

	return E_OK;
}

uint8_t uarm_gcode_P2241(char reply[])
{
	uint8_t index = 0;

	if (code_seen('N'))
	{
		index = code_value_byte();
	}
	else
	{
		return E_PARAMETERS_WRONG;
	}

	msprintf(reply, "V%d", analogRead(index));

	return E_OK;
}

uint8_t uarm_gcode_P2242(char reply[])
{
	uint16_t value[3];

	for (int i = 0; i < 3; i++)

	{
		value[i] = get_current_angle_adc(i);
	}

	msprintf(reply, "B%d L%d R%d", value[0], value[1], value[2]);

	return E_OK;
}

uint8_t uarm_gcode_P2245(char reply[])
{
	strcpy(reply, "");
	return E_OK;
}



uint8_t uarm_gcode_P2400(char reply[])
{
	msprintf(reply, "V%d", get_user_mode());
	return E_OK;
}


#ifdef SWIFT_TEST_MODE
extern uint8_t get_test_result();
uint8_t uarm_gcode_P2250(char reply[])
{
	msprintf(reply, "R%d V", get_test_result());

	strcat(reply, getMac());
	
	return E_OK;
}
#endif // SWIFT_TEST_MODE
