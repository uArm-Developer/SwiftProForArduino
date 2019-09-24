#include "uarm_angle.h"
#include "uarm_iic.h"

//uint16_t reference_param[3] = {0};

struct refer_value_t {
	uint16_t param_b;
	uint16_t param_l;
	uint16_t param_r;
} reference = {0};

uint16_t reference_param_pointb[3] = {0};


uint16_t base_calibra_angle_map[101]		= {0};		// <! angle 0~180
uint16_t left_calibra_angle_map[64] 		= {0};		// <! angle 115.2~1.8
uint16_t right_calibra_angle_map[61] 		= {0};    // <! angle -1.8~108



// <! reference point angle
#define POINT_A_ARML_ANGLE		(27.526186)
#define POINT_A_ARMR_ANGLE		(111.687637)
#define POINT_A_BASE_ANGLE		(90.0)
#define POINT_B_ARML_ANGLE		(32.758930)
#define POINT_B_ARMR_ANGLE		(88.980537)
#define POINT_B_BASE_ANGLE		(90.0)
#define POINT_C_ARML_ANGLE		(24.572023)
#define POINT_C_ARMR_ANGLE		(62.595272)
#define POINT_C_BASE_ANGLE		(90.0)



static float get_point_b_angle(enum angle_channel_e channel){
	switch(channel){
		case CHANNEL_ARML:	return POINT_B_ARML_ANGLE;
		case CHANNEL_ARMR:	return POINT_B_ARMR_ANGLE;
		case CHANNEL_BASE:	return POINT_B_BASE_ANGLE;	
	}
}

static uint16_t read_angle_reg_value(enum angle_channel_e channel){
	uint16_t temp_value = 0, max = 0, min = 0, sum_value = 0, aver_value = 0;
	temp_value = ((uint16_t)iic_read_byte(channel, (0x36<<1), 0x0e))<<8 | iic_read_byte(channel, (0x36<<1), 0x0f);
	max = temp_value;
	min = temp_value;
	sum_value = temp_value;
	
	for( int i=1; i < 5; i++ ){
		temp_value = ((uint16_t)iic_read_byte(channel, (0x36<<1), 0x0e))<<8 | iic_read_byte(channel, (0x36<<1), 0x0f);
		if( temp_value > max ){ max = temp_value; }
		if( temp_value < min ){ min = temp_value; }
		sum_value += temp_value;
	}
	
	aver_value = ((sum_value - max - min)/3);
	if( aver_value > 4096 ){
		return 0;
	}
	
	return aver_value;
}

float calculate_current_angle(enum angle_channel_e channel){
	uint16_t angle_reg_value = read_angle_reg_value(channel);
	int long offset = 0;
	float angle = 0.0;
	int i = 0;
	uint16_t head_data = 0, tail_data = 0;
	int long refer_value = 0;  
	
	switch( channel ){
		case CHANNEL_ARML:;
			#if defined(UARM_2500)
				for( i=0; i < sizeof(left_calibra_angle_map)/sizeof(uint16_t)-1; i++ ){
					head_data = left_calibra_angle_map[i];
					tail_data = left_calibra_angle_map[i+1];
					if( head_data > tail_data   ){ 
						tail_data += 4095; 
					}
					if( (angle_reg_value<=tail_data) && (angle_reg_value>=head_data) ){ break; }
				}
				angle = ( ( 115.2 - i*1.8 - (angle_reg_value-head_data)/4096.0*360.0) + (115.2-(i+1)*1.8+(tail_data-angle_reg_value)/4096.0*360.0) ) / 2;
				return angle;				
			#else
				refer_value = (int long)reference.param_l;

				if( refer_value-(int long)angle_reg_value > 2048 ){
					refer_value -= 4096;
				}else if( refer_value-(int long)angle_reg_value < -2048 ){
					refer_value += 4096;
				}
				
				offset = refer_value - (int long)angle_reg_value;
			#endif
			break;
		case CHANNEL_ARMR:	
			#if defined(UARM_2500)
				for( i=0; i < sizeof(right_calibra_angle_map)/sizeof(uint16_t)-1; i++ ){
					head_data = right_calibra_angle_map[i];
					tail_data = right_calibra_angle_map[i+1];
					if( tail_data > head_data ){
						head_data += 4095;
					}
					if( (angle_reg_value<=head_data) && (angle_reg_value>=tail_data) ){ break; }
				}
				angle = ( ((head_data-angle_reg_value)/4096.0*360.0 + i*1.8 - 1.8) + ((i+1)*1.8 - 1.8 - (angle_reg_value-tail_data)/4096.0*360.0) ) / 2;
				return angle;							
			#else
				refer_value = (int long)reference.param_r;

				if( refer_value-(int long)angle_reg_value > 2048 ){
					refer_value -= 4096;
				}else if( refer_value-(int long)angle_reg_value < -2048 ){
					refer_value += 4096;
				}
				
				offset = refer_value - (int long)angle_reg_value;
			#endif	
			break;
		case CHANNEL_BASE:;		
			#if defined(UARM_2500)
				for( i=0; i < sizeof(base_calibra_angle_map)/sizeof(uint16_t)-1; i++ ){
					head_data = base_calibra_angle_map[i];
					tail_data = base_calibra_angle_map[i+1];
					if( tail_data > head_data ){
						head_data += 4095;
					}
					if( (angle_reg_value<=head_data) && (angle_reg_value>tail_data) ){ break; }
				}
				angle = ( ((head_data-angle_reg_value)/4096.0*360.0 + i*1.8) + ((i+1)*1.8 - (angle_reg_value-tail_data)/4096.0*360.0) ) / 2;
				return angle;
			#else
				refer_value = (int long)reference.param_b;
			
				if( refer_value-(int long)angle_reg_value > 2048 ){
					refer_value -= 4096;
				}else if( refer_value-(int long)angle_reg_value < -2048 ){
					refer_value += 4096;
				}
				
				offset = refer_value - (int long)angle_reg_value;
			#endif
			break;
	}
	
	
	angle = get_point_b_angle(channel) - offset * 360.0 / 4096;

//	if(angle > 360){
//		angle -= 360;
//	}
//	if(angle < 0){
//		angle += 360;
//	}

	return angle;
}


static void read_angle_reference_param(void){
	uint8_t angle_addr_flag = eeprom_get_char(EEPROM_ANGLE_REFER_FLAG);

	int8_t read_size = sizeof(struct refer_value_t);
	unsigned int read_addr = 0;
	if( angle_addr_flag == 0xBB ){
		read_addr = EEPROM_ADDR_ANGLE_REFER;	
	}else{
		read_addr = EEPROM_OLD_ADDR_ANGLE_REFER;
	}
	
	char *p = (char *)(&reference);
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}

//	DB_PRINT_STR( "refer:B%d L%d R%d\r\n", reference.param_b, reference.param_l, reference.param_r );
	
}

static void write_angle_reference_param(void){
	int8_t write_size = sizeof(struct refer_value_t);
	unsigned int write_addr = EEPROM_ADDR_ANGLE_REFER;

	char *p = (char *)(&reference);
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}
  
	eeprom_put_char( EEPROM_ANGLE_REFER_FLAG, 0xBB );
	
	read_angle_reference_param();
	uarm.init_arml_angle = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	uarm.init_armr_angle = calculate_current_angle(CHANNEL_ARMR);
	uarm.init_base_angle = calculate_current_angle(CHANNEL_BASE);	
}


void single_point_reference(void){				// <! B point 
	reference.param_l = read_angle_reg_value(CHANNEL_ARML);
	reference.param_r = read_angle_reg_value(CHANNEL_ARMR);
	reference.param_b = read_angle_reg_value(CHANNEL_BASE);

	write_angle_reference_param();
}

static void read_angle_calibra_map(void){
	char *p = (char *)(base_calibra_angle_map);																			// <! read base encoder angle map
	unsigned int read_addr = EEPROM_ADDR_ANGLE_CAL_MAP;
	int16_t read_size = sizeof(base_calibra_angle_map);	
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}	

	p = (char *)(left_calibra_angle_map);																						// <! read left encoder angle map
	read_addr = EEPROM_ADDR_ANGLE_CAL_MAP + sizeof(base_calibra_angle_map);
	read_size = sizeof(left_calibra_angle_map);
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}	

	p = (char *)(right_calibra_angle_map);																					// <! read right encoder angle map
	read_addr = EEPROM_ADDR_ANGLE_CAL_MAP + sizeof(base_calibra_angle_map) + sizeof(left_calibra_angle_map);
	read_size = sizeof(right_calibra_angle_map);
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}	
}

static void write_angle_calibra_map(void){
	char *p = (char *)(base_calibra_angle_map);
	unsigned int write_addr = EEPROM_ADDR_ANGLE_CAL_MAP;	
	int16_t write_size = sizeof(base_calibra_angle_map);
	
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}	

	p = (char *)(left_calibra_angle_map);
	write_addr = EEPROM_ADDR_ANGLE_CAL_MAP + sizeof(base_calibra_angle_map);
	write_size = sizeof(left_calibra_angle_map);
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}

	p = (char *)(right_calibra_angle_map);
	write_addr = EEPROM_ADDR_ANGLE_CAL_MAP + sizeof(base_calibra_angle_map) + sizeof(left_calibra_angle_map);
	write_size = sizeof(right_calibra_angle_map);
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}	
	
}

bool atuo_angle_calibra(void){
	uint16_t calibra_cnt = 0;
	uint16_t per_angle_cnt = 0; 

	BASE_DIRECTION_PORT &= (~BASE_DIRECTION_MASK);
	for( calibra_cnt=0; calibra_cnt < sizeof(base_calibra_angle_map)/sizeof(uint16_t)-1; calibra_cnt++ ){		// <! base motor range 0~180								
		base_calibra_angle_map[calibra_cnt] = read_angle_reg_value(CHANNEL_BASE);
		
		for( per_angle_cnt=0; per_angle_cnt < 400; per_angle_cnt++ ){
			BASE_STEP_PORT |= 	BASE_STEP_MASK;
			delay_us(10);
			BASE_STEP_PORT &= (~BASE_STEP_MASK);
			delay_us(300);
		}
		delay_ms(1000);
	}
	base_calibra_angle_map[calibra_cnt] = read_angle_reg_value(CHANNEL_BASE);

	BASE_DIRECTION_PORT |= BASE_DIRECTION_MASK;																			// <!	base back to 90
	for( per_angle_cnt=0; per_angle_cnt < 400*50; per_angle_cnt++ ){
		BASE_STEP_PORT |= 	BASE_STEP_MASK;
		delay_us(10);
		BASE_STEP_PORT &= (~BASE_STEP_MASK);
		delay_us(300);
	}


//	for( int i=0; i<sizeof(base_calibra_angle_map)/sizeof(uint16_t); i++ ){
//		DB_PRINT_STR("%d : %d\r\n ", i, base_calibra_angle_map[i]);
//		delay_ms(10);
//	}


	
	
	ARML_DIRECTION_PORT &= (~ARML_DIRECTION_MASK);							// <! arml move to back. about L115.2
	for( int i=0; i<14; i++ ){
		for( per_angle_cnt=0; per_angle_cnt < 400; per_angle_cnt++ ){
			ARML_STEP_PORT |=  ARML_STEP_MASK;
			delay_us(10);
			ARML_STEP_PORT &= (~ARML_STEP_MASK);
			delay_us(300);
		}
	}
	delay_ms(1000);
	ARML_DIRECTION_PORT |= ARML_DIRECTION_MASK;	
	for( calibra_cnt=0; calibra_cnt < sizeof(left_calibra_angle_map)/sizeof(uint16_t)-1; calibra_cnt++ ){
		left_calibra_angle_map[calibra_cnt] =  read_angle_reg_value(CHANNEL_ARML);
		for( per_angle_cnt=0; per_angle_cnt < 400; per_angle_cnt++ ){
			ARML_STEP_PORT |=  ARML_STEP_MASK;
			delay_us(10);
			ARML_STEP_PORT &= (~ARML_STEP_MASK);
			delay_us(500);
		}
		
		if( calibra_cnt == 14 ){
			ARMR_DIRECTION_PORT &= (~ARMR_DIRECTION_MASK);																// <! R to 57.6																
			for( per_angle_cnt=0; per_angle_cnt < 400*32; per_angle_cnt++ ){
				ARMR_STEP_PORT |=  ARMR_STEP_MASK;
				delay_us(10);
				ARMR_STEP_PORT &= (~ARMR_STEP_MASK);
				delay_us(300);
			}		
		}
		
		delay_ms(1000);
	}
	left_calibra_angle_map[calibra_cnt] =  read_angle_reg_value(CHANNEL_ARML);

//	for( int i=0; i<sizeof(left_calibra_angle_map)/sizeof(uint16_t); i++ ){
//		DB_PRINT_STR("%d : %d\r\n ", i, left_calibra_angle_map[i]);
//		delay_ms(10);
//	}

	ARML_DIRECTION_PORT &= (~ARML_DIRECTION_MASK);																// <! L to 55.8
	for( per_angle_cnt=0; per_angle_cnt < 400*29; per_angle_cnt++ ){
		ARML_STEP_PORT |=  ARML_STEP_MASK;
		delay_us(10);
		ARML_STEP_PORT &= (~ARML_STEP_MASK);
		delay_us(300);
	}	
	delay_ms(1000);
	ARMR_DIRECTION_PORT |= ARMR_DIRECTION_MASK;																// <! R to -1.8															
	for( per_angle_cnt=0; per_angle_cnt < 400*33; per_angle_cnt++ ){
		ARMR_STEP_PORT |=  ARMR_STEP_MASK;
		delay_us(10);
		ARMR_STEP_PORT &= (~ARMR_STEP_MASK);
		delay_us(300);
	}	
	delay_ms(1000);

	ARMR_DIRECTION_PORT &= (~ARMR_DIRECTION_MASK);
	for( calibra_cnt=0; calibra_cnt < sizeof(right_calibra_angle_map)/sizeof(uint16_t)-1; calibra_cnt++ ){
		right_calibra_angle_map[calibra_cnt] =  read_angle_reg_value(CHANNEL_ARMR);
		for( per_angle_cnt=0; per_angle_cnt < 400; per_angle_cnt++ ){
			ARMR_STEP_PORT |=  ARMR_STEP_MASK;
			delay_us(10);
			ARMR_STEP_PORT &= (~ARMR_STEP_MASK);
			delay_us(300);
		}
		delay_ms(1000);
	}
	right_calibra_angle_map[calibra_cnt] =  read_angle_reg_value(CHANNEL_ARMR);	


	write_angle_calibra_map();
	update_motor_position();
//	DB_PRINT_STR( "auto angle calibra done\r\n" );

	return true;
}

void get_refer_value(uint16_t *value){
	value[0] = reference.param_l;
	value[1] = reference.param_r;
	value[2] = reference.param_b;
}

void get_angle_reg_value(uint16_t *value){
	value[0] = read_angle_reg_value(CHANNEL_ARML);
	value[1] = read_angle_reg_value(CHANNEL_ARMR);
	value[2] = read_angle_reg_value(CHANNEL_BASE);
}

void angle_sensor_init(void){
	angle_iic_init();
	read_angle_reference_param();	// <! read reference param from eeprom 

	read_angle_calibra_map();
//	for( int i=0; i<sizeof(base_calibra_angle_map)/sizeof(uint16_t); i++ ){
//		DB_PRINT_STR("%d : %d\r\n ", i, base_calibra_angle_map[i]);
//		delay_ms(10);
//	}

//	for( int i=0; i<sizeof(left_calibra_angle_map)/sizeof(uint16_t); i++ ){
//		DB_PRINT_STR("%d : %d\r\n ", i, left_calibra_angle_map[i]);
//		delay_ms(10);
//	}

//	for( int i=0; i<sizeof(right_calibra_angle_map)/sizeof(uint16_t); i++ ){
//		DB_PRINT_STR("%d : %d\r\n ", i, right_calibra_angle_map[i]);
//		delay_ms(10);
//	}


//	uarm.init_arml_angle = calculate_current_angle(CHANNEL_ARML); 	// <! calculate init angle
//	uarm.init_armr_angle = calculate_current_angle(CHANNEL_ARMR);
//	uarm.init_base_angle = calculate_current_angle(CHANNEL_BASE);


	
}




