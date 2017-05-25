#ifndef _X_IIC_H
#define _X_IIC_H

#include <Arduino.h>

void X_IIC_Init(void);

void X_IIC_Write_OneByte(u8 deviceaddr,u8 writeaddr,u8 writedata);

u8 X_IIC_Read_OneByte(u8 deviceaddr,u8 readaddr);


#endif // _X_IIC_H