#ifndef _UARM_DEBUG_H_
#define _UARM_DEBUG_H_

#include "grbl.h"

#define UART_DEBUG

#ifdef UART_DEBUG
	#define DB_PRINT_STR(format, args...) 		uart_printf(format, ##args)
	#define DB_PRINT_FLOAT(__VALUE__)					printFloat(__VALUE__,2)
	#define DB_PRINT_INT(__VALUE__)						printInteger(__VALUE__)
#else
	#define DB_PRINT_STR(format, args...) 		
	#define DB_PRINT_FLOAT(__VALUE__) 				
	#define DB_PRINT_INT(__VALUE__) 					
#endif

#endif

