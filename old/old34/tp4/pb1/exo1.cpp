////////////////////////////////////////////////////////////////////////////////
/// \Auteurs: Gabriel Bruy�re & Eliott Bonnefoy
///
/// \Description: Contr�le de la DEL � l'aide du bouton-poussoir pour usage g�n�ral
///				  selon la machine � �tats repr�sent�e dans le tableau plus bas.
///
/// \Identifiactions Mat�rielles:
///		-Bouton-poussoir libre pour usage g�n�ral [Entr� - PortD pin2]
///		-DEL libre [Sortie - PortA pin0(-) & pin1(+)]
///		-C�ble violet et blanc
///
/// \Tables des �tats:
/// +---------------+--------------+------------+-----------+
/// | Current State | Button State | Next State | LED Color |
/// +---------------+--------------+------------+-----------+
/// | RED           | PRESSED      | YELLOW     | Red       |
/// | RED           | RELEASED     | OFF        | Red       |
/// | GREEN         | PRESSED      | RED        | Green     |
/// | GREEN         | RELEASED     | RED        | Green     |
/// | YELLOW        | PRESSED      | YELLOW     | Amber     |
/// | YELLOW        | RELEASED     | GREEN      | Amber     |
/// | OFF           | PRESSED      | GREEN      | Off       |
/// | OFF           | RELEASED     | YELLOW     | Off       |
/// +---------------+--------------+------------+-----------+
///
////////////////////////////////////////////////////////////////////////////////

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

const uint8_t TICK = 30;
const uint8_t MASK = 0x04;
const uint8_t YELLOW_RED_TIME = 1;
const uint8_t YELOW_GREEN_TIME = 3;

enum class LedColor
{
	OFF,
	RED,
	GREEN,
	YELLOW
};

enum class ButtonState
{
	TOUCHED,
	UNTOUCHED,
	PRESSED,
	RELEASED
};

volatile ButtonState buttonState = ButtonState::UNTOUCHED;
volatile bool pressed = false;
volatile LedColor currentColor = LedColor::RED;

void initialisation(void)
{

	// cli est une routine qui bloque toutes les interruptions.
	// Il serait bien mauvais d'être interrompu alors que
	// le microcontrôleur n'est pas prêt...

	cli();

	// configurer et choisir les ports pour les entrées
	// et les sorties. DDRx... Initialisez bien vos variables
	DDRA |= (1 << PA0) | (1 << PA1);

	// cette procédure ajuste le registre EIMSK
	// de l’ATmega324PA pour permettre les interruptions externes
	EIMSK |= (1 << INT0);

	// il faut sensibiliser les interruptions externes aux
	// changements de niveau du bouton-poussoir
	// en ajustant le registre EICRA
	EICRA |= (1 << INT0);

	// sei permet de recevoir à nouveau des interruptions.

	sei();
}

LedColor updateColorState(LedColor color, ButtonState button)
{
	switch (color)
	{
	case LedColor::RED:
	{
		if (button == ButtonState::PRESSED)
		{
			return LedColor::YELLOW;
		}
		else if (button == ButtonState::RELEASED)
		{
			return LedColor::OFF;
		}
	}
	break;
	case LedColor::GREEN:
	{
		if (button == ButtonState::PRESSED)
		{
			return LedColor::RED;
		}
		else if (button == ButtonState::RELEASED)
		{
			return LedColor::RED;
		}
	}
	break;
	case LedColor::YELLOW:
	{
		if (button == ButtonState::RELEASED)
		{
			return LedColor::GREEN;
		}
	}
	break;
	case LedColor::OFF:
	{
		if (button == ButtonState::PRESSED)
		{
			return LedColor::GREEN;
		}
	}
	break;
	}
	return color;
}

ISR(INT0_vect)
{
	// laisser un délai avant de confirmer la réponse du
	// bouton-poussoir: environ 30 ms (anti-rebond)
	// se souvenir ici si le bouton est pressé ou relâché
	if (PIND & MASK)
	{
		_delay_ms(TICK);
		if (PIND & MASK)
		{
			pressed = true;
			buttonState = ButtonState::PRESSED;
		}
	}
	else if (PIND != MASK)
	{
		_delay_ms(TICK);
		if (PIND != MASK && pressed)
		{
			buttonState = ButtonState::RELEASED;
			pressed = false;
		}
	}

	// changements d'état tels que ceux de la
	// semaine précédente
	currentColor = updateColorState(currentColor, buttonState);

	// Voir la note plus bas pour comprendre cette instruction et son rôle
	EIFR |= (1 << INTF0);
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

void displayYellow()
{
	displayRed();
	_delay_ms(YELLOW_RED_TIME);
	displayGreen();
	_delay_ms(YELOW_GREEN_TIME);
}

void displayOff()
{
	PORTA &= ~(1 << PA0) & ~(1 << PA1);
}

void displayColor(LedColor color)
{
	switch (color)
	{
	case LedColor::RED:
		displayRed();
		break;
	case LedColor::GREEN:
		displayGreen();
		break;
	case LedColor::YELLOW:
		displayYellow();
		break;
	case LedColor::OFF:
		displayOff();
		break;
	}
}

int main()
{
	initialisation();

	// bool buttonWasPressed = false;
	// ButtonState previousButtonState = ButtonState::UNTOUCHED;
	// ButtonState currentButtonState = ButtonState::UNTOUCHED;

	while (true)
	{
		displayColor(currentColor);
	}
}
