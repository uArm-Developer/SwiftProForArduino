#include "uarm_common.h"
#include "uarm_coord_convert.h"


/*	end-effector driver
 *
 */


/*************** servo mode ****************/
float duty_ms = 1.7;
static void servo_init(void){
	time4_pwm_init(20000);
  time4_set_duty( 3, (duty_ms/21.0) * 1023 );
}

static void servo_set_angle(float angle){
	if( angle > 180 ){ angle = 0; }
	if( angle < 0 ){ angle = 180; }
	angle = 180 - angle;
	
	duty_ms = (angle / 180.0) * 2.0 + 0.7;
	time4_set_duty( 3, (duty_ms/21.0) * 1023 );
	//delay_ms(500);
}

static void servo_deinit(void){
	time4_stop();
}

static float servo_get_angle(void){
	return 180 - (duty_ms - 0.7) / 2.0 *180;
}

/************ steper mode ******************/
int32_t step_count = 0;
float steper_current_angle = 0;

void step_creater(void){
	static bool status_flag = false;
	if( step_count > 0 ){
		if( status_flag ){
			status_flag = false;
			step_count--;
			PORTH |= (1<<6);
		}else{
			status_flag = true;
			PORTH &= ~(1<<6);
		}
	}else{
		time4_stop();
		step_count = 0;
		//DB_PRINT_STR( "time4 stop\r\n" );
	}
}

static void steper_init(void){
	DDRH |= (1<<5);		// <! DIR
  DDRH |= (1<<6);		// <! STEP	

	PORTH |= (1<<5);	// <! DIR clockwise
}

static void steper_set_angle(float angle){
	if( uarm.param.work_mode==WORK_MODE_STEPER_FLAT ){
		step_count = angle / 0.9 * 64;
		time4_set( 0.0002, step_creater );
	}else if( uarm.param.work_mode == WORK_MODE_STEPER_STANDARD ){
		step_count = angle / 1.8 * 64;
		time4_set( 0.0002, step_creater );		
	}
	time4_start();
}

static void steper_deinit(void){
	step_count = 0;
	time4_stop();
	uarm.effect_origin_check = false;
}

static float steper_get_angle(void){
	return steper_current_angle;
}


void end_effector_init(void){
	if( uarm.param.work_mode==WORK_MODE_STEPER_FLAT || uarm.param.work_mode==WORK_MODE_STEPER_STANDARD ){
		steper_init();
	}else{
		servo_init();
	}
}

void end_effector_set_angle(float angle){
	if( angle > 360 || angle < 0 ){ return; }

	if( uarm.param.work_mode==WORK_MODE_STEPER_FLAT || uarm.param.work_mode==WORK_MODE_STEPER_STANDARD ){
		float offset_angle = angle - steper_current_angle;
		steper_current_angle = angle;
		if( offset_angle > 0 ){
			PORTH &= ~(1<<5);	// <!  anticlockwise
		}else{
			PORTH |= (1<<5);	// <! clockwise
		}
		steper_set_angle(fabs(offset_angle));
	}else{
		servo_set_angle(angle);
	}
}

void end_effector_deinit(void){
	steper_deinit();
	servo_deinit();

}

float end_effector_get_angle(void){
	if( uarm.param.work_mode==WORK_MODE_STEPER_FLAT || uarm.param.work_mode==WORK_MODE_STEPER_STANDARD ){
		return steper_get_angle();
	}else{
		return servo_get_angle();
	}
}

void end_effector_get_origin(void){
	if( uarm.param.work_mode==WORK_MODE_STEPER_FLAT || uarm.param.work_mode==WORK_MODE_STEPER_STANDARD ){
		steper_current_angle = 0;
		DDRK &= ~(1<<5);
		PORTK |= (1<<5);
		
		PORTH |= (1<<5);   // <! clockwise
		steper_set_angle(360);
		uarm.effect_origin_check = true;

	}
}

void end_effector_check_limit(void){
	if( (PINK & (1<<5)) == 0 ){
		time4_stop();
		uarm.effect_origin_check = false;
		PORTH &= ~(1<<5);	// <! DIR counterclockwise
		steper_current_angle = -uarm.param.effect_angle_offset;
		end_effector_set_angle(90);
	}
	if( step_count == 0 ){			// <! check timeout
		uarm.effect_origin_check = false;	
		DB_PRINT_STR( "effect check timeout!\r\n" );
	}
}

/*
void end_effect_adjust_angle(void){
	float current_angle = end_effector_get_angle();
	steper_current_angle -= uarm.param.effect_angle_offset;
	end_effector_set_angle(current_angle);
}*/

 /*  beep driver
	*
	*/
uint16_t beep_duration = 520;	
static void beep_creater_callback(void){
  static bool state = false;
  static uint16_t cnt  = 0;
  if( !state ){
    state = true;
    PORTL |= (1<<5);  
  }else{
    state = false;
    PORTL &= ~(1<<5);  
  }
  if( cnt++ > beep_duration ){
    cnt = 0;
    time2_stop();
  }	
}

void beep_tone(unsigned long duration, double frequency){
  DDRL  |= 1<<5;  
  PORTL &= ~(1<<5); 
	beep_duration = duration * 2;
   
  time2_set( frequency / 2000000.0, beep_creater_callback );
  time2_start();	
}


/*	delay time  : ms
 *	
 */
void cycle_report_coord(void){
	float x = 0, y = 0, z = 0, angle_e;
	float angle_l = 0, angle_r = 0, angle_b =0;
	char l_str[20] = {0}, r_str[20] = {0}, b_str[20] = {0}, e_str[20] = {0};

	angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	angle_r = calculate_current_angle(CHANNEL_ARMR);
	angle_b = calculate_current_angle(CHANNEL_BASE)-90;
	angle_e = servo_get_angle();

	angle_to_coord( angle_l, angle_r, angle_b, &x, &y, &z );

	coord_arm2effect( &x, &y, &z );
	dtostrf( x, 5, 4, l_str );
	dtostrf( y, 5, 4, r_str );
	dtostrf( z, 5, 4, b_str );
	dtostrf( angle_e, 5, 4, e_str );
	
	uart_printf( "@3 X%s Y%s Z%s R%s\n", l_str, r_str, b_str, e_str );
}

bool time_tick_work = false;
bool cycle_tick_flag = false;
int gcode_delay_ms = 0;
int cycle_delay_ms = 0;

static void tick_callback(void){			//per 1ms
	static int delay_cnt = 0;
	static int cycle_cnt = 0;

	if( gcode_delay_ms ){								// run once
		if( delay_cnt++ >= gcode_delay_ms ){
			delay_cnt = 0;
			gcode_delay_ms = 0; 
			uarm.gcode_delay_flag = false;
			//DB_PRINT_STR( "delay done\r\n" );
		}
	}

	if( cycle_delay_ms ){								// run cycle
		
		if( cycle_cnt++ >= cycle_delay_ms ){
			cycle_cnt = 0;
			uarm.cycle_report_flag = true;
			//DB_PRINT_STR( "cycle call\r\n" );
			//cycle_report_coord();
		}
	}
	
	if( (uarm.gcode_delay_flag == false)&&( cycle_tick_flag == false) ){
		time5_stop();
		time_tick_work = false;
		gcode_delay_ms = 0;
		cycle_delay_ms = 0;
		
		delay_cnt = 0;
		cycle_cnt = 0;
	//	DB_PRINT_STR( "stop time5\r\n" );
	}
 }

 static void time_tick_init(void){
	 time5_set( 0.001, tick_callback );
	 time5_start();
//	 DB_PRINT_STR( "start time5\r\n" );
 }

void gcode_cmd_delay(int ms){
	gcode_delay_ms = ms;
	uarm.gcode_delay_flag = true;
	if( !time_tick_work ){
		time_tick_init();
		time_tick_work = true;
	}
}

void cycle_report_start(int ms){
	if( ms <= 0 ){
		cycle_report_stop();
		return;
	}
	
	cycle_delay_ms = ms;
	cycle_tick_flag = true;

	if( !time_tick_work ){
		time_tick_init();
		time_tick_work = true;
	}

}

void cycle_report_stop(void){
	cycle_tick_flag = false;
	cycle_delay_ms = 0;
}


/*	param to eeprom
 *		
 */
 
void read_sys_param(void){
	int8_t read_size = 0;
	unsigned  int  read_addr = 0;
	char *p = NULL;
	

	p = (char *)(&(uarm.param.work_mode));
	read_size = sizeof(uarm.param.work_mode);
	read_addr = EEPROM_MODE_ADDR;
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}

	p = (char *)(&(uarm.param.front_offset));
	read_size = sizeof(uarm.param.front_offset);
	read_addr = EEPROM_FRONT_OFFSET_ADDR;
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}	

	p = (char *)(&(uarm.param.high_offset));
	read_size = sizeof(uarm.param.high_offset);
	read_addr = EEPROM_HEIGHT_OFFSET_ADDR;
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}	

	p = (char *)(&(uarm.param.effect_angle_offset));
	read_size = sizeof(uarm.param.effect_angle_offset);
	read_addr = EEPROM_EFFECT_ANGLE_OFFSET_ADDR;
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}
	if( uarm.param.effect_angle_offset > 360 || isnan(uarm.param.effect_angle_offset) ){
		uarm.param.effect_angle_offset = 0;
	}
	
/*	char l_str[20], r_str[20], b_str[20];
	dtostrf( uarm.param.work_mode, 5, 4, l_str );
	dtostrf( uarm.param.front_offset, 5, 4, r_str );
	dtostrf( uarm.param.high_offset, 5, 4, b_str );
	
	DB_PRINT_STR( "mode:%s, front:%s, high:%s\r\n", l_str, r_str, b_str );*/

	char l_str[20], r_str[20], b_str[20];
	dtostrf( uarm.param.effect_angle_offset, 5, 4, b_str );
	DB_PRINT_STR( "angle offset:%s\r\n", b_str );

}

void save_sys_param(void){
	int8_t write_size = 0;
	unsigned int write_addr = 0;
	char *p = NULL;

	p = (char *)(&(uarm.param.work_mode));
	write_size = sizeof(uarm.param.work_mode);
	write_addr = EEPROM_MODE_ADDR;	
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}	

	p = (char *)(&(uarm.param.front_offset));
	write_size = sizeof(uarm.param.front_offset);
	write_addr = EEPROM_FRONT_OFFSET_ADDR;	
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}	

	p = (char *)(&(uarm.param.high_offset));
	write_size = sizeof(uarm.param.high_offset);
	write_addr = EEPROM_HEIGHT_OFFSET_ADDR;	
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}	

	p = (char *)(&(uarm.param.effect_angle_offset));
	write_size = sizeof(uarm.param.effect_angle_offset);
	write_addr = EEPROM_EFFECT_ANGLE_OFFSET_ADDR;	
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}	
}


void read_hardware_version(void){
#define HARD_MASK	 ( 1<<6 | 1<<5 | 1<<4 | 1<<3 )
	DDRJ &= ~HARD_MASK;
	if( (PINJ & HARD_MASK) == (1 <<6) ){
		strcpy( hardware_version, "V3.3.1" );
		settings_store_global_setting( 3, 0 );
	}else if( (PINJ & HARD_MASK) == 0x00 ){
		strcpy( hardware_version, "V3.3.0" );
		settings_store_global_setting( 3, 7 );
	}
	settings_init();
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


static int adc_read_value(uint8_t pin){
  uint8_t low, high;
  if (pin >= 54) pin -= 54; // allow for channel or pin numbers

  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
  ADMUX = (1 << 6) | (pin & 0x07);
  ADCSRA |= (1<<ADSC);
  while( !(ADCSRA&(1<<ADSC)) );
  low  = ADCL;
  high = ADCH;
  return (high << 8) | low;
}


static int getAnalogPinValue(unsigned int pin)
{
	unsigned int dat[8];


	for(int i = 0; i < 8; i++)
	{
		dat[i] = adc_read_value(pin);
	}

	_sort(dat, 8);

	unsigned int result = (dat[2]+dat[3]+dat[4]+dat[5])/4;

	return result;    
}



/*	pump
 *		
 */
void pump_on(void){
	DDRG |= 0x10;		// PG4 PUMP_D5_N
	DDRG |= 0x20;		// PG5 PUMP_D5
	
	PORTG |= 0x10;
	PORTG &= (~0x20); 	
}

void pump_off(void){
	DDRG |= 0x10; 	// PG4 PUMP_D5_N
	DDRG |= 0x20; 	// PG5 PUMP_D5
		
	PORTG &= (~0x10);
	PORTG |= 0x20;	

}

uint8_t get_pump_status(void){
	if( (PORTG & 0x10) == 0x00 ){ return 0; }
	uint16_t value = getAnalogPinValue(63);
//	DB_PRINT_STR( "ad:%d\r\n", value );
	if( value < 10){
		return 0;
	}else if( value <= 70 ){
		return 2;
	}else{
		return 1;
	}	
}

/*	gripper
 *		
 */

void gripper_relesae(void){
	DDRC |= 0x80; 	// PC7 gripper 
	PORTC |= 0x80;	
}

void gripper_catch(void){
	DDRC |= 0x80; 	// PC7 gripper 
	PORTC &= (~0x80);	
}

uint8_t get_gripper_status(void){
	#define MASK_G (1<<0)
	if( PORTC & 0x80 ){ return 0;	}

	DDRA &= ~(MASK_G);
	if( PINA & MASK_G ){
		return 1;
	}else{
		return 2;
	}
}

/*	gripper
 *		
 */
void laser_on(void){
	DDRH |= (1<<5);		// <! OUTPUT
  DDRH |= (1<<6);		// <! OUTPUT

	PORTH |= (1<<5);
	PORTH |= (1<<6);
}

void laser_off(void){
	DDRH |= (1<<5);		
  DDRH |= (1<<6);	

	PORTH &= ~(1<<5);
	PORTH &= ~(1<<6);
}

uint8_t get_laser_status(void){
	if( PORTH & (1<<5) ){
		return 1;
	}else{
		return 0;
	}
}


uint8_t get_limit_switch_status(void){
	if( (PING & (1<<3)) == 0 ){
		return 1;
	}else{
		return 0;
	}
}

uint8_t get_power_status(void){
	//uint16_t power_adc_value = getAnalogPinValue(59);
	uint16_t power_adc_value = adc_read_value(59);
	//DB_PRINT_STR( "%d\r\n", power_adc_value );
	
	if( power_adc_value > 100 ){
		return 1;
	}else{
		return 0;
	}
}

void check_motor_positon(void){
	if( sys.state == STATE_IDLE && uarm.motor_state_bits == 0x0F && uarm.power_state ){
		float current_angle_l = 0, current_angle_r = 0, current_angle_b = 0;

		coord_to_angle( uarm.coord_x, uarm.coord_y, uarm.coord_z, &current_angle_l, &current_angle_r, &current_angle_b );

		float reg_angle_l = calculate_current_angle(CHANNEL_ARML);		
		float reg_angle_r = calculate_current_angle(CHANNEL_ARMR);
		float reg_angle_b = calculate_current_angle(CHANNEL_BASE) ;

		if( fabs(current_angle_l-reg_angle_l)>0.5 || fabs(current_angle_r-reg_angle_r)>0.5 || fabs(current_angle_b-reg_angle_b+90)>0.5 ){
			char l_str[20], r_str[20], b_str[20];

			memset(&sys, 0, sizeof(system_t));  // Clear all system variables
			plan_sync_position();
    	gc_sync_position();
			
			uarm.init_arml_angle = reg_angle_l;
			uarm.init_armr_angle = reg_angle_r;
			uarm.init_base_angle = reg_angle_b;
			beep_tone(260, 1000);

			uarm.target_step[X_AXIS] = sys.position[X_AXIS];
			uarm.target_step[Y_AXIS] = sys.position[Y_AXIS];
			uarm.target_step[Z_AXIS] = sys.position[Z_AXIS];

	/*		angle_to_coord( uarm.init_arml_angle, uarm.init_armr_angle, uarm.init_base_angle-90,
											&(uarm.coord_x), &(uarm.coord_y), &(uarm.coord_z) );

			
			dtostrf( uarm.coord_x, 5, 4, l_str );
			dtostrf( uarm.coord_y, 5, 4, r_str );
			dtostrf( uarm.coord_z, 5, 4, b_str );

			DB_PRINT_STR( "coord: %s, %s, %s\r\n", l_str, r_str, b_str );*/

			
			float target[3] = {0};
			target[X_AXIS] = uarm.coord_x;
			target[Y_AXIS] = uarm.coord_y;
			target[Z_AXIS] = uarm.coord_z;
			coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
			mc_line( 0, target, 50, false );
			
		}
	}
}


