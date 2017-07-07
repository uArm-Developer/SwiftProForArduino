#ifndef _Z_IIC_H
#define _Z_IIC_H

#include <Arduino.h>

void Z_IIC_Init(void);

void Z_IIC_Write_OneByte(u8 deviceaddr,u8 writeaddr,u8 writedata);

u8 Z_IIC_Read_OneByte(u8 deviceaddr,u8 readaddr);


#endif // _Z_IIC_H