
#ifndef GROVEELECTROMAGNET_H
#define GROVEELECTROMAGNET_H

class Groveelectromagnet
{
	public:
		Groveelectromagnet();

		void setPin(int pin);
		void on(void);
		void off(void);
		
	private:
		int _pin;
};
#endif
