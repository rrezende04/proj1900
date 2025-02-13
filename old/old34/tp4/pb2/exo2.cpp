#define F_CPU 8000000


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



const uint16_t WAIT_TIME_MS = 10000;
const uint8_t BLINK_TIME_MS = 100;
const uint16_t WIN_TIME_CYCLE = 7813;
const uint8_t BUTTON_MASK = 0x04;
const uint8_t TICK_MS = 10;

volatile uint8_t gExpiredTimer = 2;
volatile uint8_t gPushButton = 0;


ISR(TIMER1_COMPA_vect)
{
	gExpiredTimer = 1;
}

ISR(INT0_vect)
{
	if (gExpiredTimer != 2)
	{
		if (PIND & BUTTON_MASK)
		{
			_delay_ms(TICK_MS);
			if (PIND & BUTTON_MASK)
			{
				gPushButton = 1;
			}
		}
	}
}

void initialisation(void)
{
	cli();

	DDRA |= (1 << PA0) | (1 << PA1);

	// cette procédure ajuste le registre EIMSK
	// de l’ATmega324PA pour permettre les interruptions externes
	EIMSK |= (1 << INT0);

	// il faut sensibiliser les interruptions externes aux
	// changements de niveau du bouton-poussoir
	// en ajustant le registre EICRA
	EICRA |= (1 << INT0);

	sei();
}

void startTimer(uint16_t duration)
{
	gExpiredTimer = 0;

	// mode CTC du timer 1 avec horloge divis�e par 1024

	// interruption apr�s la dur�e sp�cifi�e

	TCNT1 = 0;
	OCR1A = duration;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << CS12) | (1 << CS10);
	TCCR1C = 0;
	TIMSK1 = 0;
	TIMSK1 |= (1 << OCIE1A);
}

void displayRed()
{
	PORTA |= (1 << PA1);
	PORTA &= ~(1 << PA0);
}

void displayGreen()
{
	PORTA |= (1 << PA0);
	PORTA &= ~(1 << PA1);
}

void displayOff()
{
	PORTA &= ~(1 << PA0) & ~(1 << PA1);
}

int main()
{
	initialisation();

	displayOff();
	_delay_ms(WAIT_TIME_MS);
	displayRed();
	_delay_ms(BLINK_TIME_MS);
	displayOff();
	startTimer(WIN_TIME_CYCLE);

	do {

		// attendre qu'une des deux variables soit modifi�e

		// par une ou l'autre des interruptions.

	} while (gExpiredTimer == 0 && gPushButton == 0);


	// Une interruption s'est produite. Arr�ter toute

	// forme d'interruption. Une seule r�ponse suffit.

	// Verifier la r�ponse

	if (gExpiredTimer == 0 && gPushButton == 1)
	{
		displayGreen();
	}
	else
	{
		displayRed();
	}

}