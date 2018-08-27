/*
  cpu_map_atmega2560.h - CPU and pin mapping configuration file
  Part of Grbl

  Copyright (c) 2012-2015 Sungeun K. Jeon

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

/* This cpu_map file serves as a central pin mapping settings file for AVR Mega 2560 */


#ifdef GRBL_PLATFORM
#error "cpu_map already defined: GRBL_PLATFORM=" GRBL_PLATFORM
#endif


#define GRBL_PLATFORM "Atmega2560"

// Serial port pins
#define SERIAL_RX USART0_RX_vect
#define SERIAL_UDRE USART0_UDRE_vect

// Increase Buffers to make use of extra SRAM
//#define RX_BUFFER_SIZE		256
//#define TX_BUFFER_SIZE		128
//#define BLOCK_BUFFER_SIZE	36
//#define LINE_BUFFER_SIZE	100

/*
// Define step pulse output pins. NOTE: All step bit pins must be on the same port.
#define STEP_DDR      DDRA
#define STEP_PORT     PORTA
#define STEP_PIN      PINA
#define X_STEP_BIT    2 // MEGA2560 Digital Pin 24
#define Y_STEP_BIT    3 // MEGA2560 Digital Pin 25
#define Z_STEP_BIT    4 // MEGA2560 Digital Pin 26
#define STEP_MASK ((1<<X_STEP_BIT)|(1<<Y_STEP_BIT)|(1<<Z_STEP_BIT)) // All step bits

// Define step direction output pins. NOTE: All direction pins must be on the same port.
#define DIRECTION_DDR     DDRC
#define DIRECTION_PORT    PORTC
#define DIRECTION_PIN     PINC
#define X_DIRECTION_BIT   7 // MEGA2560 Digital Pin 30
#define Y_DIRECTION_BIT   6 // MEGA2560 Digital Pin 31
#define Z_DIRECTION_BIT   5 // MEGA2560 Digital Pin 32
#define DIRECTION_MASK ((1<<X_DIRECTION_BIT)|(1<<Y_DIRECTION_BIT)|(1<<Z_DIRECTION_BIT)) // All direction bits

// Define stepper driver enable/disable output pin.
#define STEPPERS_DISABLE_DDR   DDRB
#define STEPPERS_DISABLE_PORT  PORTB
#define STEPPERS_DISABLE_BIT   7 // MEGA2560 Digital Pin 13
#define STEPPERS_DISABLE_MASK (1<<STEPPERS_DISABLE_BIT)
*/

// Define homing/hard limit switch input pins and limit interrupt vectors. 
// NOTE: All limit bit pins must be on the same port
#define LIMIT_DDR       DDRD
#define LIMIT_PORT      PORTD
#define LIMIT_PIN       PIND
#define X_LIMIT_BIT     4 // MEGA2560 Digital Pin 10
#define Y_LIMIT_BIT     4 // MEGA2560 Digital Pin 11
#define Z_LIMIT_BIT     4 // MEGA2560 Digital Pin 12
#define LIMIT_INT       PCIE0  // Pin change interrupt enable pin
#define LIMIT_INT_vect  PCINT0_vect 
#define LIMIT_PCMSK     PCMSK0 // Pin change interrupt register
#define LIMIT_MASK ((1<<X_LIMIT_BIT)|(1<<Y_LIMIT_BIT)|(1<<Z_LIMIT_BIT)) // All limit bits

// Define spindle enable and spindle direction output pins.
#define SPINDLE_ENABLE_DDR      DDRD
#define SPINDLE_ENABLE_PORT     PORTD
#define SPINDLE_ENABLE_BIT      4 // MEGA2560 Digital Pin 6
#define SPINDLE_DIRECTION_DDR   DDRD
#define SPINDLE_DIRECTION_PORT  PORTD
#define SPINDLE_DIRECTION_BIT   4 // MEGA2560 Digital Pin 5

// Define flood and mist coolant enable output pins.
// NOTE: Uno analog pins 4 and 5 are reserved for an i2c interface, and may be installed at
// a later date if flash and memory space allows.
#define COOLANT_FLOOD_DDR     DDRD
#define COOLANT_FLOOD_PORT    PORTD
#define COOLANT_FLOOD_BIT     4 // MEGA2560 Digital Pin 8
#ifdef ENABLE_M7 // Mist coolant disabled by default. See config.h to enable/disable.
#define COOLANT_MIST_DDR    DDRD
#define COOLANT_MIST_PORT   PORTD
#define COOLANT_MIST_BIT    4 // MEGA2560 Digital Pin 9
#endif  

// Define user-control CONTROLs (cycle start, reset, feed hold) input pins.
// NOTE: All CONTROLs pins must be on the same port and not on a port with other input pins (limits).
#define CONTROL_DDR       DDRD
#define CONTROL_PIN       PIND
#define CONTROL_PORT      PORTD
#define RESET_BIT         4  // MEGA2560 Analog Pin 8
#define FEED_HOLD_BIT     4  // MEGA2560 Analog Pin 9
#define CYCLE_START_BIT   4  // MEGA2560 Analog Pin 10
#define SAFETY_DOOR_BIT   4  // MEGA2560 Analog Pin 11
#define CONTROL_INT       PCIE2  // Pin change interrupt enable pin
#define CONTROL_INT_vect  PCINT2_vect
#define CONTROL_PCMSK     PCMSK2 // Pin change interrupt register
#define CONTROL_MASK ((1<<RESET_BIT)|(1<<FEED_HOLD_BIT)|(1<<CYCLE_START_BIT)|(1<<SAFETY_DOOR_BIT))
#define CONTROL_INVERT_MASK CONTROL_MASK // May be re-defined to only invert certain control pins.

// Define probe switch input pin.
#define PROBE_DDR       DDRD
#define PROBE_PIN       PIND
#define PROBE_PORT      PORTD
#define PROBE_BIT       4  // MEGA2560 Analog Pin 15
#define PROBE_MASK      (1<<PROBE_BIT)

// Start of PWM & Stepper Enabled Spindle
#ifdef VARIABLE_SPINDLE
  // Advanced Configuration Below You should not need to touch these variables
  // Set Timer up to use TIMER4B which is attached to Digital Pin 7
  #define PWM_MAX_VALUE       65535.0
  #define TCCRA_REGISTER		TCCR4A
  #define TCCRB_REGISTER		TCCR4B
  #define OCR_REGISTER		OCR4B
  
  #define COMB_BIT			COM4B1
  #define WAVE0_REGISTER		WGM40
  #define WAVE1_REGISTER		WGM41
  #define WAVE2_REGISTER		WGM42
  #define WAVE3_REGISTER		WGM43
  
  #define SPINDLE_PWM_DDR		DDRA
  #define SPINDLE_PWM_PORT    PORTA
  #define SPINDLE_PWM_BIT		4 // MEGA2560 Digital Pin 97
#endif // End of VARIABLE_SPINDLE


// ARML  <--> grbl x axis
#define ARML_STEP_DDR 	DDRF   
#define ARML_STEP_PORT	PORTF
#define ARML_STEP_PIN		6		//PF6
#define ARML_STEP_MASK	(1<<ARML_STEP_PIN)
#define ARML_STEP_BIT		0

//ARMR <--> grbl y axis
#define ARMR_STEP_DDR     DDRL
#define ARMR_STEP_PORT    PORTL
#define ARMR_STEP_PIN     3		// PL3
#define ARMR_STEP_MASK		(1<<ARMR_STEP_PIN)
#define ARMR_STEP_BIT    	1

//BASE <--> grbl z axis
#define BASE_STEP_DDR      DDRF
#define BASE_STEP_PORT     PORTF
#define BASE_STEP_PIN      0	// PF0
#define BASE_STEP_MASK			(1<<BASE_STEP_PIN)
#define BASE_STEP_BIT    	2 

#define ARML_DIRECTION_DDR     DDRF
#define ARML_DIRECTION_PORT    PORTF
#define ARML_DIRECTION_PIN     7	//PF7 
#define ARML_DIRECTION_MASK 	(1<<ARML_DIRECTION_PIN)
#define ARML_DIRECTION_BIT   	0	


#define ARMR_DIRECTION_DDR     DDRL
#define ARMR_DIRECTION_PORT    PORTL
#define ARMR_DIRECTION_PIN     1	//PL1
#define ARMR_DIRECTION_MASK 	(1<<ARMR_DIRECTION_PIN)
#define ARMR_DIRECTION_BIT   	1	


#define BASE_DIRECTION_DDR     DDRF
#define BASE_DIRECTION_PORT    PORTF
#define BASE_DIRECTION_PIN     1	//PF1
#define BASE_DIRECTION_MASK 	(1<<BASE_DIRECTION_PIN)
#define BASE_DIRECTION_BIT   	2	

#define ARML_STEPPERS_DISABLE_DDR   DDRF
#define ARML_STEPPERS_DISABLE_PORT  PORTF
#define ARML_STEPPERS_DISABLE_BIT   2 //PF2
#define ARML_STEPPERS_DISABLE_MASK (1<<ARML_STEPPERS_DISABLE_BIT)

#define ARMR_STEPPERS_DISABLE_DDR   DDRK
#define ARMR_STEPPERS_DISABLE_PORT  PORTK
#define ARMR_STEPPERS_DISABLE_BIT   0 //PK0
#define ARMR_STEPPERS_DISABLE_MASK (1<<ARMR_STEPPERS_DISABLE_BIT)

#define BASE_STEPPERS_DISABLE_DDR   DDRD
#define BASE_STEPPERS_DISABLE_PORT  PORTD
#define BASE_STEPPERS_DISABLE_BIT   7 //PD7
#define BASE_STEPPERS_DISABLE_MASK (1<<BASE_STEPPERS_DISABLE_BIT)


#define MS3_DDRD	DDRK
#define MS3_PORT	PORTK
#define MS3_PIN		4
#define MS3_MASK	(1<<MS3_PIN)

#define MS2_DDRD	DDRK
#define MS2_PORT	PORTK
#define MS2_PIN		3
#define MS2_MASK	(1<<MS2_PIN)

#define MS1_DDRD	DDRK
#define MS1_PORT	PORTK
#define MS1_PIN		2
#define MS1_MASK	(1<<MS1_PIN)




