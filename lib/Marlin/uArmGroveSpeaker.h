/*******************************************
/*******************************************
/***@file	uArmGroveSpeaker.h
/***@author	LUYU
/***@email	yu.lu@ufactory.cc
/***@date	20170908
/*******************************************
*******************************************/


#ifndef _UARMGROVE_SPEAKER_H_
#define _UARMGROVE_SPEAKER_H_

#include <Arduino.h>
#include "Marlin.h"

#include "uArmGroveBase.h"
#include "GroveControl.h"

class uArmGroveSpeaker : public uArmGroveBase
{
public:
	uArmGroveSpeaker();
	int BassTab[7] = {1911,1702,1516,1431,1275,1136,1012};		//bass 1~7

public:
	bool init(uint8_t portNum, uint8_t clk_pin, uint8_t dat_pin);

	void report();
	void control();
	void tick();

private:
	GroveControl _uArmGroveSpeaker;
};


#endif // _UARMGROVE_SPEAKER_H_
