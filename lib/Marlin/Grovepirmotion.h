
#ifndef GROVEPIRMOTION_H
#define GROVEPIRMOTION_H

class Grovepirmotion
{
	public:
		Grovepirmotion();

		void setPin(int pin);
		uint8_t getstatus(void);
	private:
		int _pin;
};
#endif
