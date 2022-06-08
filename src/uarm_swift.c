#include "uarm_swift.h"

struct uarm_state_t uarm = {0};
char hardware_version[8] = {0};
char bt_mac_addr[13] = {0};
enum uarm_device_e uarm_device = UARM_PRO;

void uarm_swift_init(void){
	//DB_PRINT_STR("uarm init\r\n");
	delay_ms(10);

	read_hardware_version();	
	angle_sensor_init();
	button_init();

	beep_tone(260, 1000);

	read_sys_param();
	pump_off();
	gripper_relesae();
	laser_off();
	end_effector_init();
	
	update_motor_position();

	uarm.reset_flag = false;
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

	pump_tick();
}


