/**
  ******************************************************************************
  * @file	uArmSerial.h
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-21
  ******************************************************************************
  */

#ifndef _UARMSERIAL_H_
#define _UARMSERIAL_H_

#include <Arduino.h>
#include "Print.h"

class uArmSerial : public Print
{
public:
	uArmSerial();
	void setSerialPortLock(bool locked);
	void setSerialPort(HardwareSerial* serial);	

    void begin(unsigned long baud) { begin(baud, SERIAL_8N1); }
    void begin(unsigned long, uint8_t);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    int availableForWrite(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool() { return true; }

private:
	HardwareSerial *_serial;
	unsigned char _serialPortLock = false;
};

extern uArmSerial commSerial;

#endif // _UARMSERIAL_H_
