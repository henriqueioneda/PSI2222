blalblabalbalbal

#ifndef Cofre_h
#define Cofre_h

#include "Arduino.h"

class Cofre
{
	public:
		Cofre(int chave_A, int chave_B, int chave_C, int chave_D, int chave_E, int LED_R, int LED_G, int LED_B, int temp);
		void acende_led(int led);
		void apaga_led();
		int senha_errada(int estado);
		int leia_chave();
	private:
		int _chave_A, _chave_B, _chave_C, _chave_D, _chave_E, _LED_R, _LED_G, _LED_B, _temp;
};
#endif
