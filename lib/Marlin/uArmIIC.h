/**
  ******************************************************************************
  * @file	uArmIIC.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2016-10-18
  ******************************************************************************
  */

#ifndef _UARMIIC_H_
#define _UARMIIC_H_

#include <Arduino.h>




// scl pb6
// sda pb7

#define SDA_SET		PORTB |= 0x80
#define SDA_CLEAR	PORTB &= 0x7F
#define SCL_SET		PORTB |= 0x40	
#define SCL_CLEAR	PORTB &= 0xBF

#define SDA_INPUT	DDRB &= 0x7F
#define SDA_OUTPUT	DDRB |= 0x80

#define SCL_INPUT	DDRB &= 0xBF
#define SCL_OUTPUT	DDRB |= 0x40

#define SDA_READ	PINB & 0x80

#define PORT_DDR	DDRB





void delay_us();

void iic_start();
void iic_stop();

//return 0:ACK=0
//return 1:NACK=1
unsigned char read_ack();

//ack=0:send ack
//ack=1:do not send ack
void send_ack();

void iic_sendbyte(unsigned char dat);

unsigned char iic_receivebyte();

unsigned char iic_writebuf(unsigned char *buf,unsigned char device_addr,unsigned int addr,unsigned char len);

unsigned char iic_readbuf(unsigned char *buf,unsigned char device_addr,unsigned int addr,unsigned char len);





#endif // _UARMIIC_H_
