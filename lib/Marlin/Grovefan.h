
#ifndef Grovefan
#define Grovefan_H

class Grovefan
{
	public:
		Grovefan();

		void setPin(int pin);
		void on(void);
		void off(void);
		void speed(uint8_t   speed );
	private:
		int _pin;
};
#endif
