#include "uarm_swift.h"

struct uarm_state_t uarm = {0};
char hardware_version[8] = {0};
char bt_mac_addr[13] = {0};

void uarm_swift_init(void){
	//DB_PRINT_STR("uarm init\r\n");
	delay_ms(10);

	read_hardware_version();	
	angle_sensor_init();

	beep_tone(260, 1000);
	read_sys_param();
	pump_off();
	gripper_relesae();
	laser_off();
	end_effector_init();
	
	angle_to_coord( uarm.init_arml_angle, uarm.init_armr_angle, uarm.init_base_angle-90,
									&(uarm.coord_x), &(uarm.coord_y), &(uarm.coord_z) );

	uarm.target_step[X_AXIS] = sys.position[X_AXIS];
	uarm.target_step[Y_AXIS] = sys.position[Y_AXIS];
	uarm.target_step[Z_AXIS] = sys.position[Z_AXIS];

	ADCSRA |= ( 1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0 | 1<<ADEN );			// <!adc set

	DDRG &= ~(1<<3);
	PORTG |= (1<<3);
	uarm.motor_state_bits = 0x0F;
	end_effector_get_origin();
	uart_printf( "device name : %s\r\n", DEVICE_NAME );
	uart_printf( "soft ver : %s\r\n" , SOFTWARE_VERSION );
	uart_printf( "api ver : %s\r\n", API_VERSION );
	uart_printf( "work mode : %d\r\n", uarm.param.work_mode );
	printString( "@1\n" );

}

void uarm_swift_tick_run(void){
//	swift_buzzer.tick();

}


