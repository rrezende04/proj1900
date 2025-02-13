#define F_CPU 8000000


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


const uint16_t WHEEL_TIME_MS = 2000;
const uint8_t MAX_PMW = 255;
const uint8_t CENT = 100;
const uint8_t PWM_PERCENT_INCREASE = 25;



void initialisation(void)
{
	cli();

	DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);

	PORTD &= ~(1 << PD2) & ~(1 << PD3);

	sei();
}

uint8_t convertPercentToValue(int percent)
{
	return (percent * MAX_PMW / CENT);
}

void ajustementPwm(uint8_t leftWheelPwm, uint8_t rightWheelPwm) {

	// mise a un des sorties OC1A et OC1B sur comparaison

	// reussie en mode PWM 8 bits, phase correcte

	// et valeur de TOP fixe a 0xFF (mode #1 de la table 16-5

	// page 130 de la description technique du ATmega324PA)


	OCR1A = leftWheelPwm;

	OCR1B = rightWheelPwm;



	// division d'horloge par 8 - implique une frequence de PWM fixe
	TCCR1A = 0;
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);

	TCCR1B = 0;
	TCCR1B |= (1 << CS11);

	TCCR1C = 0;

}

void stopTimer()
{
	TCCR1B = 0;
}

void stopPwm()
{
	TCCR1A = 0;
}

int main()
{
	initialisation();

	uint8_t currentPwmPercent = 0;

	do
	{
		currentPwmPercent += PWM_PERCENT_INCREASE;
		ajustementPwm(convertPercentToValue(currentPwmPercent), convertPercentToValue(currentPwmPercent));
		_delay_ms(WHEEL_TIME_MS);
	} while (currentPwmPercent < CENT);

	stopPwm();
	stopTimer();
}