#ifndef _UARM_TIMER_H_
#define _UARM_TIMER_H_

#include "uarm_common.h"

void time2_set(double resolution, void (*callback)());
void time2_start(void);
void time2_stop(void);

void time3_set(double resolution, void (*callback)());
void time3_start(void);
void time3_stop(void);

void time4_set(double resolution, void (*callback)());
void time4_start(void);
void time4_stop(void);
void time4_pwm_init(unsigned long period_us);
void time4_set_duty(char pin, unsigned int duty);
void time4_set_period(unsigned long period_us);




void time5_set(double resolution, void (*callback)());
void time5_start(void);
void time5_stop(void);


#endif
