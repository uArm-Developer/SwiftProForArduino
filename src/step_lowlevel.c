#include "step_lowlevel.h"

enum step_state_e {
	SPEED_UP 				= 0,
	SPEED_CONSTANT,
	SPEED_DOWN,
};

enum step_mode_e {
	POSITION_MODE = 0,
	SPEED_MODE,
};

struct step_param_t {
	enum step_state_e state;
	enum step_mode_e mode;
	volatile long remain_counts;
	volatile long total_conuts;
	volatile long constant_conunts;	
	volatile long timer_load_val;
	volatile long timer_min_val;
	volatile long n;
	bool is_move_done;

	void(*positon_complete_callback)(void);
	void(*speed_complete_callback)(void);
}step_param = {0};


bool step_init_ll(void){
	DIR_OUTPUT;
	STEP_OUTPUT;
  
	DIR_LOW;
	STEP_LOW;

	memset( &step_param, 0x00, sizeof(struct step_param_t) );

	noInterrupts();
  TCCR4A = 0;                               // <! clear register value
  TCCR4B = 0;                               // <! clear register value
  TCNT4  = 0;
	OCR4A = 1600;
	TCCR4B |= (1 << WGM42);
	TCCR4B |= ((1 << CS41) | (1 << CS40));

  interrupts();

	return true;
}

bool set_steps(long steps, long timer_min_val, void(*complete_callback)(void), bool wait){
	if( step_param.n > 1 ){											// <! already work
		return false;
	}
  
	memset( &step_param, 0x00, sizeof(struct step_param_t) );

	step_param.mode = POSITION_MODE;
	step_param.total_conuts = step_param.remain_counts = abs(steps);
	step_param.timer_min_val = timer_min_val;
  
	step_param.timer_load_val = 1600;

	step_param.is_move_done = false;
	step_param.positon_complete_callback = complete_callback;

	if(steps > 0){														// <! changed the direction
		DIR_LOW;
	}else{
		DIR_HIGH;
	}

	step_param.n = 0;
	OCR4A = step_param.timer_load_val;
	TIMSK4 |= (1 << OCIE4A);									// <! enbale timer4 interrupt 	

	if( wait ){
		while( !step_param.is_move_done ){
			delay_us(100);
		};
	}

	return true;
}


bool set_speed(int8_t direction, long timer_min_val, void(*complete_callback)(void), bool wait){
	if( timer_min_val == -1 ){
    if( step_param.state == SPEED_DOWN ){           // <! already work
      return false;
    }
		step_param.state = SPEED_DOWN;
		if( wait ){
			while( step_param.n > 1 ){ delay_us(100); }
		}
	}else{
    if( step_param.n > 1 ){                        // <! already work
      return false;
    }    
    memset( &step_param, 0x00, sizeof(struct step_param_t) );
    step_param.mode = SPEED_MODE;
    step_param.speed_complete_callback = complete_callback;
		if(direction > 0){												    // <! changed the direction
			DIR_HIGH;
		}else{
			DIR_LOW;
		}
		
		step_param.state = SPEED_UP;
		step_param.timer_min_val = timer_min_val;
		step_param.timer_load_val = 1600;
		step_param.n = 0;
		OCR4A = step_param.timer_load_val;
		TIMSK4 |= (1 << OCIE4A);									// <! enbale timer4 interrupt		
		if( wait ){
			while( step_param.state != SPEED_CONSTANT ){ delay_us(100); }
		}
	}
	return true;
}

void position_interrrupt_handle(void){
	if( step_param.remain_counts-- ){
		STEP_HIGH;
		STEP_LOW;	
	}else{
		if( step_param.positon_complete_callback != NULL ){
			step_param.positon_complete_callback();
		}
		TIMSK4 &= ~(1 << OCIE4A);								// <! disable timer4 interrupt
		step_param.is_move_done = true;
	}

	switch( step_param.state ){
		case SPEED_UP:
			if( step_param.remain_counts>=step_param.total_conuts/2 ){
				step_param.n++;
				step_param.timer_load_val = step_param.timer_load_val - (2 * step_param.timer_load_val) / (4 * step_param.n + 1 );		// <! Tarlor series

				if( step_param.timer_load_val < step_param.timer_min_val ){
					step_param.constant_conunts = 2 * step_param.remain_counts - step_param.total_conuts;
					step_param.timer_load_val = step_param.timer_min_val;
					step_param.state = SPEED_CONSTANT;
				}
			}else{
				step_param.state = SPEED_DOWN;		// <! remain_counts reach half total counts, it mean starting to speed down
			}
			break;
		case SPEED_CONSTANT:
			if( step_param.constant_conunts-- <= 0 ){
				step_param.state = SPEED_DOWN;
			}
			break;
		case SPEED_DOWN:
			if( step_param.remain_counts ){
				step_param.n--;
				step_param.timer_load_val = (step_param.timer_load_val * (4 * step_param.n + 1)) / (4 * step_param.n + 1 - 2);			// <! Tarlor series
			}
			break;
	}
	OCR4A = step_param.timer_load_val;
}

void speed_interrupt_handle(void){
	STEP_HIGH;
	STEP_LOW;	

	switch( step_param.state ){
		case SPEED_UP:
				if( step_param.timer_load_val > step_param.timer_min_val ){
					step_param.n++;
					step_param.timer_load_val = step_param.timer_load_val - (2 * step_param.timer_load_val) / (4 * step_param.n + 1 ); 	// <! Tarlor series
				}else{
					if( step_param.speed_complete_callback != NULL ){
						step_param.speed_complete_callback();
						step_param.speed_complete_callback = NULL;
					}
					step_param.state = SPEED_CONSTANT;
				}
			break;
		case SPEED_CONSTANT:
			
			break;
		case SPEED_DOWN:
			if( step_param.n-- ){
				step_param.timer_load_val = (step_param.timer_load_val * (4 * step_param.n + 1)) / (4 * step_param.n + 1 - 2);			// <! Tarlor series
			}else{
				TIMSK4 &= ~(1 << OCIE4A);								// <! disable timer4 interrupt
				if( step_param.speed_complete_callback != NULL ){
					step_param.speed_complete_callback();
					step_param.speed_complete_callback = NULL;
				}
			}
			break;
	}
	
	OCR4A = step_param.timer_load_val;
}

bool step_terminate(void){
	memset( &step_param, 0x00, sizeof(struct step_param_t) );	
	noInterrupts();
  TCCR4A = 0;                               // <! clear register value
  TCCR4B = 0;                               // <! clear register value
  TCNT4  = 0;
	OCR4A = 1600;
	TCCR4B |= (1 << WGM42);
	TCCR4B |= ((1 << CS41) | (1 << CS40));

  interrupts();	
	TIMSK4 &= ~(1 << OCIE4A);								// <! disable timer4 interrupt
	return true;
}



ISR(TIMER4_COMPA_vect){
	switch( step_param.mode ){
		case POSITION_MODE:		position_interrrupt_handle();	break;
		case SPEED_MODE:			speed_interrupt_handle();			break;	
	}
}
