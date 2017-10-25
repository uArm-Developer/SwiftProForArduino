/**
  ******************************************************************************
  * @file	uArmService.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-17
  ******************************************************************************
  */

#include "uArmService.h" 
#include "macros.h"
#include "stepper_indirection.h"
#include "servo.h"
#include "uArmServo.h"


//#define SERVICE_DEBUG

#ifdef SERVICE_DEBUG
#define service_debug	debugPrint
#else
#define service_debug
#endif

uArmService service;

extern uArmButton button_menu;
extern uArmButton button_play;
extern uArmLed led_R;
extern uArmLed led_G;
extern uArmLed led_B;
extern Servo servo[NUM_SERVOS];

extern float destination[NUM_AXIS];
extern float feedrate_mm_m;
extern void prepare_move_to_destination();
extern inline void set_current_to_destination();

uArmService::uArmService()
{

	mRecordAddr = 0;

	mButtonServiceDisable = false;
	mBTDisable = true;
	mReportStartTime = millis();

	mTickRecorderTime = millis();

	mTipState = 0;
	mPowerState = 0;
}

void uArmService::setButtonService(bool on)
{
	if (on)
	{
		mButtonServiceDisable = false;
	}
	else
	{
		mButtonServiceDisable = true;
	}
}


void uArmService::disableBT(bool disable)
{
	mBTDisable = disable;

	if (mBTDisable)
	{
		led_B.off();
		
		commSerial.setSerialPort(&Serial1);
		
		mSysStatus = NORMAL_MODE;		
	}
}

void menuButtonClicked(void *param)
{
	if (service.buttonServiceDisable())
	{
		reportButtonEvent(BUTTON_MENU, EVENT_CLICK);
	}
	else
	{
		service.handleButtonEvent(BUTTON_MENU, EVENT_CLICK);
	}
}

void menuButtonLongPressed(void *param)
{
	if (service.buttonServiceDisable())
	{
		reportButtonEvent(BUTTON_MENU, EVENT_LONG_PRESS);
	}
	else
	{
		service.handleButtonEvent(BUTTON_MENU, EVENT_LONG_PRESS);
	}
}

void playButtonClicked(void *param)
{
	if (service.buttonServiceDisable())
	{
		reportButtonEvent(BUTTON_PLAY, EVENT_CLICK);
	}
	else
	{
		service.handleButtonEvent(BUTTON_PLAY, EVENT_CLICK);
	}
}

void playButtonLongPressed(void *param)
{
	if (service.buttonServiceDisable())
	{
		reportButtonEvent(BUTTON_PLAY, EVENT_LONG_PRESS);
	}
	else
	{
		service.handleButtonEvent(BUTTON_PLAY, EVENT_LONG_PRESS);
	}	
}

#ifdef SWIFT_TEST_MODE

#define SCL_PIN	21
#define SDA_PIN	20

uArmLed led_seeed;

uint8_t test_result;

enum bt_test_state_t
{
	BT_TEST_INIT,
	BT_TEST_CONNECT,
	BT_TEST_WAIT_RESPONSE,
	BT_TEST_DISCONNECT,
	BT_TEST_GET_DATA,
	BT_TEST_GET_MAC,
	BT_TEST_IDLE,
	BT_TEST_DONE,

	BT_TEST_COUNT
};


uint8_t bt_test_state = BT_TEST_INIT;


enum test_item_t
{
	TEST_ITEM_EEPROM,
	TEST_ITEM_BT,

	TEST_ITEM_COUNT
};

uint8_t get_test_result()
{
	return test_result;
}

void all_light_on(void *param)
{
	debugPrint("all_light_on\r\n");

	PORTC &= ~0x80;
	PORTA &= ~0x01;

	PORTG |= 0x08;

	digitalWrite(6, HIGH);
	digitalWrite(8, LOW);
	digitalWrite(9, LOW);


	digitalWrite(A13, HIGH);
	digitalWrite(A3, LOW);	

	service.ledAllOff();
	led_R.on();


}

void all_light_off(void *param)
{
	debugPrint("all_light_off\r\n");

	PORTC |= 0x80;
	PORTA |= 0x01;	

	PORTG &= ~0x08;

	digitalWrite(6, LOW);
	digitalWrite(8, HIGH);
	digitalWrite(9, HIGH);

	digitalWrite(A13, LOW);
	digitalWrite(A3, HIGH);		
	service.ledAllOff();
	led_G.on();

}


void all_pins_init()
{
	// output 
	// pc7
	DDRC |= 0x80;
	PORTC |= 0x80;	
		
	// pa0
	DDRA |= 0x01;
	PORTA |= 0x01;	
	// pg3
	DDRG |= 0x08;
	PORTG |= 0x08;
	
	// pk5 a13
	// pf3 a3
	pinMode(A13, OUTPUT);
	pinMode(A3, OUTPUT);

	// ph3 7
	// d8 d9	
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);

	pinMode(SCL_PIN, OUTPUT);
	pinMode(SDA_PIN, INPUT_PULLUP);			

	led_seeed.setPin(SCL_PIN);

	led_seeed.blink();

	bt_test_state = BT_TEST_INIT;

	test_result |= eeprom_write_test(EEPROM_EXTERN_USER) ? (1 << TEST_ITEM_EEPROM) : 0;

	all_light_off(NULL);
	service.ledAllOff();	
}





void bt_test()
{
	static uint32_t bt_tick_count = 0;
	uint8_t readBuf[128];
	uint8_t i = 0;
	
	switch (bt_test_state)
	{
	case BT_TEST_INIT:
		if (!(test_result & (1 << TEST_ITEM_BT)))
		{
			if (service.btConnected())
			{
				bt_tick_count = 0;
				
				bt_test_state = BT_TEST_CONNECT;
			}
		}
		
		break;
		
	case BT_TEST_CONNECT:
		Serial1.write("$1 VSwiftPro\r\n");
		debugPrint("$1 VSwiftPro\r\n");
		bt_test_state = BT_TEST_WAIT_RESPONSE;
		break;
		
	case BT_TEST_WAIT_RESPONSE:
		// wait 1s
		bt_tick_count++;
		if (bt_tick_count >= 2)
		{
			bt_tick_count = 0;

			// check result
			i = 0;
			while (Serial1.available())
			{
				readBuf[i++] = Serial1.read();
			}

			readBuf[i] = '\0';
			debugPrint("Received:%s\r\n", readBuf);

			// expect data: #OK SwiftPro 8848+mac
			// parse data
			char *s = strstr(readBuf, "OK SwiftPro");

			if (s != NULL)
			{
				
				bt_test_state = BT_TEST_DISCONNECT;				
			}
			else
			{
				bt_test_state = BT_TEST_IDLE;
			}
		}
		break;

	case BT_TEST_DISCONNECT:
		Serial1.write("$2 VDisconnect\r\n");
		bt_test_state = BT_TEST_GET_DATA;
		bt_tick_count = 0;
		break;

	case BT_TEST_GET_DATA:
		bt_tick_count++;
		if (bt_tick_count >= 10)
		{
			bt_tick_count = 0;	
			clearMacFlag();
			Serial1.write("AT");	// disconnect bt
			bt_test_state = BT_TEST_GET_MAC;
			bt_tick_count = 0;
		}
		break;
		
	case BT_TEST_GET_MAC:
		bt_tick_count++;
		if (bt_tick_count >= 10)
		{
			bt_tick_count = 0;	
			getMacAddr();
			bt_test_state = BT_TEST_DONE;
			test_result |= (1 << TEST_ITEM_BT);
		}
		break;		

	case BT_TEST_IDLE:
		bt_tick_count++;
		if (bt_tick_count >= 20)
		{
			bt_tick_count = 0;	
			bt_test_state = BT_TEST_INIT;
		}
		break;
		
	case BT_TEST_DONE:
		break;
		

	}
}

void test_mode_run()
{


	static uint8_t last_switch_state = 0;

	uint8_t switch_state = getSwitchState();

	if (switch_state != last_switch_state)
	{
		if (switch_state)
		{
			pumpOn();
			service.ledAllOff();
			led_B.on();			
		}
		else
		{
			pumpOff();
			service.ledAllOff();
		}

		last_switch_state = switch_state;
	}



	E0_STEP_WRITE(1);
	delayMicroseconds(50);
	E0_STEP_WRITE(0);
	delayMicroseconds(50);
}

#endif 


void uArmService::init()
{
	// bt pin set
	DDRE &= 0xFB;
	PORTE |= 0x04;

#ifdef SWIFT_TEST_MODE
	::all_pins_init();

	button_menu.setClickedCB(::all_light_on, NULL);
	button_play.setClickedCB(::all_light_off, NULL);
#else
	button_menu.setClickedCB(menuButtonClicked, NULL);
	button_menu.setLongPressedCB(menuButtonLongPressed, NULL);
	button_play.setClickedCB(playButtonClicked, NULL);
	button_play.setLongPressedCB(playButtonLongPressed, NULL);
#endif // SWIFT_TEST_MODE
}


bool uArmService::buttonServiceDisable()
{
	return mButtonServiceDisable;
}

void uArmService::ledLearning()
{
	led_B.off();
	led_R.off();
	led_G.on();
}

void uArmService::ledPlaying()
{
	led_B.off();
	led_R.off();
	led_G.blink(500, 255);	
	// led_B.off();
	// led_R.blink(500, 255);
	// led_G.blink(500, 50);
}

void uArmService::ledAllOff()
{
	led_R.off();
	led_G.off();
	led_B.off();
}

void uArmService::handleButtonEvent(BUTTON_ID button, unsigned char event)
{
	switch(button)
	{
	case BUTTON_MENU:
		if (event == EVENT_CLICK)
		{
			switch (mSysStatus)
			{
			case NORMAL_MODE:
			case NORMAL_BT_CONNECTED_MODE:
				mSysStatus = LEARNING_MODE;
				ledLearning();
				mRecordAddr = 0;//recording/playing address

				
				disable_all_steppers();
				if (getHWSubversion() >= SERVO_HW_FIX_VER)
				{
					servo[0].detach();
				}
				break;

			case LEARNING_MODE:
				//LEARNING_MODE_STOP is just used to notificate record() function to stop, once record() get it then change the sys_status to normal_mode
				mSysStatus = LEARNING_MODE_STOP;//do not detec if BT is connected here, will do it seperatly
				ledAllOff();
				pumpOff();
				gripperRelease();
				break;

			default: 
				break;
			}	
		}

		break;

	case BUTTON_PLAY:
		if (event == EVENT_CLICK)
		{
			switch(mSysStatus)
			{
			case NORMAL_MODE:
			case NORMAL_BT_CONNECTED_MODE:
				mRecordAddr = 0;//recording/playing address
				mSysStatus = SINGLE_PLAY_MODE;  // or play just one time
				ledPlaying();
				set_acceleration(USER_MODE_NORMAL);
				break;

			case SINGLE_PLAY_MODE:
			case LOOP_PLAY_MODE:
				pumpOff();
				gripperRelease();
				mSysStatus = NORMAL_MODE;
				ledAllOff();
				set_acceleration(get_user_mode());
				break;

			case LEARNING_MODE:
			
				if (!getSwitchState())
				{
					if (getPumpStatus())
					{
						pumpOff();
					}
					else
					{
						pumpOn();
					}    
				}
				else
				{
					if (getGripperStatus())
					{
						gripperRelease();
					}
					else
					{
						gripperCatch();
					} 					
				}
				break;
			}
		}
		else if (event == EVENT_LONG_PRESS)
		{
			switch(mSysStatus)
			{
			case NORMAL_MODE:
			case NORMAL_BT_CONNECTED_MODE:
				mRecordAddr = 0;
				mSysStatus = LOOP_PLAY_MODE;
				ledPlaying();
				set_acceleration(USER_MODE_NORMAL);				
				break;

			case SINGLE_PLAY_MODE:
			case LOOP_PLAY_MODE:
				break;

			case LEARNING_MODE: 
				break;
			}				
		}	
		break;
	}
}


void uArmService::recorderTick()
{
    //sys led function detec every 0.05s-----------------------------------------------------------------

	switch(mSysStatus)//every 0.05s per point
	{
/*		
	case SINGLE_PLAY_MODE:
		if(play() == false)
		{
			mSysStatus = NORMAL_MODE;
			
			ledAllOff();
			
			mRecordAddr = 0;
		}
		break;

	case LOOP_PLAY_MODE:

		if(play() == false)
		{

			mRecordAddr = 0;
		}
		break;
*/

	case LEARNING_MODE:
	case LEARNING_MODE_STOP:
		if(record() == false)
		{
			mSysStatus = NORMAL_MODE;
			mRecordAddr = 0;
			
			ledAllOff();
			
			//controller.attachAllServo();
			enable_all_steppers();
			if (getHWSubversion() >= SERVO_HW_FIX_VER)
			{			
				servo[0].attach(SERVO0_PIN);
			}

			delay(500);

			update_current_pos();

		}
		break;

	default: 
		break;
	}

}



void uArmService::systemRun()
{
	static uint8_t last_switch_state = 0;

	uint8_t switch_state = getSwitchState();

	if (switch_state != last_switch_state)
	{
		if (switch_state)
		{
			mBTDisable = false;
		}
		else
		{
			mBTDisable = true;
		}

		last_switch_state = switch_state;
	}

	btDetect();	

}

bool uArmService::btConnected()
{
	return (PINE & 0x04) ? true : false;
}

void uArmService::btDetect()
{

/*
	if ((mSysStatus == NORMAL_MODE) || (mSysStatus == NORMAL_BT_CONNECTED_MODE))
	{
		
	
		if (btConnected() && !mBTDisable)
		{
			led_B.on();
			
			commSerial.setSerialPort(&Serial1);
			
			mSysStatus = NORMAL_BT_CONNECTED_MODE;
		}
		else
		{
			led_B.off();
			
			commSerial.setSerialPort(&Serial);
			
			mSysStatus = NORMAL_MODE;
		}

		
	}
*/

	if (mSysStatus == NORMAL_MODE)
	{
		if (btConnected() && !mBTDisable)
		{
			led_B.on();
			
			commSerial.setSerialPort(&Serial1);
			
			mSysStatus = NORMAL_BT_CONNECTED_MODE;
		}		
	}
	else if (mSysStatus == NORMAL_BT_CONNECTED_MODE)
	{
		if (!btConnected() || mBTDisable)
		{
			led_B.off();
			
			commSerial.setSerialPort(&Serial);
			
			mSysStatus = NORMAL_MODE;
		}		
	}

}


void uArmService::powerDetect()
{
	unsigned char powerState = isPowerPlugIn() ? 1 : 0;

	if (powerState != mPowerState)
	{
		mPowerState = powerState;
		char buf[128];

		msprintf(buf, "@5 V%d\r\n", mPowerState);

		reportString(buf);
	}

}

void uArmService::tipDetect()
{
	unsigned char tipState = getTip() ? 1 : 0;

	if (tipState != mTipState)
	{
		mTipState = tipState;
		char buf[128];

		msprintf(buf, "@6 N0 V%d\r\n", mTipState);

		reportString(buf);
	}	
}


uint8_t tick_count = 0;
void uArmService::run()
{
#ifdef SWIFT_TEST_MODE
	::test_mode_run();

	if (millis() - mTickRecorderTime >= TICK_INTERVAL)
	{
		mTickRecorderTime= millis();

		bt_test();
		led_seeed.tick();
	}

#else
	systemRun();

	if (millis() - mTickRecorderTime >= TICK_INTERVAL)
	{
		mTickRecorderTime= millis();
		recorderTick();
		
		tipDetect();

		tick_count++;
		if (tick_count >= (1000/TICK_INTERVAL))
		{
			tick_count = 0;
			powerDetect();
		}

	}
#endif
}


struct point_data
{
	double x;
	double y;
	double z;
};

#define POINT_DATA_MAX	10


class PointDataFilter
{
private:
	struct point_data points_data[POINT_DATA_MAX];
	int point_count = 0; 
	int point_index = 0; 	

public:
	PointDataFilter()
	{
		point_count = 0;
		point_index = 0;
	}

	void reset()
	{
		point_count = 0;
		point_index = 0;
	}

	void filter(point_data& in, point_data& out)
	{
		/*
		out.x = in.x;
		out.y = in.y;
		out.z = in.z;
		return;
		*/

		debugPrint("in %f, %f, %f\r\n", in.x, in.y, in.z);
		
		points_data[point_index].x = in.x;
		points_data[point_index].y = in.y;
		points_data[point_index].z = in.z;

/*
		for (int i = 0; i < POINT_DATA_MAX; i++)
		{
			debugPrint("%d %f, %f, %f\r\n", i, points_data[i].x, points_data[i].y, points_data[i].z);		
		}
*/

		point_index++;

		point_index %= POINT_DATA_MAX;

		point_count++;

		point_count = point_count > POINT_DATA_MAX ? POINT_DATA_MAX : point_count;

		if (point_count <= 1)
		{
			out.x = in.x;
			out.y = in.y;
			out.z = in.z;
			debugPrint("out %f, %f, %f\r\n", out.x, out.y, out.z);	
			return ;
		}

		point_data even_data = {0};
		even_data.x = 0;
		even_data.y = 0;
		even_data.z = 0;

		int start_pos = point_index - 1 - (point_count - 1) + POINT_DATA_MAX;

		start_pos %= POINT_DATA_MAX;

		//debugPrint("start_pos %d\r\n", start_pos);
		
		for (int i = 0; i < point_count-1; i++)
		{
			int pos = (start_pos+i)% POINT_DATA_MAX;

			//debugPrint("pos %d\r\n", pos);
			
			even_data.x += points_data[pos].x;
			even_data.y += points_data[pos].y;
			even_data.z += points_data[pos].z;
		}

		even_data.x /= (point_count - 1);
		even_data.y /= (point_count - 1);		
		even_data.z /= (point_count - 1);	

		//debugPrint("even %f, %f, %f\r\n", even_data.x, even_data.y, even_data.z);


		out.x = in.x * 0.8 + even_data.x * 0.2;
		out.y = in.y * 0.8 + even_data.y * 0.2;
		out.z = in.z * 0.8 + even_data.z * 0.2;
		
		debugPrint("out %f, %f, %f\r\n", out.x, out.y, out.z);		
	}	
};


#define SPEED_X 1

extern bool line_to_destination_play_mode(float fr_mm_m);

enum PlayState
{
	PLAY_STATE_IDLE,
	PLAY_STATE_START,
	PLAY_STATE_RUN,
	PLAY_STATE_WAIT,
	PLAY_STATE_DELAY,
	PLAY_STATE_GET_NEXT,
	PLAY_STATE_REPLACE_CUR,
	PLAY_STATE_END,
	PALY_STATE_END2,
	PLAY_STATE_DONE,
};

static PlayState play_state = PLAY_STATE_IDLE;

struct PlayData
{
	double x;
	double y;
	double z;
	double e;
	uint16_t eef;
};


#define PLAY_DATA_RECORD_MAX	5
class PlayDataRecord
{
private:
	PlayData _data[PLAY_DATA_RECORD_MAX];
	int pre_2;
	int pre;
	int cur;
	int next;
	int next_2;

public:
	PlayDataRecord()
	{
		pre_2 = 0;
		pre = 1;
		cur = 2;
		next = 3;
		next_2 = 4;
	}

	void initData(int type, PlayData data)
	{
		_data[type].x = data.x;
		_data[type].y = data.y;
		_data[type].z = data.z;
		_data[type].e = data.e;
		_data[type].eef = data.eef;
	}

	void indexInc()
	{
		pre_2++;
		pre++;
		cur++;
		next++;
		next_2++;

		pre_2 %= PLAY_DATA_RECORD_MAX;
		pre %= PLAY_DATA_RECORD_MAX;
		cur %= PLAY_DATA_RECORD_MAX;
		next %= PLAY_DATA_RECORD_MAX;
		next_2 %= PLAY_DATA_RECORD_MAX;
	}

	void setNext(PlayData data)
	{
		indexInc();

		_data[next_2].x = data.x;
		_data[next_2].y = data.y;
		_data[next_2].z = data.z;
		_data[next_2].e = data.e;
		_data[next_2].eef = data.eef;		
	}

	void getData(int type, PlayData& data)
	{
		
	}

	void setData(int type, PlayData data)
	{
		
	}

	void getCurData(PlayData& data)
	{
		data.x = _data[cur].x;
		data.y = _data[cur].y;
		data.z = _data[cur].z;
		data.e = _data[cur].e;
		data.eef = _data[cur].eef;
		
	}

	void replaceInvalidData(PlayData data)
	{
		_data[cur].x = _data[next].x;
		_data[cur].y = _data[next].y;
		_data[cur].z = _data[next].z;
		_data[cur].e = _data[next].e;
		_data[cur].eef = _data[next].eef;		

		_data[next].x = _data[next_2].x;
		_data[next].y = _data[next_2].y;
		_data[next].z = _data[next_2].z;
		_data[next].e = _data[next_2].e;
		_data[next].eef = _data[next_2].eef;	

		_data[next_2].x = data.x;
		_data[next_2].y = data.y;
		_data[next_2].z = data.z;
		_data[next_2].e = data.e;
		_data[next_2].eef = data.eef;		
	}

	bool isDataValid(int dir[4])
	{
		service_debug("dir: %d, %d, %d, %d\r\n", dir[0], dir[1], dir[2], dir[3]);

		if (dir[1] != 0 && dir[0] != 0 && dir[2] != 0 && dir[1] != dir[0] && dir[1] != dir[2])
		{
			service_debug("!!!!!!!!!!!!!!!Invalid data!\r\n");
			return false;
		}

		return true;
	}

	bool isXDataValid()
	{
		int dir[4] = {0};


		int cur_pos = pre;
		int last_pos = 0;
		
		for (int i = 0; i < (PLAY_DATA_RECORD_MAX-1); i++)
		{
			cur_pos = (pre + i) % PLAY_DATA_RECORD_MAX;
			last_pos = (pre + i - 1 + PLAY_DATA_RECORD_MAX) % PLAY_DATA_RECORD_MAX;

			if (abs(_data[cur_pos].x - _data[last_pos].x) < 0.00001)
			{
				dir[i] = 0;
			}
			else if (_data[cur_pos].x > _data[last_pos].x)
			{
				dir[i] = 1;
			}
			else
			{
				dir[i] = -1;
			}
		}

		service_debug("x data: %f, %f, %f, %f, %f\r\n", _data[pre_2].x, _data[pre].x, _data[cur].x, _data[next].x, _data[next_2].x);

		return isDataValid(dir);
	}

	bool isYDataValid()
	{
		int dir[4] = {0};


		int cur_pos = pre;
		int last_pos = 0;
		
		for (int i = 0; i < (PLAY_DATA_RECORD_MAX-1); i++)
		{
			cur_pos = (pre + i) % PLAY_DATA_RECORD_MAX;
			last_pos = (pre + i - 1 + PLAY_DATA_RECORD_MAX) % PLAY_DATA_RECORD_MAX;

			if (abs(_data[cur_pos].y - _data[last_pos].y) < 0.00001)
			{
				dir[i] = 0;
			}
			else if (_data[cur_pos].y > _data[last_pos].y)
			{
				dir[i] = 1;
			}
			else
			{
				dir[i] = -1;
			}
		}

		service_debug("y data: %f, %f, %f, %f, %f\r\n", _data[pre_2].y, _data[pre].y, _data[cur].y, _data[next].y, _data[next_2].y);
		return isDataValid(dir);	
	}

	bool isZDateValid()
	{
		int dir[4] = {0};


		int cur_pos = pre;
		int last_pos = 0;
		
		for (int i = 0; i < (PLAY_DATA_RECORD_MAX-1); i++)
		{
			cur_pos = (pre + i) % PLAY_DATA_RECORD_MAX;
			last_pos = (pre + i - 1 + PLAY_DATA_RECORD_MAX) % PLAY_DATA_RECORD_MAX;

			if (abs(_data[cur_pos].z - _data[last_pos].z) < 0.00001)
			{
				dir[i] = 0;
			}
			else if (_data[cur_pos].z > _data[last_pos].z)
			{
				dir[i] = 1;
			}
			else
			{
				dir[i] = -1;
			}
		}
		service_debug("z data: %f, %f, %f, %f, %f\r\n", _data[pre_2].z, _data[pre].z, _data[cur].z, _data[next].z, _data[next_2].z);

		return isDataValid(dir);		
	}

	bool isCurDataValid()
	{
		if (!isXDataValid())
			return false;

		if (!isYDataValid())
			return false;

		if (!isZDateValid())
			return false;

		return true;
	}

	bool isCurDataDelay()
	{
		if (abs(_data[cur].x - _data[pre].x) > 0.1)
			return false;

		if (abs(_data[cur].y - _data[pre].y) > 0.1)
			return false;	

		if (abs(_data[cur].z - _data[pre].z) > 0.1)
			return false;	

		return true;
	}
	
};

bool readNextData(PlayData& data, uint16_t& addr)
{
	if (addr > 65520)
	{
		return false;
	}

	unsigned char chdata[10];

	recorder.read(addr, chdata, 10);
	
	
	uint16_t angledata[5];
	double realdata[5];

	

	for (int i = 0; i < 5; i++)
	{
		angledata[i] = (chdata[2*i] << 8) + chdata[2*i+1];
		realdata[i] = ((double)angledata[i]) / 100;
	}	

	service_debug("mRecordAddr = %d, data=%f, %f, %f, %f\r\n", addr, realdata[0], realdata[1], realdata[2], realdata[3]);

	addr += 10;

	if (angledata[0] == 0xffff)
		return false;

	data.x = realdata[0];
	data.y = realdata[1];
	data.z = realdata[2];
	data.e = realdata[3];
	data.eef = angledata[4];



	return true;
}

bool uArmService::play()
{
	static PlayDataRecord play_data_record;
	static PointDataFilter point_data_filter;
	PlayData data;
	PlayData curData;
	static unsigned long lastPlayTime = 0;

	if (mSysStatus != SINGLE_PLAY_MODE && mSysStatus != LOOP_PLAY_MODE)
	{
		if (play_state != PLAY_STATE_IDLE)
		{
			ledAllOff();		

			pumpOff();
			gripperRelease();

			mRecordAddr = 65535;

			play_state = PLAY_STATE_IDLE;
		}
		
		return;
	}
	
	switch (play_state)
	{
	case PLAY_STATE_IDLE:
		if (mSysStatus == SINGLE_PLAY_MODE || mSysStatus == LOOP_PLAY_MODE)
		{
			play_state = PLAY_STATE_START;
			mRecordAddr = 0;
			point_data_filter.reset();
		}
		break;
		
	case PLAY_STATE_START:
		// read first 5 data
		service_debug("PLAY_STATE_START \r\n");
		for (int i = 0; i < PLAY_DATA_RECORD_MAX; i++)
		{
			delay(5);
			if (readNextData(data, mRecordAddr))
			{
				play_data_record.initData(i, data);
			}
			else
			{
				play_state = PLAY_STATE_DONE;
				return;
			}
		}

		play_state = PLAY_STATE_RUN;
		break;
		
	case PLAY_STATE_RUN:
		if (play_data_record.isCurDataValid())
		{
			
			play_data_record.getCurData(curData);
			
			if (getHWSubversion() >= SERVO_HW_FIX_VER)
			{
				servo[0].write(curData.e);
			}
			else
			{
				servo_write(curData.e, true);
			}
			
			if (curData.eef >= 0x10)
			{
				gripperCatch();
			}
			else if (curData.eef >= 0x01)
			{
				pumpOn();
			}
			else
			{
				pumpOff();
				gripperRelease();
			}

		
			if (play_data_record.isCurDataDelay())
			{
				play_state = PLAY_STATE_WAIT;
			}
			else
			{
				/*
				point_data in;
				point_data out;
				
				in.x = curData.x;
				in.y = curData.y;
				in.z = curData.z;
				
				
				point_data_filter.filter(in, out)
				
			
				getXYZFromAngle(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], out.x, out.y, out.z);
				*/

				getXYZFromAngle(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], curData.x, curData.y, curData.z);
				
				float target[NUM_AXIS];
				LOOP_XYZE(i) target[i] = destination[i];
			
		
				float difference[NUM_AXIS];
				LOOP_XYZE(i) difference[i] = target[i] - current_position[i];
		
		
			  
				float cartesian_mm = sqrt(sq(difference[X_AXIS]) + sq(difference[Y_AXIS]) + sq(difference[Z_AXIS]));
				feedrate_mm_m = cartesian_mm * 20 * 60 * SPEED_X; // the interval of points is 50ms
		
				line_to_destination_play_mode(feedrate_mm_m);

				set_current_to_destination();
				
				play_state = PLAY_STATE_GET_NEXT;
			}
		
		}
		else
		{
			service_debug("PLAY_STATE_REPLACE_CUR \r\n");
			play_state = PLAY_STATE_REPLACE_CUR;
		}
		break;
		
	case PLAY_STATE_WAIT:
		if (!get_block_running())
		{
			service_debug("PLAY_STATE_DELAY \r\n");
			play_state = PLAY_STATE_DELAY;
			lastPlayTime = millis();
		}
		break;

	case PLAY_STATE_DELAY:
		if (millis() - lastPlayTime >= TICK_INTERVAL)
		{
			play_state = PLAY_STATE_GET_NEXT;
		}
		break;

	case PLAY_STATE_GET_NEXT:
		if (readNextData(data, mRecordAddr))
		{
			play_data_record.setNext(data);
			play_state = PLAY_STATE_RUN;
		}
		else
		{
			play_state = PLAY_STATE_DONE;
		}
		break;

	case PLAY_STATE_REPLACE_CUR:
		
		if (readNextData(data, mRecordAddr))
		{
			play_data_record.replaceInvalidData(data);
			play_state = PLAY_STATE_RUN;
		}
		else
		{
			play_state = PLAY_STATE_DONE;
		}		
		break;

	
		
	case PLAY_STATE_END:
		break;
		
	case PALY_STATE_END2:
		break;
		
	case PLAY_STATE_DONE:
		service_debug("PLAY_STATE_DONE \r\n");
		if (mSysStatus == SINGLE_PLAY_MODE)
			mSysStatus = NORMAL_MODE;
		ledAllOff();		

		pumpOff();
		gripperRelease();

		mRecordAddr = 65535;
		
		play_state = PLAY_STATE_IDLE;
		break;
			
	}
}


#if 0
bool uArmService::play()
{
	static bool need_check = false;
	static unsigned long lastPlayTime = 0;
	static PointDataFilter point_data_filter;

	if(/*need_check &&*/ (millis() - lastPlayTime < TICK_INTERVAL))
	{
		return;
	}   

	need_check = false;
	lastPlayTime = millis();

	unsigned char data[10]; // 0: L  1: R  2: Rotation 3: hand rotation 4:gripper

	if (mRecordAddr >= 65530)
	{
		mRecordAddr = 0;
		point_data_filter.reset();
		
		if (mSysStatus == SINGLE_PLAY_MODE)
		{
			mSysStatus = NORMAL_MODE;
			ledAllOff();
		}
		else if (mSysStatus == LOOP_PLAY_MODE)
		{

		}
		else
		{
			mSysStatus = NORMAL_MODE;
		}
	}

	if (mSysStatus != SINGLE_PLAY_MODE && mSysStatus != LOOP_PLAY_MODE)
	{
		point_data_filter.reset();
		return false;
	}

	recorder.read(mRecordAddr, data, 10);
	mRecordAddr += 10;

	uint16_t angledata[5];
	double realdata[5];
	

	for (int i = 0; i < 5; i++)
	{
		angledata[i] = (data[2*i] << 8) + data[2*i+1];
		realdata[i] = ((double)angledata[i]) / 100;
		
	}

	debugPrint("mRecordAddr = %d, data=%f, %f, %f, %f\r\n", mRecordAddr, realdata[0], realdata[1], realdata[2], realdata[3]);


	point_data in;
	point_data out;

	in.x = realdata[0];
	in.y = realdata[1];
	in.z = realdata[2];

	point_data_filter.filter(in, out);



	

	// !!! Do not comment
	// !!! Tt's weird that the program will die if not use debugPrint or msprintf. Why???
	char buf[200];
	msprintf(buf, "mRecordAddr = %d, data=%f, %f, %f, %f\r\n", mRecordAddr, realdata[0], realdata[1], realdata[2], realdata[3]);


	if (angledata[0] != 0xffff)
	{
		if (getHWSubversion() >= SERVO_HW_FIX_VER)
		{
			servo[0].write((double)realdata[3]);
		}
		else
		{
			servo_write((double)realdata[3], true);
		}
		
		if (angledata[4] >= 0x10)
		{
			gripperCatch();
		}
		else if (angledata[4] >= 0x01)
		{
			pumpOn();
		}
		else
		{
			pumpOff();
			gripperRelease();
		}

		getXYZFromAngle(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], out.x, out.y, out.z);

		float target[NUM_AXIS];
		LOOP_XYZE(i) target[i] = destination[i];
	

  		float difference[NUM_AXIS];
	  	LOOP_XYZE(i) difference[i] = target[i] - current_position[i];


	  
	  	float cartesian_mm = sqrt(sq(difference[X_AXIS]) + sq(difference[Y_AXIS]) + sq(difference[Z_AXIS]));
	  	feedrate_mm_m = cartesian_mm * 20 * 60 * SPEED_X; // the interval of points is 50ms

		/*
		if (line_to_destination_play_mode(feedrate_mm_m))
		{
			need_check = false;
		}
		else
		{
			need_check = true;
		}
		*/
		prepare_move_to_destination();

		set_current_to_destination();

	}
	else
	{

		pumpOff();
		gripperRelease();

		mRecordAddr = 65535;
		point_data_filter.reset();
		return false;
		
	}


	return true;
}

#endif // 0

bool uArmService::record()
{
	service_debug("mRecordAddr = %d  ", mRecordAddr);


	if(mRecordAddr <= 65525)
	{
		uint8_t data[10]; // 0: L  1: R  2: Rotation 3: hand rotation 4:gripper
		uint16_t angledata[5];
		if((mRecordAddr != 65525) && (mSysStatus != LEARNING_MODE_STOP))
		{

			angledata[0] = (uint16_t)(get_current_angle(0) * 100);
			angledata[1] = (uint16_t)(get_current_angle(1) * 100);
			angledata[2] = (uint16_t)(get_current_angle(2) * 100);
			angledata[3] = (uint16_t)(get_current_angle(3) * 100);
			if (!getSwitchState())
			{
				angledata[4] = getPumpStatus() > 0 ? 1 : 0;
			}
			else
			{
				angledata[4] = getGripperStatus() > 0 ? 0x10 : 0;
			}

			service_debug("b=%d, l=%d, r= %d,  t=%d\r\n", angledata[0], angledata[1], angledata[2], angledata[3]);
		}
		else
		{
			//angledata[0] = 0xffff;//ending flag
			for (int i = 0; i < 10; i++)
			{
				data[i] = 255;
			}

			
			delay(5);
			recorder.write(mRecordAddr, data, 10);



			return false;
		}


		for (int i = 0; i < 5; i++)
		{
			data[2 * i] = (angledata[i] & 0xff00 ) >> 8;
			data[2*i + 1] = angledata[i] & 0xff;
		}


		recorder.write(mRecordAddr, data, 10);
		mRecordAddr += 10;

		return true;
	}
	else
	{
		return false;
	}

}
