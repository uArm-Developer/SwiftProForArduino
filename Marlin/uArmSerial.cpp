/**
  ******************************************************************************
  * @file	uArmSerial.cpp
  * @author	David.Long	
  * @email	xiaokun.long@ufactory.cc
  * @date	2017-03-21
  ******************************************************************************
  */

#include "uArmSerial.h" 

uArmSerial commSerial;

uArmSerial::uArmSerial()
{
	_serial = &Serial;
}

void uArmSerial::setSerialPortLock(bool locked)
{
	_serialPortLock = locked;
}

void uArmSerial::setSerialPort(HardwareSerial* serial)
{
	if (serial == NULL || _serialPortLock)
	{
		return;
	}
	
	_serial = serial;
}

void uArmSerial::begin(unsigned long baud, uint8_t config)
{
	_serial->begin(baud, config);
}

void uArmSerial::end()
{
	_serial->end();
}

int uArmSerial::available(void)
{
	return _serial->available();
}

int uArmSerial::peek(void)
{
	return _serial->peek();
}

int uArmSerial::read(void)
{
	return _serial->read();
}

int uArmSerial::availableForWrite(void)
{
	return _serial->availableForWrite();
}

void uArmSerial::flush(void)
{
	_serial->flush();
}

size_t uArmSerial::write(uint8_t n)
{
	return _serial->write(n);
}