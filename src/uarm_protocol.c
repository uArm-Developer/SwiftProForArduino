#include "grbl.h"
#include "uarm_protocol.h"
#include "uarm_common.h"
#include "uarm_peripheral.h"
#include "uarm_coord_convert.h"
#include "uarm_angle.h"
#include "uarm_swift.h"


struct cmd_pack_t {
	char line[CMD_BUFFER_SIZE];
	uint8_t parse_result;
	bool line_parse_done;
	char report_str[CMD_BUFFER_SIZE];		
};

struct cmd_pack_t syn_queue[PACK_MAX_SIZE] = {0};
struct cmd_pack_t asyn_pack = {0};
uint8_t syn_pack_remain = 0;
char tail_report_str[TAIL_BUFFER_SIZE] = {0};
char event_report_str[EVENT_BUFFER_SIZE] = {0};

void receive_cmd_line(char *line){		
  protocol_execute_realtime(); 												// Runtime command check point.
  if (sys.abort) { return; } 													// Bail to calling function upon system abort  

	char *p = line;
	char temp_str[10] = {0};
	if( line[0] == '$' ){																					// <! grbl sys cmd
		report_status_message(system_execute_line(line)); 
		return;
	}else if( line[0] == '#' ){																		// <! prase debug head
		sscanf( line, "#%[0-9]", temp_str );
		p = line+strlen(temp_str)+1;
	}else if( (line[0]=='G') || (line[0]=='M') || (line[0]=='P') || (line[0]=='S') ){
		
	}else{																													
		return;
	}


	if( p[0] == 'P' || p[0] == 'S' ){
//		uart_printf( "1111\r\n" );
		strcpy( asyn_pack.line, p );															// <! save asyn cmd
		
		if( strlen(temp_str) ){
			sprintf( asyn_pack.report_str, "$%s", temp_str );				// <! get serial number
		}
		
		memset( line, 0x0, CMD_BUFFER_SIZE );
	}else{
		static uint8_t syn_receive_sp = 0;
		if( syn_pack_remain >= PACK_MAX_SIZE ){
			uart_printf( "E23 buffer full\r\n" );
			return;
		}
		strcpy( syn_queue[syn_receive_sp].line, p );						// <! save syn cmd

		if( strlen(temp_str) ){
			sprintf( syn_queue[syn_receive_sp].report_str, "$%s", temp_str );
		}
		
		memset( line, 0x0, CMD_BUFFER_SIZE );
		
		if( syn_receive_sp++ >= PACK_MAX_SIZE-1 ){ syn_receive_sp = 0; }
		syn_pack_remain++;

//		DB_PRINT_STR( "rece sp: %d, cnt :%d\r\n", syn_receive_sp, syn_pack_remain );
	}
	
}


static void add_result_to_report(uint8_t result, char *str){		// <! create report string
	switch( result ){
			case STATUS_OK:	
				if( strlen(str) ){
					strcat( str, " " );
				}
				strcat( str, "ok" );
				break;
			case STATUS_EXPECTED_COMMAND_LETTER:			break;
			case STATUS_BAD_NUMBER_FORMAT:						break;
			case STATUS_INVALID_STATEMENT:						break;
			case STATUS_NEGATIVE_VALUE:								break;
			case STATUS_SETTING_DISABLED:							break;
			case STATUS_SETTING_STEP_PULSE_MIN:				break;
			case STATUS_SETTING_READ_FAIL:						break;
			case STATUS_IDLE_ERROR:										break;
			case STATUS_ALARM_LOCK:										break;
			case STATUS_SOFT_LIMIT_ERROR:							break;
			case STATUS_OVERFLOW:											break;
			case STATUS_GCODE_MODAL_GROUP_VIOLATION:	break;
			case STATUS_GCODE_UNSUPPORTED_COMMAND:	
				if( strlen(str) ){
					strcat( str, " " );
				}
				strcat( str, "E20" );
			break;
			case STATUS_GCODE_UNDEFINED_FEED_RATE:		break;
			case STATUS_UARM_ERROR:										
				if( strlen(str) ){
					strcat( str, " " );
				}
				strcat( str, "E21" );
			break;
			case UARM_COORD_ERROR:
				if( strlen(str) ){
					strcat( str, " " );
				}
				strcat( str, "E22" );
			break;	
			case UARM_ENCODER_ERR0R:
				if( strlen(str) ){
					strcat( str, " " );
				}
				strcat( str, "E26" );
			break;
			case UARM_ENABLE_ERROR:
				if( strlen(str) ){
					strcat( str, " " );
				}
				strcat( str, "E27" );
			break;				
			default:			
				if( strlen(str) ){
					strcat( str, " " );
				}
				strcat( str, "E20" );
		}

	if( strlen(tail_report_str) ){
		strcat( str, tail_report_str );
		memset( tail_report_str, 0x0, TAIL_BUFFER_SIZE );
	}else{
		strcat( str, "\n" );
	}
}

void parse_cmd_line(void){
	if( strlen(asyn_pack.line) ){																									// <! asyn cmd is not empty
		asyn_pack.parse_result = gc_execute_line(asyn_pack.line);
		add_result_to_report( asyn_pack.parse_result, asyn_pack.report_str );
	
		asyn_pack.line_parse_done = true;
		memset( asyn_pack.line, 0x0, CMD_BUFFER_SIZE );
	}

//	if( !uarm.gcode_delay_flag && syn_pack_remain && sys.state == STATE_IDLE ){		// <! delay cmd done && syn queue is not empty && sys is idle 
	if( !uarm.gcode_delay_flag && syn_pack_remain ){ 	// <! delay cmd done && syn queue is not empty

		static uint8_t syn_parse_sp = 0;
		if( uarm.effect_ldie == false || uarm.beep_ldie == false ){ 
//			DB_PRINT_STR( "no ldie, %d, %d\r\n", uarm.effect_ldie, uarm.beep_ldie );
			return; 
		}

		syn_queue[syn_parse_sp].parse_result = gc_execute_line(syn_queue[syn_parse_sp].line);
		if(sys.print_reset_flag==false)
			add_result_to_report( syn_queue[syn_parse_sp].parse_result, syn_queue[syn_parse_sp].report_str );
	
		syn_queue[syn_parse_sp].line_parse_done = true;
		memset( syn_queue[syn_parse_sp].line, 0x0, CMD_BUFFER_SIZE );	
		
		if( syn_parse_sp++ >= PACK_MAX_SIZE-1 ){ syn_parse_sp = 0; }
		syn_pack_remain--;

//		DB_PRINT_STR( "parse sp: %d, cnt :%d\r\n", syn_parse_sp, syn_pack_remain );
	}
}


void report_parse_result(void){
	if( asyn_pack.line_parse_done ){															//<! report asyn result
		printString( asyn_pack.report_str );
		memset( asyn_pack.report_str , 0x0, CMD_BUFFER_SIZE );
		asyn_pack.line_parse_done = false;
	}

	if( sys.state==STATE_IDLE && uarm.effect_ldie==true && uarm.beep_ldie==true ){			//<! report syn result
		static uint8_t syn_report_sp = 0;
		for( int i=0; i < PACK_MAX_SIZE; i++ ){
			if( syn_queue[syn_report_sp].line_parse_done == true ){
				printString( syn_queue[syn_report_sp].report_str );
				memset( syn_queue[syn_report_sp].report_str , 0x0, CMD_BUFFER_SIZE );
				syn_queue[syn_report_sp].line_parse_done = false;
				if( syn_report_sp++ >= PACK_MAX_SIZE-1 ){ syn_report_sp = 0; }
			}else{ break; }
		}		
	}

	if( uarm.cycle_report_flag ){																	//<! cycle report coord
		uarm.cycle_report_flag = false;
//		if(sys.state =STATE_CYCLE)
			cycle_report_coord();
	}

	static uint8_t limit_per_status = 0;																//<! report limit sw 											
	uint8_t limit_cur_status = get_limit_switch_status();
	if( limit_cur_status != limit_per_status ){
		delay_ms(10);
		limit_cur_status = get_limit_switch_status();
		if( limit_cur_status != limit_per_status ){
			uart_printf( "@6 N0 V%d\n", limit_cur_status );
			limit_per_status = limit_cur_status;
		}
	}

	static uint32_t cnt = 0;
	static uint8_t power_per_status = 0;																//<! report power
	if( cnt++ > 0xff ){
		cnt = 0;
		//get_power_status();
		uint8_t power_cur_status = get_power_status();
		if( power_cur_status != power_per_status ){
			uart_printf( "@5 V%d\n", power_cur_status );
			power_per_status = power_cur_status;
			uarm.power_state = power_cur_status;
		}		
	}

	if( sys.state == STATE_IDLE && uarm.run_done_report_flag ){		//<! move complete report
		if( uarm.run_flag ){
			uart_printf( "@9 V0\n" );
			uarm.run_flag = false;
		}
	}

/*	if( strlen(event_report_str) ){																//<! print event 
		printString( event_report_str );
		memset( event_report_str, 0x0, EVENT_BUFFER_SIZE );
	}*/
	
}


/*
 *
 *				CMD -> G
 *
 */
static bool uarm_cmd_g2004(char *payload){					// <! g cmd delay ms
	int delay_ms = 0;
	uint8_t rtn = 0;
	
	if( rtn = sscanf(payload, "P%d", &delay_ms ) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		gcode_cmd_delay( delay_ms );
		return true;
	}		
}

static enum uarm_protocol_e uarm_cmd_g2201(char *payload){						// <! polar coord
	uint8_t rtn = 0;
	char s_str[10], r_str[10], h_str[10], f_str[10];
	if( rtn = sscanf(payload, "S%[0-9-+.]R%[0-9-+.]H%[0-9-+.]F%[0-9-+.]", s_str, r_str, h_str, f_str) < 4 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return UARM_CMD_ERROR;		
	}else{
		float length = 0, angle = 0, high = 0, speed = 0;
		float target[3] = {0};
		if( !read_float(s_str, NULL, &length) ){ return false; }
		if( !read_float(r_str, NULL, &angle) ){ return false; }
		if( !read_float(h_str, NULL, &high) ){ return false; }
		if( !read_float(f_str, NULL, &speed) ){ return false; }

		angle = (angle - 90.0) / RAD_TO_DEG;	
		target[X_AXIS] = length * cos(angle); 
		target[Y_AXIS] = length * sin(angle);
		target[Z_AXIS] = high; 		

		return mc_line( 1, target, speed, false );
	}
}


static enum uarm_protocol_e uarm_cmd_g2202(char *payload){						// <! move motor angle
	int num;
	char angle_str[20], f_str[20];
	float angle = 0, speed = 0;
	uint8_t rtn = 0;
	if( rtn = sscanf(payload, "N%dV%[0-9-+.]F%[0-9-+.]", &num, angle_str, f_str ) < 3 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return UARM_CMD_ERROR;
	}else{
		if( !read_float(angle_str, NULL, &angle) ){ return false; }
		if( !read_float(f_str, NULL, &speed) ){ return false; }

		float angle_l = 0, angle_r = 0, angle_b = 0;
		float target[3] = {0};
		get_current_angle( uarm.target_step[X_AXIS], uarm.target_step[Y_AXIS], uarm.target_step[Z_AXIS], 
											 &angle_l, &angle_r, &angle_b );
	
		switch( num ){
			case 0:																														// <! base motor
				angle -= 90;
				angle_to_coord( angle_l, angle_r, angle, &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );				
				coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
				
				return mc_line( 0, target, speed, false );
				break;
			case 1:																														//  <! left motor
				angle_to_coord( angle, angle_r, angle_b, &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
				coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
				return mc_line( 0, target, speed, false );
				break;
			case 2:																														// <! right motor
				angle_to_coord( angle_l, angle, angle_b, &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
				coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
				return mc_line( 0, target, speed, false );				
				break;
			case 3:
				end_effector_set_angle(angle);
				return UARM_CMD_OK;
				break;
		}
		return UARM_CMD_ERROR;
	}		
}

static enum uarm_protocol_e uarm_cmd_g2204(char *payload){					// <! coord offset 
	uint8_t rtn = 0;
	char x_str[10], y_str[10], z_str[10], f_str[10];
	if( rtn = sscanf(payload, "X%[0-9-+.]Y%[0-9-+.]Z%[0-9-+.]F%[0-9-+.]", x_str, y_str, z_str, f_str ) < 4 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return UARM_CMD_ERROR;
	}else{
		float x = 0, y = 0, z = 0, speed = 0;
		float target[3];
		if( !read_float(x_str, NULL, &x) ){ return false; }
		if( !read_float(y_str, NULL, &y) ){ return false; }
		if( !read_float(z_str, NULL, &z) ){ return false; }
		if( !read_float(f_str, NULL, &speed) ){ return false; }

		step_to_coord( uarm.target_step[X_AXIS], uarm.target_step[Y_AXIS], uarm.target_step[Z_AXIS], 
									 &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
		coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
		target[X_AXIS] += x;
		target[Y_AXIS] += y;
		target[Z_AXIS] += z;

		return mc_line( 1 , target, speed, false );
	}
}

static  enum uarm_protocol_e uarm_cmd_g2205(char *payload){		// <! polar coord offset
	uint8_t rtn = 0;
	char s_str[10], r_str[10], h_str[10], f_str[10];
	if( rtn = sscanf(payload, "S%[0-9-+.]R%[0-9-+.]H%[0-9-+.]F%[0-9-+.]", s_str, r_str, h_str, f_str) < 4 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return UARM_CMD_ERROR;		
	}else{
		float length = 0, angle = 0, high = 0, speed = 0;
		float target[3] = {0};
		if( !read_float(s_str, NULL, &length) ){ return false; }
		if( !read_float(r_str, NULL, &angle) ){ return false; }
		if( !read_float(h_str, NULL, &high) ){ return false; }
		if( !read_float(f_str, NULL, &speed) ){ return false; }

		float angle_l = 0, angle_r = 0, angle_b = 0;
		get_current_angle( uarm.target_step[X_AXIS], uarm.target_step[Y_AXIS], uarm.target_step[Z_AXIS], 
											 &angle_l, &angle_r, &angle_b );
		
		step_to_coord( uarm.target_step[X_AXIS], uarm.target_step[Y_AXIS], uarm.target_step[Z_AXIS], 
									 &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
		coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );

		length += sqrt( target[X_AXIS]*target[X_AXIS] + target[Y_AXIS]*target[Y_AXIS] );

		target[X_AXIS] = length * cos((angle_b+angle) / RAD_TO_DEG);;		
		target[Y_AXIS] = length * sin((angle_b+angle) / RAD_TO_DEG);;
		target[Z_AXIS] += high;

		char l_str[20], r_str[20], b_str[20], str[20];
		dtostrf( target[X_AXIS], 5, 4, l_str );
		dtostrf( target[Y_AXIS], 5, 4, r_str );
		dtostrf( target[Z_AXIS], 5, 4, b_str );
		dtostrf( length, 5, 4, str );
			
		return mc_line( 1, target, speed, false );
	}	
}

static enum uarm_protocol_e uarm_cmd_g2206(char *payload){
	char angle_b_str[20], angle_l_str[20], angle_r_str[20] ,speed_str[20];
	float angle_b, angle_l, angle_r, speed;
	uint8_t rtn = 0;
	if( rtn = sscanf(payload, "B%[0-9-+.]L%[0-9-+.]R%[0-9-+.]F%[0-9-+.]", angle_b_str, angle_l_str, angle_r_str, speed_str ) < 4 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return UARM_CMD_ERROR;
	}else{
		if( !read_float(angle_b_str, NULL, &angle_b) ){ return false; }
		if( !read_float(angle_l_str, NULL, &angle_l) ){ return false; }
		if( !read_float(angle_r_str, NULL, &angle_r) ){ return false; }
		if( !read_float(speed_str, NULL, &speed) ){ return false; }
		angle_b -= 90;

		float target[3] = {0};
		angle_to_coord( angle_l, angle_r, angle_b, &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );				
		coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
		
		return mc_line( 0, target, speed, false );	
	}
}

static enum uarm_protocol_e uarm_cmd_g2207(char *payload){
	char speed_str[20],direction_str[20];
	float speed;
	int direction;
	float x = 0, y = 0, z = 0, angle_e=0,x_l=0,y_l=0,z_l=0,z_z=0;
	float angle_l = 0, angle_r = 0, angle_b =0,angle_ab=0,h2=0,angle_z=0,length=0,s=0;
	char l_str[20] = {0}, r_str[20] = {0}, b_str[20] = {0}, e_str[20] = {0};
	float target[3];
	float final_angle_l=0,final_angle_r=0,final_angle_b=0;
	float angle_ar=0,angle_br=0,cnt=0,cnt2=0;
	uint8_t rtn =0 ;
	if(rtn = sscanf(payload, "N%d F%[0-9-+.]",&direction,speed_str)<2)
	{
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return UARM_CMD_ERROR;
	}else{
		if( !read_float(speed_str, NULL, &speed) ){ return false; }
	}


	angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	angle_r = calculate_current_angle(CHANNEL_ARMR);
	angle_b = calculate_current_angle(CHANNEL_BASE)-90;

	angle_to_coord( angle_l, angle_r, angle_b, &x, &y, &z );

	coord_arm2effect( &x, &y, &z );

	step_to_coord( uarm.target_step[X_AXIS], uarm.target_step[Y_AXIS], uarm.target_step[Z_AXIS], 
								 &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
	coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
	if(speed<0)
	{
		direction+=3;
		speed = -speed;
	}
	switch(direction)
	{
		case 0:

/*********************************************************************************************/
			final_angle_l = angle_l;
			final_angle_r = angle_r;
			final_angle_b = angle_b;
			x_l = x;
			z_l = z;
			y_l = y;

			final_angle_l = final_angle_l /RAD_TO_DEG;
			final_angle_r = final_angle_r /RAD_TO_DEG;
			h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
			angle_ab = ARMA_ARMB_MAX_ANGLE-2;
			final_angle_l = (180-angle_ab)/2;
			final_angle_l = final_angle_l /RAD_TO_DEG;
			length = ARM_A *cos(final_angle_l)*2;
			s = sqrt(length*length-h2*h2);
			s += length_center_to_origin + uarm.param.front_offset;
			final_angle_b = asin(y/s);
			x = s*cos(final_angle_b);
			coord_effect2arm( &x, &y, &z );
			coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);

			if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
			{
				final_angle_l = angle_l;
				final_angle_r = angle_r;
				final_angle_b = angle_b;
				x = x_l;
						y = y_l;
						z = z_l;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				final_angle_r = final_angle_r /RAD_TO_DEG;
				h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
					
				final_angle_r =(ARMB_MIN_ANGLE+1)/RAD_TO_DEG;
				final_angle_l = asin((ARM_B*sin(final_angle_r)+h2)/ARM_A)*RAD_TO_DEG;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);
				s += length_center_to_origin+uarm.param.front_offset;
				final_angle_b = asin(y_l/s);
				x = s*cos(final_angle_b);
				coord_effect2arm( &x, &y, &z );
				coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
				if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
				{
					final_angle_l = angle_l;
					final_angle_r = angle_r;
					final_angle_b = angle_b;
					x = x_l;
					y = y_l;
					z = z_l;
					final_angle_l = final_angle_l /RAD_TO_DEG;
					final_angle_r = final_angle_r /RAD_TO_DEG;
					h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);					
					final_angle_l = (ARMA_MIN_ANGLE+1)/RAD_TO_DEG;
					final_angle_r = asin((ARM_A*sin(final_angle_l)-h2)/ARM_B)*RAD_TO_DEG;	
					final_angle_r = final_angle_r/RAD_TO_DEG;
					s = ARM_A * cos(final_angle_l)+ ARM_B*cos(final_angle_r);
					s += length_center_to_origin + uarm.param.front_offset; //doubt

					final_angle_b = asin(y_l/s);
					x = s*cos(final_angle_b);
					coord_effect2arm( &x, &y, &z );
					coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
					
					if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
					{
										
					}
					else
					{
					
						final_angle_b = asin(y_l/s);
						x = s*cos(final_angle_b);
						
					}
				}
				else
				{
					final_angle_b = asin(y_l/s);
					x = s*cos(final_angle_b);
				}
			}
			else
			{
				final_angle_b = asin(y_l/s);
				x = s*cos(final_angle_b);
			}
			if(x>x_l)
			{
				if(abs(x-x_l)>0.7)
				{
					
				}
				else
				{
				
					x+=20;//to report error
				}
			}
			else
			{
							x = x+50;
			}
			target[X_AXIS] =x;
			
		break;
	
		case 3:

			final_angle_l = angle_l;
			final_angle_r = angle_r;
			final_angle_b = angle_b;
			x_l = x;
			z_l = z;
			y_l = y;
			final_angle_l = final_angle_l /RAD_TO_DEG;
			final_angle_r = final_angle_r /RAD_TO_DEG;
			h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
			angle_ab = ARMA_ARMB_MIN_ANGLE+1;
			final_angle_l = (180-angle_ab)/2;
			final_angle_l = final_angle_l /RAD_TO_DEG;
			length = ARM_A *cos(final_angle_l)*2;
			s = sqrt(length*length-h2*h2);
			s += length_center_to_origin + uarm.param.front_offset;
			final_angle_b = asin(y/s);
			x = s*cos(final_angle_b);
			coord_effect2arm( &x, &y, &z );

			coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
			
			if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)            //CHANGE 
			{	
				final_angle_l = angle_l;
				final_angle_r = angle_r;
				final_angle_b = angle_b;
				x = x_l;
						y = y_l;
						z = z_l;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				final_angle_r = final_angle_r /RAD_TO_DEG;
				h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
					
				final_angle_r =(ARMB_MAX_ANGLE-1)/RAD_TO_DEG;
				final_angle_l = asin((ARM_B*sin(final_angle_r)+h2)/ARM_A)*RAD_TO_DEG;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);
				s += length_center_to_origin+uarm.param.front_offset;
				final_angle_b = asin(y/s);
				x = s*cos(final_angle_b);
				coord_effect2arm( &x, &y, &z );
				coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
				if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)			//CHANGE 
				{
					final_angle_l = angle_l;
					final_angle_r = angle_r;
					final_angle_b = angle_b;
					x = x_l;
						y = y_l;
						z = z_l;
					final_angle_l = final_angle_l /RAD_TO_DEG;
					final_angle_r = final_angle_r /RAD_TO_DEG;
					h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
					final_angle_l = (ARMA_MAX_ANGLE-1)/RAD_TO_DEG;
					final_angle_r = asin((ARM_A*sin(final_angle_l)-h2)/ARM_B)*RAD_TO_DEG;	
					final_angle_r = final_angle_r/RAD_TO_DEG;
					s = ARM_A * cos(final_angle_l)+ ARM_B*cos(final_angle_r);
					s += length_center_to_origin + uarm.param.front_offset; //doubt

					final_angle_b = asin(y/s);
					x = s*cos(final_angle_b);
					coord_effect2arm( &x, &y, &z );
					coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
					if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)	
					{
						final_angle_l = angle_l;
						final_angle_r = angle_r;
						final_angle_b = angle_b;
						x = x_l;
						y = y_l;
							z = z_l;
						final_angle_l = final_angle_l /RAD_TO_DEG;
						final_angle_r = final_angle_r /RAD_TO_DEG;
						h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);	
						s = ARM_A * cos(final_angle_l)+ ARM_B*cos(final_angle_r);
						
						s += length_center_to_origin + uarm.param.front_offset; //doubt
						final_angle_b = asin(y/s);
						final_angle_b = final_angle_b*RAD_TO_DEG;

						final_angle_l =  (ARMA_MIN_ANGLE)/RAD_TO_DEG;
						final_angle_r = asin((h2)/ARM_B)*RAD_TO_DEG;
						final_angle_r = 180+final_angle_r;

						final_angle_r = (int)(final_angle_r);
						final_angle_r = final_angle_r/RAD_TO_DEG;
						s = ARM_A * cos(final_angle_l)+ ARM_B*cos(final_angle_r);
						s += length_center_to_origin + uarm.param.front_offset; //doubt

						final_angle_b = asin(y_l/s);
						
						x = s*cos(final_angle_b)+2;
						coord_effect2arm( &x, &y, &z );
						coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
						if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)	
						{
							x=3;
						}
						else
						{
								final_angle_b = asin(y_l/s);
								x = s*cos(final_angle_b)+2;
						}
					}
					else
					{
						final_angle_b = asin(y_l/s);
						x = s*cos(final_angle_b);
					}
				}
				else
				{
				
					final_angle_b = asin(y_l/s);
					x = s*cos(final_angle_b);
				}
			}
			else
			{
				final_angle_b = asin(y_l/s);
				x = s*cos(final_angle_b);
			}
			if(x<x_l)
			{
				if(abs(x-x_l)>0.7)
				{}
				else
				{
					x-=20;//to report error
				}
			}
			else
			{
				x-=20;
			}
			target[X_AXIS] =x;


		break;



		
		case 1:
			final_angle_l = angle_l;
			final_angle_r = angle_r;
			final_angle_b = angle_b;
			x_l = x;
			z_l = z;
			y_l = y;
			final_angle_l = final_angle_l /RAD_TO_DEG;
			final_angle_r = final_angle_r /RAD_TO_DEG;
			h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
			
			angle_ab = ARMA_ARMB_MAX_ANGLE-2;
			
			final_angle_l = (180-angle_ab)/2;
			final_angle_l = final_angle_l /RAD_TO_DEG;
			length = ARM_A *cos(final_angle_l)*2;
			s = sqrt(length*length-h2*h2);
			s += length_center_to_origin+uarm.param.front_offset;
			final_angle_b = acos(x_l/s);
			y= s*sin(final_angle_b);
			coord_effect2arm( &x, &y, &z );

			coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
			
			if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)   		
			{
				//ARM_B
				final_angle_l = angle_l;
				final_angle_r = angle_r;
				final_angle_b = angle_b;
				x = x_l;
						y = y_l;
						z = z_l;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				final_angle_r = final_angle_r /RAD_TO_DEG;
				h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
					
				final_angle_r =(ARMB_MIN_ANGLE+1)/RAD_TO_DEG;
				final_angle_l = asin((ARM_B*sin(final_angle_r)+h2)/ARM_A)*RAD_TO_DEG;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);
				s += length_center_to_origin+uarm.param.front_offset;
				final_angle_b = acos(x_l/s);
				y= s*sin(final_angle_b);
				
				coord_effect2arm( &x, &y, &z );

				coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);

				dtostrf( x, 5, 4, l_str );
				dtostrf( y, 5, 4, r_str );
				dtostrf( z, 5, 4, b_str );
					
				final_angle_r = abs(final_angle_r);
				if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
				{
					
					
					final_angle_l = angle_l;
					final_angle_r = angle_r;
					final_angle_b = angle_b;
					x = x_l;
					y = y_l;
					z = z_l;
					final_angle_l = final_angle_l /RAD_TO_DEG;
					final_angle_r = final_angle_r /RAD_TO_DEG;
					h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
					final_angle_l = (26+1)/RAD_TO_DEG;
					final_angle_r = asin((ARM_A*sin(final_angle_l)-h2)/ARM_B)*RAD_TO_DEG;	
					final_angle_r = final_angle_r/RAD_TO_DEG;
					s = ARM_A * cos(final_angle_l)+ ARM_B*cos(final_angle_r);
					s += length_center_to_origin + uarm.param.front_offset; //doubt

					final_angle_b = acos(x_l/s);
					y= s*sin(final_angle_b);
					
					coord_effect2arm( &x, &y, &z );

					coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
					if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
					{
						if(y_l>0)
							y = y_l;
						else
							y= -y_l;
					}
					else
					{
							final_angle_b = acos(x_l/s);
							y= s*sin(final_angle_b);

					}

				}
				else
				{
					final_angle_b = acos(x_l/s);
					y= s*sin(final_angle_b);
					
				}
					
			}
			else
			{			
				final_angle_b = acos(x_l/s);
				y= s*sin(final_angle_b);

			}
			if(y>y_l)
			{
				if(abs(y-y_l)>0.7)
				{

				}
				else
				{
					y+=20;//report error
				}
			}
			else
			{
				y+=20;
			}
			target[Y_AXIS] =y;
	
		break;
		case 4:

/*************************************************************************************/
				final_angle_l = angle_l;
				final_angle_r = angle_r;
				final_angle_b = angle_b;
				x_l = x;
				z_l = z;
				y_l = y;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				final_angle_r = final_angle_r /RAD_TO_DEG;
				h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
				
				angle_ab = ARMA_ARMB_MAX_ANGLE-2;
				
				final_angle_l = (180-angle_ab)/2;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				length = ARM_A *cos(final_angle_l)*2;
				s = sqrt(length*length-h2*h2);
				s += length_center_to_origin+uarm.param.front_offset;
				final_angle_b = acos(x/s);
				y= s*sin(final_angle_b);
				coord_effect2arm( &x, &y, &z );

				coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
				
				if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)   		
				{
					//ARM_B
					final_angle_l = angle_l;
					final_angle_r = angle_r;
					final_angle_b = angle_b;
					x = x_l;
						y = y_l;
						z = z_l;
					final_angle_l = final_angle_l /RAD_TO_DEG;
					final_angle_r = final_angle_r /RAD_TO_DEG;
					h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
						
					final_angle_r =(ARMB_MIN_ANGLE+1)/RAD_TO_DEG;
					final_angle_l = asin((ARM_B*sin(final_angle_r)+h2)/ARM_A)*RAD_TO_DEG;
					final_angle_l = final_angle_l /RAD_TO_DEG;
					s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);
					s += length_center_to_origin+uarm.param.front_offset;
					final_angle_b = acos(x/s);
					y= s*sin(final_angle_b);
					
					coord_effect2arm( &x, &y, &z );

					coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
					if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
					{
						
						
						final_angle_l = angle_l;
						final_angle_r = angle_r;
						final_angle_b = angle_b;
						x = x_l;
						y = y_l;
						z = z_l;
						final_angle_l = final_angle_l /RAD_TO_DEG;
						final_angle_r = final_angle_r /RAD_TO_DEG;
						h2 = ARM_A *sin(final_angle_l)- ARM_B*sin(final_angle_r);
						final_angle_l = (ARMA_MIN_ANGLE+1)/RAD_TO_DEG;
						final_angle_r = asin((ARM_A*sin(final_angle_l)-h2)/ARM_B)*RAD_TO_DEG;	
						final_angle_r = final_angle_r/RAD_TO_DEG;
						s = ARM_A * cos(final_angle_l)+ ARM_B*cos(final_angle_r);
						s += length_center_to_origin + uarm.param.front_offset; //doubt

						final_angle_b = acos(x/s);
						y= s*sin(final_angle_b);
						
						coord_effect2arm( &x, &y, &z );

						coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
						if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
						{
								if(y_l<0)
										y = y_l;
									else
										y= -y_l;
						}
						else
						{
								final_angle_b = acos(x_l/s);
								y= s*sin(final_angle_b);

						}

					}
					else
					{
						final_angle_b = acos(x_l/s);
						y= s*sin(final_angle_b);
						
					}
						
				}
				else
				{
						final_angle_b = acos(x_l/s);
						y= s*sin(final_angle_b);

				}
				y= -y;
				if(y<y_l)
				{
					if(abs(y-y_l)>0.7)
					{

					}
					else
					{
						y-=20;//report error
					}
				}
				else
				{
					y-=20;
				}
				target[Y_AXIS] =y;
		
			break;


				
		break;
		case 2:

				final_angle_l = angle_l;
				final_angle_r = angle_r;
				final_angle_b = angle_b;
				x_l = x;
				z_l = z;
				y_l = y;
				z_z = z;
				final_angle_l = final_angle_l /RAD_TO_DEG;
				final_angle_r = final_angle_r /RAD_TO_DEG;
				h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
				s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);
				final_angle_l = (ARMA_MAX_ANGLE-1)/RAD_TO_DEG;
				final_angle_r = (s-ARM_A*cos(final_angle_l))/ARM_B;
				final_angle_r = acos(final_angle_r);
				h2 -= ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
				z -=h2;
				
				coord_effect2arm( &x, &y, &z );
				coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
				if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
				{
					final_angle_l = angle_l;
					final_angle_r = angle_r;
					final_angle_b = angle_b;
					x = x_l;
					y = y_l;
					z = z_l;
					final_angle_l = final_angle_l /RAD_TO_DEG;
					final_angle_r = final_angle_r /RAD_TO_DEG;
					h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
					s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);
					final_angle_r = (ARMB_MIN_ANGLE+1)/RAD_TO_DEG;
					final_angle_l = acos((s-ARM_B*cos(final_angle_r))/ARM_A);
					h2 -= ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);	
					z -=h2;
					
					coord_effect2arm( &x, &y, &z );
					coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
					if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
					{
						final_angle_l = angle_l;
						final_angle_r = angle_r;
						final_angle_b = angle_b;
						x = x_l;
						y = y_l;
						z = z_l;

						final_angle_l = final_angle_l /RAD_TO_DEG;
						final_angle_r = final_angle_r /RAD_TO_DEG;
						h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
						s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);	
						angle_ab = ARMA_ARMB_MAX_ANGLE-1;
						angle_z = ((180-angle_ab)/2)/RAD_TO_DEG;
						length = cos(angle_z)*ARM_A +cos(angle_z)*ARM_B;
						h2 -= sqrt(abs(length*length-s*s));
						z -=h2;
						coord_effect2arm( &x, &y, &z );
						coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
						if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
						{

									


						}
						else
						{
							z_l -=h2;	

						}
							
					}
					else
					{
						z_l -=h2;		
					}
					
				}
				else
				{
					z_l -=h2;			
				}
				if(z_l>z_z)
				{
					if(abs(z_z-z_l)>0.7)
					{
					}
					else
					{
						z_l +=20;
					}
				}
				else
				{

					z_l +=20;
				}
				target[Z_AXIS] = z_l;
				
		break;
		case 5:

		final_angle_l = angle_l;
		final_angle_r = angle_r;
		final_angle_b = angle_b;
		x_l = x;
		z_l = z;
		y_l = y;
		z_z = z;
		final_angle_l = final_angle_l /RAD_TO_DEG;
		final_angle_r = final_angle_r /RAD_TO_DEG;	
		s = ARM_A *cos(final_angle_l)+ ARM_B*cos(final_angle_r);
		h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);	

		final_angle_r = (ARMB_MAX_ANGLE-2)/RAD_TO_DEG;
		final_angle_l = acos((s-ARM_B*cos(final_angle_r))/ARM_A);
		h2 -= ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
		z -= h2;
		coord_effect2arm( &x, &y, &z );
		coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
		if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
		{
			final_angle_l = angle_l;
			final_angle_r = angle_r;
			final_angle_b = angle_b;
			x = x_l;
			y = y_l;
			z = z_l;

			final_angle_l = final_angle_l /RAD_TO_DEG;
			final_angle_r = final_angle_r /RAD_TO_DEG;
			h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
			s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);	

			final_angle_l = (ARMA_MIN_ANGLE+1)/RAD_TO_DEG;
			final_angle_r = acos((s-ARM_A*cos(final_angle_l))/ARM_B);
			h2 -= ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);	
			z -=h2;
			coord_effect2arm( &x, &y, &z );
			coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
			if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
			{
				final_angle_l = angle_l;
				final_angle_r = angle_r;
				final_angle_b = angle_b;
				x = x_l;
				y = y_l;
				z = z_l;

				final_angle_l = final_angle_l /RAD_TO_DEG;
				final_angle_r = final_angle_r /RAD_TO_DEG;
				h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
				s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);	

				angle_ab = ARMA_ARMB_MIN_ANGLE+1;
				angle_z = ((180-angle_ab)/2)/RAD_TO_DEG;
				length = cos(angle_z)*ARM_A +cos(angle_z)*ARM_B;
				h2 -= sqrt(abs(length*length-s*s));
				z -=h2;
				coord_effect2arm( &x, &y, &z );
				coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
				
				if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)
				{

					if((180-angle_l-angle_r)>(ARMA_ARMB_MAX_ANGLE-2))
					{
						final_angle_l = angle_l;
						final_angle_r = angle_r;
						final_angle_b = angle_b;
						x = x_l;
						y = y_l;
						z = z_l;
						
						final_angle_l = final_angle_l /RAD_TO_DEG;
						final_angle_r = final_angle_r /RAD_TO_DEG;
						h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
						s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);	

						h2 -= ARM_A*sin(final_angle_r)-ARM_B*sin(final_angle_l);
						z -=h2;
						coord_effect2arm( &x, &y, &z );
						coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);
						
						if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false)
						{
						}
						else
						{
									if(h2>0)
										z_l -=h2;
									else
										z_l = z_l;
						}
					}
					else
					{
						final_angle_l = angle_l;
						final_angle_r = angle_r;
						final_angle_b = angle_b;
						x = x_l;
						y = y_l;
						z = z_l;

						final_angle_l = final_angle_l /RAD_TO_DEG;
						final_angle_r = final_angle_r /RAD_TO_DEG;
						h2 = ARM_A*sin(final_angle_l)-ARM_B*sin(final_angle_r);
						s = ARM_A*cos(final_angle_l)+ARM_B*cos(final_angle_r);	
						angle_ab = ARMA_ARMB_MAX_ANGLE-1;
						angle_z = ((180-angle_ab)/2)/RAD_TO_DEG;
						length = cos(angle_z)*ARM_A +cos(angle_z)*ARM_B;
						h2 -= sqrt(abs(length*length-s*s));
						z -=h2;
						coord_effect2arm( &x, &y, &z );
						coord_to_angle(x,y,z,&final_angle_l,&final_angle_r,&final_angle_b);

						final_angle_l = final_angle_l /RAD_TO_DEG;
						final_angle_r = final_angle_r /RAD_TO_DEG;

						h2 += sqrt(abs(length*length-s*s));
						h2 -= ARM_A*sin(final_angle_r)-ARM_B*sin(final_angle_l);
						z_l -=h2;
						if(is_angle_legal(final_angle_l,final_angle_r,final_angle_b)==false) 
							{

						}
						else
						{
							z_l -=h2;	
						}
					}
				}
				else
				{
					z_l -=h2;	   
				}
			}
			else
			{
				z_l -=h2;	  
			}	
		}
		else
		{
			z_l -=h2;	   
		}
		if(z_l<z_z)
		{
			if(abs(z_z-z_l)>0.7)
			{
			}
			else
			{
				z_l -=20;
			}
		}
		else
		{
			z_l -=20;
		}
		target[Z_AXIS] = z_l;	
		break;
	}
	return mc_line( 1, target, speed, false );
}

static enum uarm_protocol_e uarm_cmd_g2208(char *payload){
	char speed_str[20],direction_str[20];
	float angle_AB;
	float speed;
	int direction;
	uint8_t rtn;
	
	float final_angle_l=0,final_angle_r=0,final_angle_b=0;
	if(rtn = sscanf(payload, "N%d F%[0-9-+.]",&direction,speed_str)<2)
	{
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return UARM_CMD_ERROR;
	}else{
		if( !read_float(speed_str, NULL, &speed) ){ return false; }
		float angle_l = 0, angle_r = 0, angle_b = 0;
		char l_str[20] = {0}, r_str[20] = {0};

		final_angle_l=angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
		final_angle_r=angle_r = calculate_current_angle(CHANNEL_ARMR);
		final_angle_b=angle_b = calculate_current_angle(CHANNEL_BASE)-90;

		angle_AB = 180-(angle_l+angle_r);
		if(speed<0)
		{
			direction+=3;
			speed = -speed;
		}
		if(speed ==0)
		{

		}
		else
		{

			switch(direction)
			{
				case 0: final_angle_b = 90; 
					if(abs(final_angle_b-angle_b)<0.7)
						final_angle_b =200;
					angle_b = final_angle_b;
				break;
				case 3: final_angle_b = -90;
					if(abs(final_angle_b-angle_b)<0.7)
						final_angle_b =-200;
				angle_b = final_angle_b;
				break;

				case 1:	
					angle_AB = ARMA_ARMB_MIN_ANGLE;
					final_angle_l = 180-angle_AB-angle_r;
					if(final_angle_l>(ARMA_MAX_ANGLE-1))
						final_angle_l = ARMA_MAX_ANGLE-1;					
				final_angle_l = (int)final_angle_l;
				if(abs(final_angle_l-angle_l)<0.7)
					final_angle_l =200;
				angle_l = final_angle_l;
				break;


				
				case 4: 

					angle_AB =ARMA_ARMB_MAX_ANGLE;
					final_angle_l = 180-angle_AB-angle_r+0.4;	
					if(final_angle_l<(ARMB_MIN_ANGLE+1))
						final_angle_l = ARMB_MIN_ANGLE+1;
					if(abs(final_angle_l-angle_l)<0.7)
						final_angle_l =200;
					angle_l = final_angle_l;

				break;

				case 2: 

					angle_AB =ARMA_ARMB_MIN_ANGLE;
					final_angle_r =180-angle_AB-angle_l;
					if(final_angle_r >(ARMB_MAX_ANGLE-1))
						final_angle_r = ARMB_MAX_ANGLE-1;
					final_angle_r = (int)final_angle_r;

					if(abs(final_angle_r-angle_r)<0.7)
						final_angle_r =200;
					angle_r = final_angle_r;

				break;
				case 5: 

					angle_AB =ARMA_ARMB_MAX_ANGLE;
					final_angle_r = 180-angle_AB-angle_l+0.4;
					if(final_angle_r <(ARMB_MIN_ANGLE+1))
						final_angle_r = ARMB_MIN_ANGLE+1;
					if(abs(final_angle_r-angle_r)<0.7)
						final_angle_r =200;
					angle_r = final_angle_r;					
				break;
				
			}
		}
		
		float target[3] = {0};
		angle_to_coord( angle_l, angle_r, angle_b, &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );				
		coord_arm2effect( &target[X_AXIS], &target[Y_AXIS], &target[Z_AXIS] );
		
		return mc_line( 0, target, speed, false );		
	
	}
}


static enum uarm_protocol_e uarm_cmd_g2209(char *payload){
	uint8_t rtn;
	float speed;
	int direction;
	char speed_str[20],direction_str[20];
	float final_step[3];
	float final_angle_l=0,final_angle_r=0,final_angle_b=0;
	float angle_l = 0, angle_r = 0, angle_b = 0;
	int  offeset = 2;
	
	float x = 0, y = 0, z = 0, angle_e;
	if(rtn = sscanf(payload, "F%[0-9-+.]",speed_str)<1)
	{
			DB_PRINT_STR( "sscanf %d\r\n", rtn );
			return UARM_CMD_ERROR;
	}else
	{
	if( !read_float(speed_str, NULL, &speed) ){ return false; }

			final_angle_l=angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
			final_angle_r=angle_r = calculate_current_angle(CHANNEL_ARMR);
			final_angle_b=angle_b = calculate_current_angle(CHANNEL_BASE);		

			if(angle_b>(BASE_MAX_ANGLE+90))
			{
			
				final_angle_b = BASE_MAX_ANGLE+90-offeset;
				final_step[Z_AXIS] = (final_angle_b-angle_b)*80/(settings.steps_per_mm[Z_AXIS]);
			}
			else if(angle_b <(BASE_MIN_ANGLE+90))
			{
				final_angle_b = BASE_MIN_ANGLE+90+offeset;
				final_step[Z_AXIS] = (final_angle_b-angle_b)*80/(settings.steps_per_mm[Z_AXIS]);
			}
			else 
			{
				final_step[Z_AXIS] = uarm.target_step[Z_AXIS] ;
				final_step[Z_AXIS] = (final_step[Z_AXIS]) / (settings.steps_per_mm[Z_AXIS]);
			}
			if(angle_l>ARMA_MAX_ANGLE)
			{
				final_angle_l = ARMA_MAX_ANGLE-offeset;
				final_step[X_AXIS] =  (final_angle_l-angle_l)*80/(settings.steps_per_mm[X_AXIS]);
			
			}
			else if (angle_l<ARMA_MIN_ANGLE)
			{
				final_angle_l = ARMA_MIN_ANGLE+offeset;
				final_step[X_AXIS] =  (final_angle_l-angle_l)*80/(settings.steps_per_mm[X_AXIS]);

			}
			else 
			{
				final_step[X_AXIS] = uarm.target_step[X_AXIS] ;
				final_step[X_AXIS] = (final_step[X_AXIS]) / (settings.steps_per_mm[X_AXIS]);
			}

			
			if(angle_r>ARMB_MAX_ANGLE)
			{
				final_angle_r = ARMB_MAX_ANGLE-offeset;
				final_step[Y_AXIS] =  (final_angle_r-angle_r)*80/(settings.steps_per_mm[Y_AXIS]);
			
			}
			else if (angle_r<ARMB_MIN_ANGLE)
			{
				final_angle_r = ARMB_MIN_ANGLE+offeset;
				final_step[Y_AXIS] =  (final_angle_r-angle_r)*80/(settings.steps_per_mm[Y_AXIS]);

			}
			else 
			{
				final_step[Y_AXIS] = uarm.target_step[Y_AXIS] ;
				final_step[Y_AXIS] = (final_step[Y_AXIS]) / (settings.steps_per_mm[Y_AXIS]);
			}


			final_angle_b =final_angle_b- 90;
			angle_to_coord(final_angle_l,final_angle_r,final_angle_b,&x,&y,&z);
			coord_arm2effect(&x,&y,&z);
			uarm.coord_x = x;
			uarm.coord_y = y;
			uarm.coord_z = z;
			coord_to_step(x,y,z,&uarm.target_step[X_AXIS],&uarm.target_step[Y_AXIS],&uarm.target_step[Z_AXIS]);
				uarm.init_arml_angle = final_angle_l;
			uarm.init_armr_angle = final_angle_r;
			uarm.init_base_angle = final_angle_b+90;
				plan_buffer_line(final_step, speed, false);
			
				
				uarm.run_flag = true;
				uarm.restart_flag = true;
				update_motor_position();
			
				uarm.target_step[X_AXIS] = sys.position[X_AXIS];
				uarm.target_step[Y_AXIS] = sys.position[Y_AXIS];
				uarm.target_step[Z_AXIS] = sys.position[Z_AXIS];
				

				return UARM_CMD_OK;
//				
//				return mc_line( 0, target, speed, false );		
	}



}




enum uarm_protocol_e uarm_execute_g_cmd(uint16_t cmd, char *line, uint8_t *char_counter){
	switch(cmd){
		case 2004:
								if( uarm_cmd_g2004(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 
			break;
		case 2201:
								return uarm_cmd_g2201(line);
			break;
		case 2202:
								return uarm_cmd_g2202(line);
			break;
		case 2204:	
								return uarm_cmd_g2204(line);
			break;
		case 2205:
								return uarm_cmd_g2205(line);
			break;
		case 2206:
								return uarm_cmd_g2206(line);
			break;
		case 2207:
								return uarm_cmd_g2207(line);
			break;
		case 2208:
								return uarm_cmd_g2208(line);
			break;
		case 2209:
								return uarm_cmd_g2209(line);
			break;
	}

	return UARM_CMD_NOTFIND;
}



/*
 *
 *				CMD -> S
 *
 */
static void uarm_cmd_s1000(uint8_t param){
	switch( param ){
		case 0:
			bit_true_atomic(sys_rt_exec_state, EXEC_FEED_HOLD);
			break;
		case 1:
			bit_true_atomic(sys_rt_exec_state, EXEC_CYCLE_START);
			break;
			
		default:	break;
	}
}

static void uarm_cmd_s1100(void){
	mc_reset();
}



enum uarm_protocol_e uarm_execute_s_cmd(uint16_t cmd, char *line, uint8_t *char_counter){
	switch(cmd){
		case 1000:
			if( (line[0]=='V') && (line[1]>='0'&&line[1]<='1') ){
				uarm_cmd_s1000( line[1]-'0' );
				return UARM_CMD_OK;
			}else{ return UARM_CMD_ERROR; }

			break;
		case 1100:
			uarm_cmd_s1100();
			return UARM_CMD_OK;
			break;
	}

	return UARM_CMD_NOTFIND;
}



/*
 *
 *				CMD -> M
 *
 */
 
static void uarm_cmd_m17(void){				// <! lock all motor 
	
	if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
		ARML_STEPPERS_DISABLE_PORT |= ARML_STEPPERS_DISABLE_MASK;
		ARMR_STEPPERS_DISABLE_PORT |= ARMR_STEPPERS_DISABLE_MASK;
		BASE_STEPPERS_DISABLE_PORT |= BASE_STEPPERS_DISABLE_MASK;
	}else{
		ARML_STEPPERS_DISABLE_PORT &= (~ARML_STEPPERS_DISABLE_MASK);
		ARMR_STEPPERS_DISABLE_PORT &= (~ARMR_STEPPERS_DISABLE_MASK);
		BASE_STEPPERS_DISABLE_PORT &= (~BASE_STEPPERS_DISABLE_MASK);
	}
	
	end_effector_init();
	uarm.motor_state_bits = 0x0F;

	update_motor_position();
}

static bool uarm_cmd_m204(char *payload){
	char accele_str[20];
	uint8_t rtn;
	if( rtn = sscanf(payload, "A%[0-9-+.]", accele_str) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;		
	}else{
		float accele = 0;
		if( !read_float(accele_str, NULL, &accele) ){ return false; }
		settings_store_global_setting( 120, accele );
		settings_store_global_setting( 121, accele );
		settings_store_global_setting( 122, accele );
		settings_init();
		return true;
	}
}

static void uarm_cmd_m2019(void){				// <! unlock all motor

	if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
		ARML_STEPPERS_DISABLE_PORT &= (~ARML_STEPPERS_DISABLE_MASK);
		ARMR_STEPPERS_DISABLE_PORT &= (~ARMR_STEPPERS_DISABLE_MASK);
		BASE_STEPPERS_DISABLE_PORT &= (~BASE_STEPPERS_DISABLE_MASK);
	}else{
		ARML_STEPPERS_DISABLE_PORT |= ARML_STEPPERS_DISABLE_MASK;
		ARMR_STEPPERS_DISABLE_PORT |= ARMR_STEPPERS_DISABLE_MASK;
		BASE_STEPPERS_DISABLE_PORT |= BASE_STEPPERS_DISABLE_MASK;
	}

	end_effector_deinit();
	uarm.motor_state_bits = 0x00;
}

static bool uarm_cmd_m2120(char *payload){
	char time_str[20];
	float cycle_time;
	uint8_t rtn = 0;
	if( rtn = sscanf(payload, "V%s", time_str ) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( !read_float(time_str, NULL, &cycle_time) ){ return false; }
		cycle_report_start( cycle_time * 1000 );		
		return true;
	}		
}

static void uarm_cmd_m2122(uint8_t param){
	uarm.run_flag = false;
	switch( param ){
		case 0:
						uarm.run_done_report_flag = false;
			break;
		case 1:
						uarm.run_done_report_flag = true;
			break;
	}
}

static void uarm_cmd_m2123(uint8_t param){
	
	switch( param ){
		case 0:
						uarm.motor_position_check = false;
			break;
		case 1:
						uarm.motor_position_check = true;
						update_motor_position();
			break;
	}
}

static void uarm_cmd_m2124(uint8_t param){

		switch( param ){
		case 0:
					uarm.beep_state = 0;
			break;
		case 1:
					uarm.beep_state = 1;
			break;
	}
}
static void uarm_cmd_m2201(uint8_t param){		// <! lock n motor
	switch(param){
		case 0:
			if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
				ARML_STEPPERS_DISABLE_PORT |= ARML_STEPPERS_DISABLE_MASK;
				ARMR_STEPPERS_DISABLE_PORT |= ARMR_STEPPERS_DISABLE_MASK;
				BASE_STEPPERS_DISABLE_PORT |= BASE_STEPPERS_DISABLE_MASK;
			}else{
				ARML_STEPPERS_DISABLE_PORT &= (~ARML_STEPPERS_DISABLE_MASK);
				ARMR_STEPPERS_DISABLE_PORT &= (~ARMR_STEPPERS_DISABLE_MASK);
				BASE_STEPPERS_DISABLE_PORT &= (~BASE_STEPPERS_DISABLE_MASK);
			}
			uarm.motor_state_bits |= 0x04;
			break;
		case 1:
			if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
				ARML_STEPPERS_DISABLE_PORT |= ARML_STEPPERS_DISABLE_MASK;
				ARMR_STEPPERS_DISABLE_PORT |= ARMR_STEPPERS_DISABLE_MASK;
				BASE_STEPPERS_DISABLE_PORT |= BASE_STEPPERS_DISABLE_MASK;
			}else{
				ARML_STEPPERS_DISABLE_PORT &= (~ARML_STEPPERS_DISABLE_MASK);
				ARMR_STEPPERS_DISABLE_PORT &= (~ARMR_STEPPERS_DISABLE_MASK);
				BASE_STEPPERS_DISABLE_PORT &= (~BASE_STEPPERS_DISABLE_MASK);
			}
			uarm.motor_state_bits |= 0x01;
			break;
		case 2:
			if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
				ARML_STEPPERS_DISABLE_PORT |= ARML_STEPPERS_DISABLE_MASK;
				ARMR_STEPPERS_DISABLE_PORT |= ARMR_STEPPERS_DISABLE_MASK;
				BASE_STEPPERS_DISABLE_PORT |= BASE_STEPPERS_DISABLE_MASK;
			}else{
				ARML_STEPPERS_DISABLE_PORT &= (~ARML_STEPPERS_DISABLE_MASK);
				ARMR_STEPPERS_DISABLE_PORT &= (~ARMR_STEPPERS_DISABLE_MASK);
				BASE_STEPPERS_DISABLE_PORT &= (~BASE_STEPPERS_DISABLE_MASK);
			}
			uarm.motor_state_bits |= 0x02;
			break;
		case 3:
			end_effector_init();
			uarm.motor_state_bits |= 0x08;
			break;
	}
	update_motor_position();
}

static void uarm_cmd_m2202(uint8_t param){		// <! unlock n motor
	switch(param){
		case 0:
			if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
				BASE_STEPPERS_DISABLE_PORT &= (~BASE_STEPPERS_DISABLE_MASK);
			}else{
				BASE_STEPPERS_DISABLE_PORT |= BASE_STEPPERS_DISABLE_MASK;
			}
			uarm.motor_state_bits &= (~0x04);
			break;
		case 1:
			if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
				ARML_STEPPERS_DISABLE_PORT &= (~ARML_STEPPERS_DISABLE_MASK);
			}else{
				ARML_STEPPERS_DISABLE_PORT |= ARML_STEPPERS_DISABLE_MASK;
			}
			uarm.motor_state_bits &= (~0x01);
			break;
		case 2:
			if (bit_istrue(settings.flags,BITFLAG_INVERT_ST_ENABLE)){
				ARMR_STEPPERS_DISABLE_PORT &= (~ARMR_STEPPERS_DISABLE_MASK);
			}else{
				ARMR_STEPPERS_DISABLE_PORT |= ARMR_STEPPERS_DISABLE_MASK;
			}
			uarm.motor_state_bits &= (~0x02);
			break;
		case 3:
			end_effector_deinit();
			uarm.motor_state_bits &= (~0x08);
			break;
	}
}

static void uarm_cmd_m2203(uint8_t param){	// <! get motor status
	switch(param){
		case 0:
			if( (uarm.motor_state_bits&0x01) == 0x01 ){
				sprintf( tail_report_str, " V1\n" );
			}else{
				sprintf( tail_report_str, " V0\n" );	
			}
			break;
		case 1:
			if( (uarm.motor_state_bits&0x02) == 0x02 ){
				sprintf( tail_report_str, " V1\n" );
			}else{
				sprintf( tail_report_str, " V0\n" );	
			}
			break;
		case 2:
			if( (uarm.motor_state_bits&0x04) == 0x04 ){
				sprintf( tail_report_str, " V1\n" );
			}else{
				sprintf( tail_report_str, " V0\n" );	
			}
			break;
		case 3:
			if( (uarm.motor_state_bits&0x08) == 0x08 ){
				sprintf( tail_report_str, " V1\n" );
			}else{
				sprintf( tail_report_str, " V0\n" );	
			}			
			break;
	}
}

static bool uarm_cmd_m2205(char *payload){
	int rtn;
	char sn_num[13];
	if( rtn = sscanf(payload, "VUB%[0-F.]", sn_num ) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		memcpy( bt_mac_addr, sn_num, 12 );
		write_sn_num();
		return true;
	}
}

static bool uarm_cmd_m2210(char *payload){
	int duration = 0; 
	float	frequency = 0;
	char fre_str[10] = {0};
	int rtn;
	if( rtn = sscanf(payload, "F%[0-9.]T%d", fre_str, &duration) < 2 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( !read_float(fre_str, NULL, &frequency) ){ return false; }
		beep_tone(duration, frequency);
		return true;
	}
}

static bool uarm_cmd_m2211(char *payload){
	unsigned char device,type; 
	unsigned int addr;
	int rtn;
	char x_str[20] = {0};
	float data=0;
	if(rtn = sscanf(payload,"N%dA%dT%d",&device,&addr,&type)<3){
		DB_PRINT_STR("sscanf %d\r\n",rtn);
		return false;
	}else{
		if(device != 1) return false;
		if(addr >65524) return false;
		switch(type)
		{
			case 1: 
			
				data = getE2PROMData(device,addr,type);
				dtostrf(data, 5, 0, x_str );
//				uart_printf("data:%s\r\n",x_str);
				sprintf( tail_report_str, " V%d\n",(char)data );

			break;
			
			case 2:
			
				data = getE2PROMData(device,addr,type);
				dtostrf(data, 5, 0, x_str );
//				uart_printf("data:%s\r\n",x_str);
				sprintf( tail_report_str, " V%d\n",(int)data );
			break;
			
			case 4:
				data = getE2PROMData(device,addr,type);
				dtostrf(data, 5, 4, x_str );
//				uart_printf("data:%s\r\n",x_str);
				sprintf( tail_report_str, " V%s\n",x_str );
			break;
			
			default:
				return false;
			break;
		}
		return true;
	}
	
	

}

static bool uarm_cmd_m2212(char *payload){
	unsigned char device,type; 
	unsigned int addr;
	char value[20];
	float eeprom_value;
	int rtn;
	if(rtn = sscanf(payload,"N%dA%dT%dV%[0-9.]",&device,&addr,&type,value)<4){
		DB_PRINT_STR("sscanf %d\r\n",rtn);
		return false;
	}else{
		if(device != 1) return false;
		if(addr >65524) return false;
		if( !read_float(value, NULL, &eeprom_value) ){ return false; }
		switch(type)
		{
			case 1: 
				setE2PROMData(device,addr,type,eeprom_value);
			break;
			
			case 2:
				setE2PROMData(device,addr,type,eeprom_value);
			break;
			
			case 4:
			
			setE2PROMData(device,addr,type,eeprom_value);
			break;
			default:
				return false;
			break;
		}
		delay_ms(3);
		//uart_printf("N:%d A:%d T%d V:%s",device,addr,type,value);
		return true;
	}
	

	
}

static void uarm_cmd_m2215(void){				
/*	int16_t write_size = 4096;
	unsigned int write_addr = 0;

	for( ; write_size > 0; write_size-- ){
		eeprom_put_char( write_addr++, 0x0 );
	}*/
	settings_restore(SETTINGS_RESTORE_ALL);
	read_hardware_version();
}

static bool uarm_cmd_m2220(char *payload){     // <! coord to angle
	char x_str[20] = {0}, y_str[20] = {0}, z_str[20] = {0};
	float x = 0, y = 0, z = 0;
	int rtn = 0;
	
	if( rtn = sscanf(payload, "X%[0-9-+.]Y%[0-9-+.]Z%[0-9-+.]", x_str, y_str, z_str ) < 3 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( !read_float(x_str, NULL, &x) ){ return false; }
		if( !read_float(y_str, NULL, &y) ){ return false; }
		if( !read_float(z_str, NULL, &z) ){ return false; }

		float angle_b = 0, angle_l = 0, angle_r = 0;
		char b_str[20] = {0}, l_str[20] = {0}, r_str[20] = {0};
		coord_effect2arm( &x, &y, &z );
		coord_to_angle( x, y, z, &angle_l, &angle_r, &angle_b );
		angle_b += 90;

		dtostrf( angle_l, 5, 4, l_str );
		dtostrf( angle_r, 5, 4, r_str );
		dtostrf( angle_b, 5, 4, b_str );

		sprintf( tail_report_str, " B%s L%s R%s\n", b_str, l_str, r_str );
		return true;
	}
}

static bool uarm_cmd_m2221(char *payload){    // <! angle to coord
	char b_str[20] = {0}, l_str[20] = {0}, r_str[20] = {0};
	float angle_b = 0, angle_l = 0, angle_r = 0;
	int rtn = 0;
	
	if( rtn = sscanf(payload, "B%[0-9-+.]L%[0-9-+.]R%[0-9-+.]", b_str, l_str, r_str ) < 3 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( !read_float(b_str, NULL, &angle_b) ){ return false; }
		if( !read_float(l_str, NULL, &angle_l) ){ return false; }
		if( !read_float(r_str, NULL, &angle_r) ){ return false; }

		float x = 0, y = 0, z = 0;
		char x_str[20] = {0}, y_str[20] = {0}, z_str[20] = {0};
		angle_b -= 90;
		angle_to_coord( angle_l, angle_r, angle_b, &x, &y, &z );
		coord_arm2effect( &x, &y, &z );
		
		dtostrf( x, 5, 4, x_str );
		dtostrf( y, 5, 4, y_str );
		dtostrf( z, 5, 4, z_str );
		
		sprintf( tail_report_str, " X%s Y%s Z%s\n", x_str, y_str, z_str );
		return true;
	}
}


static bool uarm_cmd_m2222(char *payload){	// <! check coord if legal
	float x=0, y=0, z=0;
	char x_str[20] = {0}, y_str[20] = {0}, z_str[20] = {0};
	int mode = 0xFF;
	uint8_t rtn = 0;
	if( rtn = sscanf(payload, "X%[0-9-+.]Y%[0-9-+.]Z%[0-9-+.]P%d", x_str, y_str, z_str, &mode) < 4 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		float anglea, angleb, anglec;
		if( !read_float(x_str, NULL, &x) ){ return false; }
		if( !read_float(y_str, NULL, &y) ){ return false; }
		if( !read_float(z_str, NULL, &z) ){ return false; }

		switch(mode){
			case 1:;
									float length = x, angle = y, high = z;
									angle = (angle - 90.0) / RAD_TO_DEG;	
									x = length * cos(angle); 
									y = length * sin(angle);
									z = high; 				
			case 0:
									coord_effect2arm( &x, &y, &z );
									coord_to_angle( x, y, z, &anglea, &angleb, &anglec );	
									if( is_angle_legal(anglea, angleb, anglec) == true ){
										sprintf( tail_report_str, " V1\n" );
									}else{
										sprintf( tail_report_str, " V0\n" );
									}							
									return true;
				break;
		}
		return false;
	}

}

static void uarm_cmd_m2231(uint8_t param){		// <! control pump
	switch(param){
		case 0:	pump_off();	break;
		case 1: pump_on();	break;
		case 2: pump_suction();break;

	}
}

static void uarm_cmd_m2232(uint8_t param){	// <! control gripper
	switch(param){
		case 0:		gripper_relesae();	break;
		case 1:		gripper_catch();		break;
	}
}

static void uarm_cmd_m2233(uint8_t param){
	switch(param){
		case 0:	laser_off();	break;
		case 1:	laser_on();		break;
	}	
}

static bool uarm_cmd_m2240(char *payload){
	uint8_t rtn = 0;
	int pin, value;
	if( rtn = sscanf(payload, "N%dV%d", &pin, &value) < 2 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( pin<0 || pin>69 || value<0 || value>1 ){ return false; }
		if( value ){
			digitalWrite(pin, HIGH);
		}else{
			digitalWrite(pin, LOW);
		}
		
	}
	return true;
}

static bool uarm_cmd_m2241(char *payload){
	uint8_t rtn = 0;
	int pin, value;
	if( rtn = sscanf(payload, "N%dV%d", &pin, &value) < 2 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( pin<0 || pin>69 || value<0 || value>1 ){ return false; }
		if( value ){
			pinMode(pin, OUTPUT);
		}else{
			pinMode(pin, INPUT);
		}
		
	}
	return true;	
}

static void uarm_cmd_m2400(uint8_t param){	// <! set work mode
	char h_str[20],s_str[20];
	switch(param){
		case WORK_MODE_NORMAL:				// <! nomal mode
						end_effector_deinit();
						uarm.param.work_mode = WORK_MODE_NORMAL;
						uarm.param.high_offset 	= DEFAULT_NORMAL_HEIGHT;
						uarm.param.front_offset = DEFAULT_NORMAL_FRONT;
			break;
		case WORK_MODE_LASER:				// <! laser mode 
						end_effector_deinit();		
						uarm.param.work_mode = WORK_MODE_LASER;
						uarm.param.high_offset 	= DEFAULT_LASER_HEIGHT;
						uarm.param.front_offset = DEFAULT_LASER_FRONT;			
			break;
		case WORK_MODE_PRINTER:				// <! 3D printer mode
						end_effector_deinit();		
						uarm.param.work_mode = WORK_MODE_PRINTER;
						uarm.param.high_offset 	= DEFAULT_3DPRINT_HEIGHT;
						uarm.param.front_offset = DEFAULT_3DPRINT_FRONT;				
			break;
		case WORK_MODE_PEN:				// <! pen mode
						end_effector_deinit();	
						uarm.param.work_mode = WORK_MODE_PEN;
						uarm.param.high_offset 	= DEFAULT_PEN_HEIGHT;
						uarm.param.front_offset = DEFAULT_PEN_FRONT;							
			break;
		case WORK_MODE_STEPER_FLAT:				// <! end effect steper mode
						end_effector_deinit();
						uarm.param.work_mode 		= WORK_MODE_STEPER_FLAT;
						uarm.param.high_offset 	= DEFAULT_STEP_FLAT_HEIGHT;
						uarm.param.front_offset	= DEFAULT_STEP_FLAT_FRONT;
						end_effector_get_origin();
		break;
		case WORK_MODE_STEPER_STANDARD:
						end_effector_deinit();
						uarm.param.work_mode 		= WORK_MODE_STEPER_STANDARD;
						uarm.param.high_offset  = DEFAULT_STEP_STANDARD_HEIGHT;
						uarm.param.front_offset = DEFAULT_STEP_STANDARD_FRONT;
						end_effector_get_origin();
			break;
		case WORK_MODE_TOUCH_PEN:				// <! touch pen mode
						end_effector_deinit();	
						uarm.param.work_mode = WORK_MODE_TOUCH_PEN;
						uarm.param.high_offset 	= DEFAULT_ROUND_PEN_HEIGHT;
						uarm.param.front_offset = DEFAULT_ROUND_PEN_FRONT;		
		break;	
		case WORK_MODE_USER:
						end_effector_deinit();
						uarm.param.work_mode = WORK_MODE_USER;
						read_user_endoffest();
		
		break;
		case WORK_MODE_ClAM_JAW:
					end_effector_deinit();
					uarm.param.work_mode = WORK_MODE_ClAM_JAW;
					uarm.param.high_offset 	= DEFAULT_CLAW_JAW_HEIGHT;
					uarm.param.front_offset = DEFAULT_CLAW_JAW_FRONT;
					
		break;

		case WORK_MODE_STEERING_GEAR:
					end_effector_deinit();
					uarm.param.work_mode = WORK_MODE_STEERING_GEAR;
					uarm.param.high_offset 	= DEFAULT_STEERING_GEAR_HEIGHT;
					uarm.param.front_offset = DEFAULT_STEEPING_GEAR_FRONT;
					
		break;
		case WORK_MODE_CLAMP:
					end_effector_deinit();
					uarm.param.work_mode = 	WORK_MODE_CLAMP;
					uarm.param.high_offset	= DEFAULT_CLAMP_HEIGHT;
					uarm.param.front_offset = DEFAULT_CLAMP_FRONT;
				
		break;
		case 11: 
						end_effector_deinit();	
						uarm.param.work_mode = WORK_MODE_TEST;
						uarm.param.high_offset 	= DEFAULT_TEST_HEIGHT;
						uarm.param.front_offset = DEFAULT_TEST_FRONT;				
		break;

	}

	save_sys_param();
	
	
}

static void uarm_cmd_m2401(void){		 // <! single reference 
	single_point_reference();
}

static void uarm_cmd_m2410(void){
	float x, y, z;
//	step_to_coord( sys.position[X_AXIS], sys.position[Y_AXIS], sys.position[Z_AXIS], &x, &y, &z); // calculate the current coord  
	float angle_l = 0, angle_r = 0, angle_b =0;
	char z_str[20] = {0};

	angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	angle_r = calculate_current_angle(CHANNEL_ARMR);
	angle_b = calculate_current_angle(CHANNEL_BASE)-90;
	//angle_e = end_effector_get_angle();

	angle_to_coord( angle_l, angle_r, angle_b, &x, &y, &z );
	uarm.param.high_offset = z;
											 
//	dtostrf( z, 5, 4, z_str );
//	
//
//	sprintf( tail_report_str, " H%s\n", z_str );
	save_sys_param();
	save_user_endoffest();
}

static bool uarm_cmd_m2411(char *payload){
	float front_offset = 0;
	char offset_str[20] = {0};
	
	uint8_t rtn = 0;
	if( rtn = sscanf(payload, "S%[0-9-+.]", offset_str) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( !read_float(offset_str, NULL, &front_offset) ){ return false; }
		uarm.param.front_offset = front_offset;
		save_sys_param();
		
		save_user_endoffest();
		return true;
	}	
}

static bool uarm_cmd_m2412(char *payload){
	float effect_angle_offset = 0;
	char offset_str[20] = {0};
	
	uint8_t rtn = 0;
	if( rtn = sscanf(payload, "V%[0-9-+.]", offset_str) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( !read_float(offset_str, NULL, &effect_angle_offset) ){ return false; }
		uarm.param.effect_angle_offset = effect_angle_offset;
		save_sys_param();
		end_effector_get_origin();
		return true;
	}	
}

static bool uarm_cmd_m2413(char *payload){
	float heigth_offset = 0;
	char offset_str[20] = {0};
	
	uint8_t rtn = 0;
	if( rtn = sscanf(payload, "H%[0-9-+.]", offset_str) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( !read_float(offset_str, NULL, &heigth_offset) ){ return false; }
		uarm.param.high_offset = heigth_offset;
		save_sys_param();
		
		save_user_endoffest();
		return true;
	}	
}

static void uarm_cmd_m2420(uint8_t param){
//	multi_point_reference(param);
}

static bool uarm_cmd_m2421(void){
//	return calculate_refer_write_eeprom();
}

static bool uarm_cmd_m2500(void){
	return atuo_angle_calibra();
}

enum uarm_protocol_e uarm_execute_m_cmd(uint16_t cmd, char *line, uint8_t *char_counter){

	switch(cmd){
		case 17:
								uarm_cmd_m17();				// <! lock all motor
								return UARM_CMD_OK;
			break;
		case 18:
								uarm_cmd_m2019();		// <! unlock all motor
								return UARM_CMD_OK;
			break;
		case 204:
								if( uarm_cmd_m204(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 				
			break;
		case 2019:
								uarm_cmd_m2019();		// <! unlock all motor
								return UARM_CMD_OK;
			break;
		case 2120:	
								if( uarm_cmd_m2120(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 
			break;
		case 2121:
								cycle_report_stop();
								return UARM_CMD_OK;
			break;
		case 2122:
								if( (line[0]=='V') && (line[1]>='0'&&line[1]<='1') ){
									uarm_cmd_m2122( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }

			break;
		case 2123:
								if( (line[0]=='V') && (line[1]>='0'&&line[1]<='1') ){
									uarm_cmd_m2123( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }				
			break;

		case 2124:
								if( (line[0]=='V') && (line[1]>='0'&&line[1]<='1') ){
									uarm_cmd_m2124( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }				
			break;
		case 2201:
								if( (line[0]=='N') && (line[1]>='0'&&line[1]<='3') ){
									uarm_cmd_m2201( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }
			break;
		case 2202:
								if( (line[0]=='N') && (line[1]>='0'&&line[1]<='3') ){
									uarm_cmd_m2202( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }
			break;
		case 2203:
								if( (line[0]=='N') && (line[1]>='0'&&line[1]<='3') ){
									uarm_cmd_m2203( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }
			break;
		case 2205:
								if( uarm_cmd_m2205(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 		
			break;
		case 2210:
								if( uarm_cmd_m2210(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 
			break;
		case 2211:
			//DB_PRINT_STR( "M2211\r\n" );
								if(uarm_cmd_m2211(line) == true)
								{
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;	
								}
			break;
		case 2212:
			//DB_PRINT_STR( "M2212\r\n" );
								if(uarm_cmd_m2212(line) == true)
								{
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;	
								}
			break;	
		case 2213:
			//DB_PRINT_STR( "M2213\r\n" );
								return UARM_CMD_OK;
			break;
		case 2215:
								uarm_cmd_m2215();
								return UARM_CMD_OK;
		case 2220:
								if( uarm_cmd_m2220(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								}		
			break;
		case 2221:
								if( uarm_cmd_m2221(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 	
			break;
		case 2222:
								if( uarm_cmd_m2222(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								}		
			break;	
		case 2231:
								if( (line[0]=='V') && (line[1]>='0'&&line[1]<='2') ){
									uarm_cmd_m2231( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }

			break;
		case 2232:
								//DB_PRINT_STR( "M2232\r\n" );
								if( (line[0]=='V') && (line[1]>='0'&&line[1]<='1') ){
									uarm_cmd_m2232( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }								
			break;
		case 2233:
								if( (line[0]=='V') && (line[1]>='0'&&line[1]<='1') ){
									uarm_cmd_m2233( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }	
			break;
		case 2234:
			//DB_PRINT_STR( "M2234\r\n" );
								return UARM_CMD_OK;
			break;
		case 2240:
							if( uarm_cmd_m2240(line) == true ){
								return UARM_CMD_OK;
							}else{
								return UARM_CMD_ERROR;
							}	
			break;
		case 2241:
							if( uarm_cmd_m2241(line) == true ){
								return UARM_CMD_OK;
							}else{
								return UARM_CMD_ERROR;
							}				
								
			break;
		case 2245:
			//DB_PRINT_STR( "M2245\r\n" );
			break;
		case 2304:
			//DB_PRINT_STR( "M2304\r\n" );
			break;
		case 2305:
			//DB_PRINT_STR( "M2305\r\n" );
			break;
		case 2306:
			//DB_PRINT_STR( "M2306\r\n" );
			break;
		case 2307:
			//DB_PRINT_STR( "M2307\r\n" );
			break;	
		case 2400:
								if( (line[0]=='S') && (line[1]>='0'&&line[1]<='11') ){
									uarm_cmd_m2400( line[1]-'0' );
									return UARM_CMD_OK;
								}else{ return UARM_CMD_ERROR; }	
			break;
		case 2401:
								uarm_cmd_m2401();
								return UARM_CMD_OK;
			break;
		case 2410:
								uarm_cmd_m2410();
								return UARM_CMD_OK;

			break;
		case 2411:
								if( uarm_cmd_m2411(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 
			break;

		case 2412:
								if( uarm_cmd_m2412(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 			
			break;
		case 2413:
								if( uarm_cmd_m2413(line) == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								} 
			break;
								
		case 2500:
								if( uarm_cmd_m2500() == true ){
									return UARM_CMD_OK;
								}else{
									return UARM_CMD_ERROR;
								}							
			break;
	}
	return UARM_CMD_NOTFIND;
}




/*
 *
 *				CMD -> P
 *
 */

static void uarm_cmd_p2200(void){				// <! get current angle  
	float angle_l = 0, angle_r = 0, angle_b = 0;
	char l_str[20] = {0}, r_str[20] = {0}, b_str[20] = {0};

	angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	angle_r = calculate_current_angle(CHANNEL_ARMR);
	angle_b = calculate_current_angle(CHANNEL_BASE);
										 
	dtostrf( angle_l, 5, 4, l_str );
	dtostrf( angle_r, 5, 4, r_str );
	dtostrf( angle_b, 5, 4, b_str );

	sprintf( tail_report_str, " B%s L%s R%s\n", b_str, l_str, r_str );
}

static void uarm_cmd_p2201(void){
	sprintf( tail_report_str, " %s\n", DEVICE_NAME );
}

static void uarm_cmd_p2202(void){
	sprintf( tail_report_str, " %s\n", HARDWARE_VERSION );
}

static void uarm_cmd_p2203(void){
	sprintf( tail_report_str, " %s\n", SOFTWARE_VERSION );
}

static void uarm_cmd_p2204(void){
	sprintf( tail_report_str, " %s\n", API_VERSION );
}

static void uarm_cmd_p2205(void){
	sprintf( tail_report_str, " V%s\n", BLE_UUID );
}


void uarm_cmd_p2206(uint8_t param){
	float angle_l = 0, angle_r = 0, angle_b = 0, angle_e = 0;
	char l_str[20] = {0}, r_str[20] = {0}, b_str[20] = {0} , e_str[20] = {0};

	switch( param ){
		case 0:			
								angle_b = calculate_current_angle(CHANNEL_BASE);
								dtostrf( angle_b, 5, 4, b_str );
								sprintf( tail_report_str, " V%s\n", b_str);
			break;
		case 1:			
								angle_l = calculate_current_angle(CHANNEL_ARML);
								dtostrf( angle_l, 5, 4, l_str );
								sprintf( tail_report_str, " V%s\n", l_str);
			break;
		case 2:			
								angle_r = calculate_current_angle(CHANNEL_ARMR);
								dtostrf( angle_r, 5, 4, r_str );
								sprintf( tail_report_str, " V%s\n", r_str);
			break;
		case 3:			
								angle_e = end_effector_get_angle();
								dtostrf( angle_e, 5, 4, e_str );
								sprintf( tail_report_str, " V%s\n", e_str);				
			break;
	}
}

static void uarm_cmd_p2220(void){					// <! get curren coord 
	float x = 0, y = 0, z = 0, angle_e;
	float angle_l = 0, angle_r = 0, angle_b =0;
	char l_str[20] = {0}, r_str[20] = {0}, b_str[20] = {0}, e_str[20] = {0};

	angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	angle_r = calculate_current_angle(CHANNEL_ARMR);
	angle_b = calculate_current_angle(CHANNEL_BASE)-90;
	//angle_e = end_effector_get_angle();

	angle_to_coord( angle_l, angle_r, angle_b, &x, &y, &z );

	coord_arm2effect( &x, &y, &z );
	dtostrf( x, 5, 4, l_str );
	dtostrf( y, 5, 4, r_str );
	dtostrf( z, 5, 4, b_str );
	//dtostrf( angle_e, 5, 4, e_str );
	sprintf( tail_report_str, " X%s Y%s Z%s\n", l_str, r_str, b_str );	
}

static void uarm_cmd_p2221(void){					// <! get curren polar coord 
	float x = 0, y = 0, z = 0, angle_e;
	float angle_l = 0, angle_r = 0, angle_b =0;
	char s_str[20] = {0}, r_str[20] = {0}, h_str[20] = {0}, e_str[20] = {0};

	angle_l = calculate_current_angle(CHANNEL_ARML);		// <! calculate init angle
	angle_r = calculate_current_angle(CHANNEL_ARMR);
	angle_b = calculate_current_angle(CHANNEL_BASE)-90;

	angle_to_coord( angle_l, angle_r, angle_b, &x, &y, &z );
	coord_arm2effect( &x, &y, &z );

	angle_b += 90;
	float length = sqrt( x*x + y*y );
	
	dtostrf( length, 5, 4, s_str );
	dtostrf( angle_b, 5, 4, r_str );
	dtostrf( z, 5, 4, h_str );

	sprintf( tail_report_str, " S%s R%s H%s\n", s_str, r_str, h_str );	
}


static void uarm_cmd_p2231(void){
	sprintf( tail_report_str, " V%d\n", get_pump_status() );	
}

static void uarm_cmd_p2232(void){
	sprintf( tail_report_str, " V%d\n", get_gripper_status() );	
}

static void uarm_cmd_p2233(void){
	sprintf( tail_report_str, " V%d\n", get_limit_switch_status() );	
}

static void uarm_cmd_p2234(void){
	sprintf( tail_report_str, " V%d\n", get_power_status() );	
}

static void uarm_cmd_p2235(void){
	sprintf( tail_report_str, " V%d\n", get_laser_status() );	
}

static bool uarm_cmd_p2236(char *payload){
	uint8_t rtn=0;
	int button;
	if(rtn = sscanf(payload,"N%d",&button)<1){
		DB_PRINT_STR("sscanf %d\r\n",rtn);
		return false;
	}else{
		if(button <0 || button>1) {return false;}
		int value = get_button_status(button);
		sprintf(tail_report_str, " V%d\n",value);
		
	}
	return true;
}


static bool uarm_cmd_p2240(char *payload){
	uint8_t rtn = 0;
	int pin;
	if( rtn = sscanf(payload, "N%d", &pin) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( pin<0 || pin>69 ){ return false; }
		int value = digitalRead(pin);
		sprintf( tail_report_str, " V%d\n", value );
	}
	return true;	
}

static bool uarm_cmd_p2241(char *payload){
	uint8_t rtn = 0;
	int pin;
	if( rtn = sscanf(payload, "N%d", &pin) < 1 ){
		DB_PRINT_STR( "sscanf %d\r\n", rtn );
		return false;
	}else{
		if( pin<0 || pin>15 ){ return false; }
		int value = analogRead(pin);
		sprintf( tail_report_str, " V%d\n", value );
	}
	return true;	

}


static void uarm_cmd_p2242(void){
	uint16_t refer_value[3] = {0};
	get_refer_value(&refer_value);
	sprintf( tail_report_str, " B%d L%d R%d\n", refer_value[2], refer_value[0], refer_value[1] );
}

static void uarm_cmd_p2243(void){
	uint16_t angle_reg_value[3] = {0};
	get_angle_reg_value(&angle_reg_value);
	sprintf( tail_report_str, " B%d L%d R%d\n", angle_reg_value[2], angle_reg_value[0], angle_reg_value[1] );
}
bool uarm_cmd_p2244(void)
{
	char encoder_status=7;
	encoder_status = check_encoder(CHANNEL_ARML)<<2|check_encoder(CHANNEL_ARMR)<<1|check_encoder(CHANNEL_BASE);
	switch(encoder_status)
	{
	case 0:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return true;
	break;
	case 1:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return false;
	break;
	case 2:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return false;
	break;
	case 3:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return false;
	break;
	case 4:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return false;
	break;
	case 5:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return false;
	break;
	case 6:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return false;
	break;
	case 7:
		sprintf( tail_report_str, " V%d\n", encoder_status);
		return false;
	break;

	}
}

static void uarm_cmd_p2400(void){
	sprintf( tail_report_str, " V%d\n", uarm.param.work_mode );	
}


enum uarm_protocol_e uarm_execute_p_cmd(uint16_t cmd, char *line, uint8_t *char_counter){

	switch(cmd){
		case 2200:
							uarm_cmd_p2200();
							return UARM_CMD_OK;
			break;
		case 2201:
							uarm_cmd_p2201();
							return UARM_CMD_OK;
			break;
		case 2202:
							uarm_cmd_p2202();
							return UARM_CMD_OK;
			break;
		case 2203:
							uarm_cmd_p2203();
							return UARM_CMD_OK;
			break;
		case 2204:
							uarm_cmd_p2204();
							return UARM_CMD_OK;
			break;
		case 2205:
							uarm_cmd_p2205();
							return UARM_CMD_OK;
			break;
		case 2206:
							if( (line[0]=='N') && (line[1]>='0'&&line[1]<='3') ){
								uarm_cmd_p2206(line[1]-'0');
								return UARM_CMD_OK;
							}else{ return UARM_CMD_ERROR; }							
			break;
		case 2220:
							uarm_cmd_p2220();
							return UARM_CMD_OK;
			break;
		case 2221:
							uarm_cmd_p2221();
							return UARM_CMD_OK;
			break;
		case 2231:
							uarm_cmd_p2231();
							return UARM_CMD_OK;
			break;
		case 2232:
							uarm_cmd_p2232();
							return UARM_CMD_OK;
			break;
		case 2233:
							uarm_cmd_p2233();
							return UARM_CMD_OK;
			break;
		case 2234:
							uarm_cmd_p2234();
							return UARM_CMD_OK;
			break;
		case 2235:
							uarm_cmd_p2235();
							return UARM_CMD_OK;			
			break;
		case 2236:			
							if( uarm_cmd_p2236(line) == true ){
								return UARM_CMD_OK;
							}else{
								return UARM_CMD_ERROR;
							} 
			break;					
		case 2240:
							if( uarm_cmd_p2240(line) == true ){
								return UARM_CMD_OK;
							}else{
								return UARM_CMD_ERROR;
							} 
			break;
		case 2241:
							if( uarm_cmd_p2241(line) == true ){
								return UARM_CMD_OK;
							}else{
								return UARM_CMD_ERROR;
							} 
			break;
		case 2242:
							uarm_cmd_p2242();
							return UARM_CMD_OK;
			break;
		case 2243:
							uarm_cmd_p2243();
							return UARM_CMD_OK;
			break;
		case 2244:
							if(uarm_cmd_p2244()==true){
								return UARM_CMD_OK;
							}else{
								return UARM_ENCODER_ERR0R;
							}
			break;
		case 2400:
							uarm_cmd_p2400();
							return UARM_CMD_OK;
			break;	
	}

	return UARM_CMD_NOTFIND;
}






