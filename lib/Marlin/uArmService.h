/**
  ******************************************************************************
  * @file	uArmService.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-17
  ******************************************************************************
  */

#ifndef _UARMSERVICE_H_
#define _UARMSERVICE_H_

#include <Arduino.h>
#include "Marlin.h"

enum BUTTON_ID
{
	BUTTON_MENU,
	BUTTON_PLAY,
	BUTTON_STATE,

	BUTTON_COUNT
};

#define NORMAL_MODE                 0
#define NORMAL_BT_CONNECTED_MODE    1
#define LEARNING_MODE               2
#define SINGLE_PLAY_MODE            3
#define LOOP_PLAY_MODE              4
#define LEARNING_MODE_STOP          5

class uArmService
{
public:
	uArmService();

	void init();
	void run();

	void btDetect();

	void setButtonService(bool on);

	void handleButtonEvent(BUTTON_ID button, unsigned char event);
	bool buttonServiceDisable();
	bool play();
	void disableBT(bool disable);
	bool btConnected();
	void ledAllOff();

private:	

	void systemRun();
	
	bool record();
	void tickTaskRun();
	void recorderTick();	

	void ledLearning();
	void ledPlaying();

	void tipDetect();
	void powerDetect();
private:
	unsigned char mButtonServiceDisable = false;
	unsigned char mSysStatus = NORMAL_MODE;
	unsigned int mRecordAddr = 0;
	unsigned char mBTDisable = false;
	unsigned long mReportStartTime;

	unsigned long mTickRecorderTime;	
	unsigned char mPowerState;
	unsigned char mTipState;
};

extern uArmService service;

#endif // _UARMSERVICE_H_
