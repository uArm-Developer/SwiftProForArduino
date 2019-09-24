#include "uarm_timer.h"

struct time_param_t {
  void (*time_callback)();
  volatile unsigned int tcnt;

	unsigned short pwm_period;
	unsigned char clock_select_bits;
};

struct time_param_t time2 = {0};
struct time_param_t time3 = {0};
struct time_param_t time4 = {0};
struct time_param_t time5 = {0};



/*  resolution = 0.001   1ms interrupt
 * 
 */
void time2_set(double resolution, void (*callback)()){
  float prescaler = 0.0;
  time2.time_callback = callback;
  TIMSK2 &= ~(1<<TOIE2);
  TCCR2A &= ~((1<<WGM21) | (1<<WGM20));
  TCCR2B &= ~(1<<WGM22);
  ASSR &= ~(1<<AS2);
  TIMSK2 &= ~(1<<OCIE2A);
  
  if ((F_CPU >= 1000000UL) && (F_CPU <= 16000000UL)) {  // prescaler set to 64
    TCCR2B |= (1<<CS22);
    TCCR2B &= ~((1<<CS21) | (1<<CS20));
    prescaler = 64.0;
  } else if (F_CPU < 1000000UL) { // prescaler set to 8
    TCCR2B |= (1<<CS21);
    TCCR2B &= ~((1<<CS22) | (1<<CS20));
    prescaler = 8.0;
  } else { // F_CPU > 16Mhz, prescaler set to 128
    TCCR2B |= ((1<<CS22) | (1<<CS20));
    TCCR2B &= ~(1<<CS21);
    prescaler = 128.0;
  } 
  time2.tcnt = 0xff + 1 - (int)((float)F_CPU * resolution / prescaler);
}

void time2_start(void){
  TCNT2 = time2.tcnt;
  TIMSK2 |= (1<<TOIE2);
}

void time2_stop(void){
  TIMSK2 &= ~(1<<TOIE2);
}

ISR(TIMER2_OVF_vect){
  TCNT2 = time2.tcnt;
  if( time2.time_callback != NULL ){
    time2.time_callback();
  }
}


/*  resolution = 0.001   1ms interrupt
 * 
 */

void time3_set(double resolution, void (*callback)()){
  time3.time_callback = callback;
  TCCR3A=0;
  TCCR3B=(1<<CS32)|(1<<CS30);
  time3.tcnt = 0xffff + 1 - (int)((float)F_CPU * resolution / 1024);
}

void time3_start(void){
  TCNT3 = time3.tcnt;
  TIMSK3 |= (1<<TOIE3);
}

void time3_stop(void){
  TIMSK3 &= ~(1<<TOIE3);
}

ISR(TIMER3_OVF_vect){
  TCNT3 = time3.tcnt;
  if( time3.time_callback != NULL ){
    time3.time_callback();
  }
}


/*  resolution = 0.001   1ms interrupt
 * 
 */

void time4_set(double resolution, void (*callback)()){
  time4.time_callback = callback;
  TCCR4A=0;
  TCCR4B=(1<<CS42)|(1<<CS40);
  time4.tcnt = 0xffff + 1 - (int)((float)F_CPU * resolution / 1024);
}

void time4_start(void){
  TCNT4 = time4.tcnt;
  TIMSK4 |= (1<<TOIE4);
}

void time4_stop(void){
  TIMSK4 &= ~(1<<TOIE4);
}

ISR(TIMER4_OVF_vect){
  TCNT4 = time4.tcnt;
  if( time4.time_callback != NULL ){
    time4.time_callback();
  }
}
#define TIMER4_RESOLUTION 65536UL

void time4_set_period(unsigned long period_us){
 	/********************** set period **************************/
  const unsigned long cycles = (F_CPU / 2000000) * period_us;
  if (cycles < TIMER4_RESOLUTION) {
    time4.clock_select_bits = _BV(CS40);
    time4.pwm_period = cycles;
  } else
  if (cycles < TIMER4_RESOLUTION * 8) {
    time4.clock_select_bits = _BV(CS41);
    time4.pwm_period = cycles / 8;
  } else
  if (cycles < TIMER4_RESOLUTION * 64) {
    time4.clock_select_bits = _BV(CS41) | _BV(CS40);
    time4.pwm_period = cycles / 64;
  } else
  if (cycles < TIMER4_RESOLUTION * 256) {
    time4.clock_select_bits = _BV(CS42);
    time4.pwm_period = cycles / 256;
  } else
  if (cycles < TIMER4_RESOLUTION * 1024) {
    time4.clock_select_bits = _BV(CS42) | _BV(CS40);
    time4.pwm_period = cycles / 1024;
  } else {
    time4.clock_select_bits = _BV(CS42) | _BV(CS40);
    time4.pwm_period = TIMER4_RESOLUTION - 1;
  }
  ICR4 = time4.pwm_period;
  TCCR4B = _BV(WGM43) | time4.clock_select_bits;		
}

void time4_set_duty(char pin, unsigned int duty){
	
	/******************* set pwm pin ***************************/
	unsigned long dutyCycle = time4.pwm_period;
  dutyCycle *= duty;
  dutyCycle >>= 10;
	switch( pin ){
		case 3:
								DDRH  |= 1<<3;
								TCCR4A |= _BV(COM4A1);
								OCR4A = dutyCycle;
			break;
		case 4:
								DDRH  |= 1<<4;
								TCCR4A |= _BV(COM4B1);
								OCR4B = dutyCycle;
			break;
		case 5:
								DDRH  |= 1<<5;
								TCCR4A |= _BV(COM4C1);
								OCR4C = dutyCycle;
			break;
	}

	TCCR4B = _BV(WGM43) | time4.clock_select_bits;
	
}

void time4_pwm_init(unsigned long period_us){
  TCCR4B = _BV(WGM43);        // set mode as phase and frequency correct pwm, stop the timer
  TCCR4A = 0;                 // clear control register A 
  time4_set_period(period_us);	
}

/*  resolution = 0.001   1ms interrupt
 * 
 */

void time5_set(double resolution, void (*callback)()){
  time5.time_callback = callback;
  TCCR5A=0;
  TCCR5B=(1<<CS52)|(1<<CS50);
  time5.tcnt = 0xffff + 1 - (int)((float)F_CPU * resolution / 1024);
}

void time5_start(void){
  TCNT5 = time5.tcnt;
  TIMSK5 |= (1<<TOIE5);
}

void time5_stop(void){
  TIMSK5 &= ~(1<<TOIE5);
}

ISR(TIMER5_OVF_vect){
  TCNT5 = time5.tcnt;
  if( time5.time_callback != NULL ){
    time5.time_callback();
  }
}






