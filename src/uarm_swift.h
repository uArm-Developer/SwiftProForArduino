#ifndef _UARM_SWIFT_H_
#define _UARM_SWIFT_H_


#include "uarm_common.h"

#if defined(UARM_MINI)
	#define DEVICE_NAME						"SwiftPro mini"
#elif	defined(UARM_2500)
	#define DEVICE_NAME						"SwiftPro 2500"
#else
	#define DEVICE_NAME						"SwiftPro"
#endif

#define HARDWARE_VERSION			hardware_version
#define SOFTWARE_VERSION			"V4.4.0"
#define API_VERSION						"V4.0.3"
#define BLE_UUID							bt_mac_addr


void uarm_swift_init(void);
void uarm_swift_tick_run(void);


#endif
