#ifndef _STEP_LOWLEVEL_H_
#define _STEP_LOWLEVEL_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <math.h>
#include <inttypes.h>    
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <Arduino.h>


#define STEP_LIB_VERSION		1.2.2


#define DIR_PIN         5     
#define DIR_PORT        PORTH
#define DIR_DDR         DDRH
#define STEP_PIN        6
#define STEP_PORT       PORTH
#define STEP_DDR        DDRH
#define ENABLE_PIN      7
#define ENABLE_PORT     PORTD
#define ENABLE_DDR      DDRD

#define DIR_OUTPUT			DIR_DDR	|= (1<<DIR_PIN)
#define DIR_HIGH				DIR_PORT |= (1<<DIR_PIN)
#define DIR_LOW					DIR_PORT &= ~(1<<DIR_PIN)
#define STEP_OUTPUT			STEP_DDR |= (1<<STEP_PIN)
#define STEP_HIGH				STEP_PORT |= (1<<STEP_PIN)
#define STEP_LOW				STEP_PORT &= ~(1<<STEP_PIN)
#define ENABLE_OUTPUT		ENABLE_DDR |= (1<<ENABLE_PIN)
#define ENABLE_HIGH			ENABLE_PORT	|= (1<<ENABLE_PIN)
#define ENABLE_LOW			ENABLE_PORT &= ~(1<<ENABLE_PIN)

/*
NOTE:
		the time creater is timer4, must sure you didn't use timer4 to do other things!
*/
bool step_init_ll(void);

/*
* steps 							:the pulse num   steps>0 forward,steps<0 backward
* timer_min_val 			:the timer minimum load count   80 max
* complete_callback		:move done callback
* wait 								:default is wait
*/
bool set_steps(long steps, long timer_min_val, void(*complete_callback)(void), bool wait);



/*
* direction						:direction>0 forward direction<0 backward
* timer_min_val 			:the timer minimum load count   80 max
* complete_callback		:speed reach callback
* wait								:default is no wait
*/

bool set_speed(int8_t direction, long timer_min_val, void(*complete_callback)(void), bool wait);


bool step_terminate(void);

#endif
