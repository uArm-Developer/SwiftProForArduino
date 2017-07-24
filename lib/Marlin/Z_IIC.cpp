#include "Z_IIC.h"

#define Z_SDA_SET		PORTH |= 0x04
#define Z_SDA_CLEAR		PORTH &= 0xFB
#define Z_SCL_SET		PORTH |= 0x80	
#define Z_SCL_CLEAR		PORTH &= 0x7F

#define Z_SDA_INPUT		DDRH &= 0xFB
#define Z_SDA_OUTPUT	DDRH |= 0x04

#define Z_SCL_INPUT		DDRH &= 0x7F
#define Z_SCL_OUTPUT	DDRH |= 0x80

#define Z_SDA_READ		PINH & 0x04

#define Z_PORT_DDR		DDRH

#define Z_SCK_H      {Z_SCL_SET;   }  
#define Z_SCK_L      {Z_SCL_CLEAR;   }
#define Z_SDA_H      {Z_SDA_OUTPUT;Z_SDA_SET; }      
#define Z_SDA_L      {Z_SDA_OUTPUT;Z_SDA_CLEAR; } 
   
#define Z_READ_SDA   Z_SDA_READ

#define AS5600_DEFAULT_ADDRESS 0x36


void Z_IIC_Init(void)
{
	Z_SDA_OUTPUT;
	Z_SCL_OUTPUT;  
	Z_SDA_SET;
	Z_SCL_SET;
}


void Z_IIC_Start(void)
{
  Z_SDA_H;
  Z_SCK_H;            
  delayMicroseconds(5);  
  Z_SDA_L;        
  delayMicroseconds(5);  
  
  Z_SCK_L;       
    delayMicroseconds(2);  

}


void Z_IIC_Stop(void)
{
  Z_SCK_L;             
  Z_SDA_L;             
  delayMicroseconds(5);
  Z_SCK_H;
  delayMicroseconds(5);       
  Z_SDA_H;             
  delayMicroseconds(5);       
  

}


u8 Z_IIC_Wait_Ack(void)
{
  u8 ucErrTime=0;
  Z_SDA_H;delayMicroseconds(2); 
  Z_SCK_H;delayMicroseconds(2);  

 
  Z_SDA_INPUT;

  while(Z_READ_SDA)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      Z_IIC_Stop();
      return 1;
    }
  }
  
  Z_SCK_L;             
    delayMicroseconds(2);  
  return 0;  
}


void Z_IIC_Ack(void)
{
  Z_SCK_L;               
  Z_SDA_L;            
  delayMicroseconds(2);
  Z_SCK_H;
  delayMicroseconds(5);      
  
  Z_SCK_L;           
  delayMicroseconds(2);      
    Z_SDA_H;  
    delayMicroseconds(2);
}


void Z_IIC_Nack(void)
{
  Z_SCK_L;           
  Z_SDA_H;           
  delayMicroseconds(2);
  Z_SCK_H;
  delayMicroseconds(5);      
  
  Z_SCK_L;            
  delayMicroseconds(2);     
    Z_SDA_H;  
    delayMicroseconds(2);
}


void Z_IIC_SendByte(u8 Txdata)
{
  u8 i; 
  for(i=0;i<8;i++)
  { 
    Z_SCK_L;           
    if(Txdata&0x80)  
      Z_SDA_H          
    else
      Z_SDA_L
    delayMicroseconds(5); 
    Z_SCK_H;      
    delayMicroseconds(5); 
    Z_SCK_L;       
    Txdata<<=1;
    delayMicroseconds(5);
  }
  
  delayMicroseconds(2);  
  Z_SDA_H;  
  delayMicroseconds(2);
}

u8 Z_IIC_ReadByte(u8 ack)
{
  u8 i,receive=0;
  
  delayMicroseconds(2);  
  Z_SDA_H;  
  delayMicroseconds(2);
  
  for(i=0;i<8;i++)
  {
    Z_SCK_L;     
    delayMicroseconds(5);
    Z_SCK_H;      
    receive<<=1;
    delayMicroseconds(5);
    Z_SDA_INPUT;

    if(Z_READ_SDA)
      receive|=0x01;
    else
      receive&=0xfe;
    delayMicroseconds(5);
  }
  if(!ack)
    Z_IIC_Nack();
  else
    Z_IIC_Ack();
  
  Z_SCK_L;             
  delayMicroseconds(2);
  
  return receive;
}

void Z_IIC_Write_OneByte(u8 deviceaddr,u8 writeaddr,u8 writedata)
{
  Z_IIC_Start();
  Z_IIC_SendByte(deviceaddr&0xfe);
  Z_IIC_Wait_Ack();
  Z_IIC_SendByte(writeaddr);
  Z_IIC_Wait_Ack();
  Z_IIC_SendByte(writedata);
  Z_IIC_Wait_Ack();
  Z_IIC_Stop();
  delay(10);               
}



u8 Z_IIC_Read_OneByte(u8 deviceaddr,u8 readaddr)
{
  u8 temp;
  Z_IIC_Start();
  Z_IIC_SendByte(deviceaddr&0xfe);
  Z_IIC_Wait_Ack();
  Z_IIC_SendByte(readaddr);
  Z_IIC_Wait_Ack();

  Z_IIC_Start();
  Z_IIC_SendByte(deviceaddr|0x01);
  Z_IIC_Wait_Ack();
  temp=Z_IIC_ReadByte(0);
  Z_IIC_Stop();
  return temp;
}






