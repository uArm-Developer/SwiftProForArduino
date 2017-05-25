#include "X_IIC.h"

#define X_SDA_SET		PORTD |= 0x20
#define X_SDA_CLEAR		PORTD &= 0xDF
#define X_SCL_SET		PORTD |= 0x40	
#define X_SCL_CLEAR		PORTD &= 0xBF

#define X_SDA_INPUT		DDRD &= 0xDF
#define X_SDA_OUTPUT	DDRD |= 0x20

#define X_SCL_INPUT		DDRD &= 0xBF
#define X_SCL_OUTPUT	DDRD |= 0x40

#define X_SDA_READ		PIND & 0x20

#define X_PORT_DDR		DDRD


#define X_SCK_H      {X_SCL_SET;   }  
#define X_SCK_L      {X_SCL_CLEAR;   }
#define X_SDA_H      {X_SDA_OUTPUT;X_SDA_SET; }      
#define X_SDA_L      {X_SDA_OUTPUT;X_SDA_CLEAR; } 
   
#define X_READ_SDA   X_SDA_READ

#define AS5600_DEFAULT_ADDRESS 0x36




void X_IIC_Init(void)
{

X_SDA_OUTPUT;
X_SCL_OUTPUT;  
X_SDA_SET;
X_SCL_SET;
}


void X_IIC_Start(void)
{
  X_SDA_H;
  X_SCK_H;            
  delayMicroseconds(5);  
  X_SDA_L;        
  delayMicroseconds(5);  
  
  X_SCK_L;       
   delayMicroseconds(2);  

}


void X_IIC_Stop(void)
{
  X_SCK_L;             
  X_SDA_L;             
  delayMicroseconds(5);
  X_SCK_H;
  delayMicroseconds(5);       
  X_SDA_H;             
  delayMicroseconds(5);       
  

}


u8 X_IIC_Wait_Ack(void)
{
  u8 ucErrTime=0;
  X_SDA_H;delayMicroseconds(2); 
  X_SCK_H;delayMicroseconds(2);  


  X_SDA_INPUT;

  while(X_READ_SDA)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      X_IIC_Stop();
      return 1;
    }
  }
  
  X_SCK_L;             
    delayMicroseconds(2);  
  return 0;  
}


void X_IIC_Ack(void)
{
  X_SCK_L;               
  X_SDA_L;            
  delayMicroseconds(2);
  X_SCK_H;
  delayMicroseconds(5);      
  
  X_SCK_L;           
  delayMicroseconds(2);      
    X_SDA_H;  
    delayMicroseconds(2);
}


void X_IIC_Nack(void)
{
  X_SCK_L;           
  X_SDA_H;           
  delayMicroseconds(2);
  X_SCK_H;
  delayMicroseconds(5);      
  
  X_SCK_L;            
  delayMicroseconds(2);     
    X_SDA_H;  
    delayMicroseconds(2);
}


void X_IIC_SendByte(u8 Txdata)
{
  u8 i; 
  for(i=0;i<8;i++)
  { 
    X_SCK_L;           
    if(Txdata&0x80)  
      X_SDA_H          
    else
      X_SDA_L
    delayMicroseconds(5); 
    X_SCK_H;      
    delayMicroseconds(5); 
    X_SCK_L;       
    Txdata<<=1;
    delayMicroseconds(5);
  }
  
  delayMicroseconds(2);  
  X_SDA_H;  
  delayMicroseconds(2);
}

u8 X_IIC_ReadByte(u8 ack)
{
  u8 i,receive=0;
  
  delayMicroseconds(2);  
  X_SDA_H;  
  delayMicroseconds(2);
  
  for(i=0;i<8;i++)
  {
    X_SCK_L;     
    delayMicroseconds(5);
    X_SCK_H;      
    receive<<=1;
    delayMicroseconds(5);
    X_SDA_INPUT;

    if(X_READ_SDA)
      receive|=0x01;
    else
      receive&=0xfe;
    delayMicroseconds(5);
  }
  if(!ack)
    X_IIC_Nack();
  else
    X_IIC_Ack();
  
  X_SCK_L;             
  delayMicroseconds(2);
  
  return receive;
}

void X_IIC_Write_OneByte(u8 deviceaddr,u8 writeaddr,u8 writedata)
{
  X_IIC_Start();
  X_IIC_SendByte(deviceaddr&0xfe);
  X_IIC_Wait_Ack();
  X_IIC_SendByte(writeaddr);
  X_IIC_Wait_Ack();
  X_IIC_SendByte(writedata);
  X_IIC_Wait_Ack();
  X_IIC_Stop();
  delay(10);               
}



u8 X_IIC_Read_OneByte(u8 deviceaddr,u8 readaddr)
{
  u8 temp;
  X_IIC_Start();
  X_IIC_SendByte(deviceaddr&0xfe);
  X_IIC_Wait_Ack();
  X_IIC_SendByte(readaddr);
  X_IIC_Wait_Ack();

  X_IIC_Start();
  X_IIC_SendByte(deviceaddr|0x01);
  X_IIC_Wait_Ack();
  temp=X_IIC_ReadByte(0);
  X_IIC_Stop();
  return temp;
}






