#include "uarm_angle.h"
#include "uarm_iic.h"

//uint16_t reference_param[3] = {0};

struct refer_value_t {
	uint16_t param_b;
	uint16_t param_l;
	uint16_t param_r;
} reference = {0};

uint16_t reference_param_pointa[3] = {0};
uint16_t reference_param_pointb[3] = {0};
uint16_t reference_param_pointc[3] = {0};

// <! reference point angle
/*
#define POINT_A_ARML_ANGLE		(27.683153)
#define POINT_A_ARMR_ANGLE		(110.509674)
#define POINT_A_BASE_ANGLE		(90.0)
#define POINT_B_ARML_ANGLE		(32.183559)
#define POINT_B_ARMR_ANGLE		(88.302086)
#define POINT_B_BASE_ANGLE		(90.0)
#define POINT_C_ARML_ANGLE		(24.049515)
#define POINT_C_ARMR_ANGLE		(62.108681)
#define POINT_C_BASE_ANGLE		(90.0)*/

#define POINT_A_ARML_ANGLE		(28.979406)
#define POINT_A_ARMR_ANGLE		(109.275169)
#define POINT_A_BASE_ANGLE		(90.0)
#define POINT_B_ARML_ANGLE		(33.081184)
#define POINT_B_ARMR_ANGLE		(87.177917)
#define POINT_B_BASE_ANGLE		(90.0)
#define POINT_C_ARML_ANGLE		(23.994249)
#define POINT_C_ARMR_ANGLE		(60.965763)
#define POINT_C_BASE_ANGLE		(90.0)


static float get_point_b_angle(enum angle_channel_e channel){
	switch(channel){
		case CHANNEL_ARML:	return POINT_B_ARML_ANGLE;
		case CHANNEL_ARMR:	return POINT_B_ARMR_ANGLE;
		case CHANNEL_BASE:	return POINT_B_BASE_ANGLE;	
	}
}

static uint16_t read_angle_reg_value(enum angle_channel_e channel){
	uint16_t temp_value = 0, max = 0, min = 0, sum_value = 0;
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
	
	return ((sum_value - max - min)/3);
}

float calculate_current_angle(enum angle_channel_e channel){
	uint16_t angle_reg_value = read_angle_reg_value(channel);
	int long offset = 0;
	float angle = 0.0;

	switch( channel ){
		case CHANNEL_ARML:		offset = (int long)reference.param_l - (int long)angle_reg_value;
			break;
		case CHANNEL_ARMR:		offset = (int long)reference.param_r - (int long)angle_reg_value;
			break;
		case CHANNEL_BASE:		offset = (int long)reference.param_b - (int long)angle_reg_value;
			break;
	}
	
	
	angle = get_point_b_angle(channel) - offset * 360.0 / 4096;

	if(angle > 360){
		angle -= 360;
	}
	if(angle < 0){
		angle += 360;
	}

	return angle;
}


static void read_angle_reference_param(void){
	int8_t read_size = sizeof(struct refer_value_t);
	unsigned int read_addr = EEPROM_ADDR_ANGLE_REFER;
	
	char *p = (char *)(&reference);
	for( ; read_size > 0; read_size-- ){
		*(p++) = eeprom_get_char(read_addr++);
	}

//	DB_PRINT_STR( "refer : %d, %d, %d\r\n", reference.param_l, reference.param_r, reference.param_b );
	
}

static void write_angle_reference_param(void){
	int8_t write_size = sizeof(struct refer_value_t);
	unsigned int write_addr = EEPROM_ADDR_ANGLE_REFER;

	char *p = (char *)(&reference);
	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, *(p++) );
	}
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

void multi_point_reference(uint8_t param){
	switch(param){
		case 0:
//						DB_PRINT_STR( "a\r\n" );
						reference_param_pointa[0]	= read_angle_reg_value(CHANNEL_ARML);		// <! A point
						reference_param_pointa[1] = read_angle_reg_value(CHANNEL_ARMR);
						reference_param_pointa[2] = read_angle_reg_value(CHANNEL_BASE);
			break;
		case 1:
//						DB_PRINT_STR( "b\r\n" );
						reference_param_pointb[0]	= read_angle_reg_value(CHANNEL_ARML);		// <! B point
						reference_param_pointb[1] = read_angle_reg_value(CHANNEL_ARMR);
						reference_param_pointb[2] = read_angle_reg_value(CHANNEL_BASE);
			break;
		case 2:
//						DB_PRINT_STR( "c\r\n" );
						reference_param_pointc[0]	= read_angle_reg_value(CHANNEL_ARML);		// <! C point
						reference_param_pointc[1] = read_angle_reg_value(CHANNEL_ARMR);
						reference_param_pointc[2] = read_angle_reg_value(CHANNEL_BASE);
			break;
	}
}

bool calculate_refer_write_eeprom(void){
	if( (reference_param_pointa[0] | reference_param_pointa[1] | reference_param_pointa[2]) == 0 ){
		DB_PRINT_STR( "a 0\r\n" );
		return false;
	}
	if( (reference_param_pointb[0] | reference_param_pointb[1] | reference_param_pointb[2]) == 0 ){
		DB_PRINT_STR( "b 0\r\n" );
		return false;
	}
	if( (reference_param_pointc[0] | reference_param_pointc[1] | reference_param_pointc[2]) == 0 ){
		DB_PRINT_STR( "c 0\r\n" );
		return false;
	}

	float pointa_b_arml_offset =  reference_param_pointa[0] - (POINT_A_ARML_ANGLE-POINT_B_ARML_ANGLE)*4096/360.0;
	float pointa_b_armr_offset =	reference_param_pointa[1] - (POINT_A_ARMR_ANGLE-POINT_B_ARMR_ANGLE)*4096/360.0;
	
	float pointc_b_arml_offset =  reference_param_pointc[0] - (POINT_C_ARML_ANGLE-POINT_B_ARML_ANGLE)*4096/360.0;
	float pointc_b_armr_offset =	reference_param_pointc[1] - (POINT_C_ARMR_ANGLE-POINT_B_ARMR_ANGLE)*4096/360.0;

/*	DB_PRINT_STR("value offset:");
	DB_PRINT_FLOAT( pointa_b_arml_offset );DB_PRINT_STR(" ");
	DB_PRINT_FLOAT( pointa_b_armr_offset );DB_PRINT_STR(" ");
	DB_PRINT_FLOAT( pointc_b_arml_offset );DB_PRINT_STR(" ");
	DB_PRINT_FLOAT( pointc_b_armr_offset );DB_PRINT_STR("\r\n");*/

	
	reference.param_l = (pointa_b_arml_offset + pointc_b_arml_offset + reference_param_pointb[0])/3.0 + 0.5;
	reference.param_r = (pointa_b_armr_offset + pointc_b_armr_offset + reference_param_pointb[1])/3.0 + 0.5;
	reference.param_b = reference_param_pointb[2];
/*	DB_PRINT_STR("value1 offset:");
	DB_PRINT_FLOAT( reference_param[0] );DB_PRINT_STR(" ");
	DB_PRINT_FLOAT( reference_param[1] );DB_PRINT_STR(" ");
	DB_PRINT_FLOAT( reference_param[2] );DB_PRINT_STR("\r\n");	*/

	write_angle_reference_param();
	return true;
}


void get_refer_value(uint16_t *value){
	value[0] = reference.param_l;
	value[1] = reference.param_r;
	value[2] = reference.param_b;
}

void angle_sensor_init(void){
	angle_iic_init();
	read_angle_reference_param();	// <! read reference param from eeprom 
	
	uarm.init_arml_angle = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	uarm.init_armr_angle = calculate_current_angle(CHANNEL_ARMR);
	uarm.init_base_angle = calculate_current_angle(CHANNEL_BASE);


/*
	char l_str[20], r_str[20], b_str[20];
	dtostrf( uarm.init_arml_angle, 5, 4, l_str );
	dtostrf( uarm.init_armr_angle, 5, 4, r_str );
	dtostrf( uarm.init_base_angle, 5, 4, b_str );

	DB_PRINT_STR( "init angle: %s, %s, %s\r\n", l_str, r_str, b_str );*/

/*	DB_PRINT_STR("angle abc:");
	DB_PRINT_FLOAT(uarm.init_arml_angle);DB_PRINT_STR(" ");
	DB_PRINT_FLOAT(uarm.init_armr_angle);DB_PRINT_STR(" ");
	DB_PRINT_FLOAT(uarm.init_base_angle);DB_PRINT_STR("\r\n");*/
	
}




