
#include "Y_IIC.h"

#define Y_SCK_PIN 34
#define Y_SDA_PIN 36
#define Y_SCK_H      {digitalWrite(Y_SCK_PIN,1);   }  
#define Y_SCK_L      {digitalWrite(Y_SCK_PIN,0);   }
#define Y_SDA_H      {pinMode(Y_SDA_PIN,OUTPUT);digitalWrite(Y_SDA_PIN,1); }      
#define Y_SDA_L      {pinMode(Y_SDA_PIN,OUTPUT);digitalWrite(Y_SDA_PIN,0); } 
   
#define Y_READ_SDA   digitalRead(Y_SDA_PIN)

#define AS5600_DEFAULT_ADDRESS 0x36






void Y_IIC_Init(void)
{
  pinMode(Y_SCK_PIN, OUTPUT);
  pinMode(Y_SDA_PIN, OUTPUT);//INPUT,
  digitalWrite(Y_SCK_PIN,1);
  digitalWrite(Y_SDA_PIN,1);//digitalRead()      
}


void Y_IIC_Start(void)
{
  Y_SDA_H;
  Y_SCK_H;            
  delayMicroseconds(5);  
  Y_SDA_L;        
  delayMicroseconds(5);  
  
  Y_SCK_L;       
    delayMicroseconds(2);  

}


void Y_IIC_Stop(void)
{
  Y_SCK_L;             
  Y_SDA_L;             
  delayMicroseconds(5);
  Y_SCK_H;
  delayMicroseconds(5);       
  Y_SDA_H;             
  delayMicroseconds(5);       
  

}


u8 Y_IIC_Wait_Ack(void)
{
  u8 ucErrTime=0;
  Y_SDA_H;delayMicroseconds(2); 
  Y_SCK_H;delayMicroseconds(2);  

  pinMode(Y_SDA_PIN, INPUT); 
  while(Y_READ_SDA)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      Y_IIC_Stop();
      return 1;
    }
  }
  
  Y_SCK_L;             
    delayMicroseconds(2);  
  return 0;  
}


void Y_IIC_Ack(void)
{
  Y_SCK_L;               
  Y_SDA_L;            
  delayMicroseconds(2);
  Y_SCK_H;
  delayMicroseconds(5);      
  
  Y_SCK_L;           
  delayMicroseconds(2);      
    Y_SDA_H;  
    delayMicroseconds(2);
}


void Y_IIC_Nack(void)
{
  Y_SCK_L;           
  Y_SDA_H;           
  delayMicroseconds(2);
  Y_SCK_H;
  delayMicroseconds(5);      
  
  Y_SCK_L;            
  delayMicroseconds(2);     
    Y_SDA_H;  
    delayMicroseconds(2);
}


void Y_IIC_SendByte(u8 Txdata)
{
  u8 i; 
  for(i=0;i<8;i++)
  { 
    Y_SCK_L;           
    if(Txdata&0x80)  
      Y_SDA_H          
    else
      Y_SDA_L
    delayMicroseconds(5); 
    Y_SCK_H;      
    delayMicroseconds(5); 
    Y_SCK_L;       
    Txdata<<=1;
    delayMicroseconds(5);
  }
  
  delayMicroseconds(2);  
  Y_SDA_H;  
  delayMicroseconds(2);
}

u8 Y_IIC_ReadByte(u8 ack)
{
  u8 i,receive=0;
  
  delayMicroseconds(2);  
  Y_SDA_H;  
  delayMicroseconds(2);
  
  for(i=0;i<8;i++)
  {
    Y_SCK_L;     
    delayMicroseconds(5);
    Y_SCK_H;      
    receive<<=1;
    delayMicroseconds(5);
    pinMode(Y_SDA_PIN, INPUT); 
    if(Y_READ_SDA)
      receive|=0x01;
    else
      receive&=0xfe;
    delayMicroseconds(5);
  }
  if(!ack)
    Y_IIC_Nack();
  else
    Y_IIC_Ack();
  
  Y_SCK_L;             
  delayMicroseconds(2);
  
  return receive;
}

void Y_IIC_Write_OneByte(u8 deviceaddr,u8 writeaddr,u8 writedata)
{
  Y_IIC_Start();
  Y_IIC_SendByte(deviceaddr&0xfe);
  Y_IIC_Wait_Ack();
  Y_IIC_SendByte(writeaddr);
  Y_IIC_Wait_Ack();
  Y_IIC_SendByte(writedata);
  Y_IIC_Wait_Ack();
  Y_IIC_Stop();
  delay(10);               
}


//res1 = Y_IIC_Read_OneByte((0x36<<1),0x0f); 
u8 Y_IIC_Read_OneByte(u8 deviceaddr,u8 readaddr)
{
  u8 temp;
  Y_IIC_Start();
  Y_IIC_SendByte(deviceaddr&0xfe);
  Y_IIC_Wait_Ack();
  Y_IIC_SendByte(readaddr);
  Y_IIC_Wait_Ack();

  Y_IIC_Start();
  Y_IIC_SendByte(deviceaddr|0x01);
  Y_IIC_Wait_Ack();
  temp=Y_IIC_ReadByte(0);
  Y_IIC_Stop();
  return temp;
}






