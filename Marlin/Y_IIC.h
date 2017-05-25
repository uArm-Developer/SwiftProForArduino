#ifndef _Y_IIC_H
#define _Y_IIC_H

#include <Arduino.h>

void Y_IIC_Init(void);

void Y_IIC_Write_OneByte(u8 deviceaddr,u8 writeaddr,u8 writedata);

u8 Y_IIC_Read_OneByte(u8 deviceaddr,u8 readaddr);


#endif // _Y_IIC_H