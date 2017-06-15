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

void menuButtonClicked()
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

void menuButtonLongPressed()
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

void playButtonClicked()
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

void playButtonLongPressed()
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

void all_light_on()
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

void all_light_off()
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
	
	all_light_off();
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

	button_menu.setClickedCB(::all_light_on);
	button_play.setClickedCB(::all_light_off);
#else
	button_menu.setClickedCB(menuButtonClicked);
	button_menu.setLongPressedCB(menuButtonLongPressed);
	button_play.setClickedCB(playButtonClicked);
	button_play.setLongPressedCB(playButtonLongPressed);
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
				if (getHWSubversion() > 0)
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
			if (getHWSubversion() > 0)
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

void uArmService::run()
{
#ifdef SWIFT_TEST_MODE
	::test_mode_run();

	if (millis() - mTickRecorderTime >= 50)
	{
		mTickRecorderTime= millis();

		bt_test();
		led_seeed.tick();
	}

#else
	systemRun();

	if (millis() - mTickRecorderTime >= 50)
	{
		mTickRecorderTime= millis();
		recorderTick();
		powerDetect();
		tipDetect();

	}
#endif
}




bool uArmService::play()
{
	
	static unsigned long lastPlayTime = 0;

	if(millis() - lastPlayTime < TICK_INTERVAL)
	{
		return;
	}   

	lastPlayTime = millis();

	unsigned char data[5]; // 0: L  1: R  2: Rotation 3: hand rotation 4:gripper

	if (mRecordAddr >= 65535)
	{
		mRecordAddr = 0;

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

	// !!! Do not comment
	// !!! Tt's weird that the program will die if not use debugPrint or msprintf. Why???
	char buf[200];
	msprintf(buf, "mRecordAddr = %d, data=%f, %f, %f, %f\r\n", mRecordAddr, realdata[0], realdata[1], realdata[2], realdata[3]);


	if (angledata[0] != 0xffff)
	{
		if (getHWSubversion() > 0)
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

		getXYZFromAngle(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], realdata[0], realdata[1], realdata[2]);

		float target[NUM_AXIS];
		LOOP_XYZE(i) target[i] = destination[i];
	

  		float difference[NUM_AXIS];
	  	LOOP_XYZE(i) difference[i] = target[i] - current_position[i];


	  
	  	float cartesian_mm = sqrt(sq(difference[X_AXIS]) + sq(difference[Y_AXIS]) + sq(difference[Z_AXIS]));
	  	feedrate_mm_m = cartesian_mm * 20 * 60 * 2; // the interval of points is 50ms  2x speed

		prepare_move_to_destination();

		set_current_to_destination();

	}
	else
	{

		pumpOff();
		gripperRelease();

		mRecordAddr = 65535;

		return false;
		
	}


	return true;
}

bool uArmService::record()
{
	debugPrint("mRecordAddr = %d  ", mRecordAddr);

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

			debugPrint("b=%d, l=%d, r= %d,  t=%d\r\n", angledata[0], angledata[1], angledata[2], angledata[3]);
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
