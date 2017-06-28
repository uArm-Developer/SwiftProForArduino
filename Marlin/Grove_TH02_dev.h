#ifndef _Grove_TH02_DEV_H
#define _Grove_TH02_DEV_H

/****************************************************************************/
/***        Including Files                                               ***/
/****************************************************************************/
#include <Wire.h>
#include <Arduino.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define TH02_I2C_DEV_ID      0x40
#define REG_STATUS           0x00 
#define REG_DATA_H           0x01
#define REG_DATA_L           0x02
#define REG_CONFIG           0x03
#define REG_ID               0x11

#define STATUS_RDY_MASK      0x01    //poll RDY,0 indicate the conversion is done

#define CMD_MEASURE_HUMI     0x01    //perform a humility measurement
#define CMD_MEASURE_TEMP     0x11    //perform a temperature measurement

#define TH02_WR_REG_MODE      0xC0
#define TH02_RD_REG_MODE      0x80
/****************************************************************************/
/***        Class Definition                                              ***/
/****************************************************************************/
class TH02_dev
{
public:
	void begin();
	uint8_t isAvailable();
	float ReadTemperature(void);
	float ReadHumidity(void);
private:
	void TH02_IIC_WriteCmd(uint8_t u8Cmd);
	uint8_t TH02_IIC_ReadReg(uint8_t u8Reg);
	void TH02_IIC_WriteReg(uint8_t u8Reg,uint8_t u8Data);
	uint16_t TH02_IIC_ReadData(void);
	uint16_t TH02_IIC_ReadData2byte(void);
};
extern TH02_dev Grove_TH02;

#endif  // _TH02_DEV_H
