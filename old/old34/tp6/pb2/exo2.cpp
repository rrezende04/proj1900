/////////////////////////////////////////////////////////////////////////////////
/// \Auteurs: Gabriel Bruy�re & Eliott Bonnefoy
/// 
/// \Description: Contr�le de la DEL � l'aide du bouton-poussoir blanc sur le 
///				  breadboard selon la machine � �tats repr�sent�e dans le tableau 
///				  plus bas.
/// 
/// \Identifiactions Mat�rielles:
///		-Photorésistance [Entr�e - PortA pin0]
///		-DEL libre [Sortie - PortB pin0(-) & pin1(+)]
///		-C�ble violet et blanc
///		-C�ble rouge et noir
///     -Câble bleu et brun
///		-Condensateur de 0.1 �F
///		-R�sistance de 10K
/// 
//////////////////////////////////////////////////////////////////////////////////

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include "can.cpp"

const uint8_t AMBER_RED_DELAY_MS = 3;
const uint8_t AMBER_GREEN_DELAY_MS = 5;
const uint8_t DECAL_10_TO_8 = 2;
const uint8_t BRIGHT_LIGHT_THRESHOLD = 230;
const uint8_t DIM_LIGHT_TRESHOLD = 180;

void displayGreen()
{
	PORTB |= (1 << PB1);
	PORTB &= ~(1 << PB0);
}

void displayRed()
{
	PORTB |= (1 << PB0);
	PORTB &= ~(1 << PB1);
}

void displayAmber()
{
	displayGreen();
	_delay_ms(AMBER_GREEN_DELAY_MS);
	displayRed();
	_delay_ms(AMBER_RED_DELAY_MS);
}



void initialisation(void)
{
	DDRB |= (1 << PB0) | (1 << PB1);
	DDRA &= ~(1 << PD0);
}


int main()
{
	initialisation();
	can converter = can();
	uint16_t converterRead = 0;
	while(true)
	{
		converterRead = converter.lecture(PA0);

		if((converterRead >> DECAL_10_TO_8) > BRIGHT_LIGHT_THRESHOLD) //Décallage de 2 bits vers la droite pour passer de 10 à 8 en enlevant les moins significatifs
			displayRed();
		else if((converterRead >> DECAL_10_TO_8) > DIM_LIGHT_TRESHOLD)
			displayAmber();
		else
			displayGreen();
	}
	
}