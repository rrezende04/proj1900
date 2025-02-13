////////////////////////////////////////////////////////////////////////////////
/// \Auteurs: Gabriel Bruyère & Eliott Bonnefoy
/// 
/// \Description: Contrôle de la DEL à l'aide du bouton-poussoir pour usage général
///				  selon la machine à états représentée dans le tableau plus bas.
/// 
/// \Identifiactions Matérielles:
///		-Bouton-poussoir libre pour usage général [Entré - PortD pin2]
///		-DEL libre [Sortie - PortA pin0(-) & pin1(+)]
///		-Câble violet et blanc
/// 
/// \Tables des états:
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


const uint8_t TICK = 10;
const uint8_t MASK = 0x04;
const uint8_t YELLOW_RED_TIME = 5;
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


bool isButtonPressed(bool& buttonWasPressed)
{
	if (PIND & MASK)
	{
		_delay_ms(TICK);
		if (PIND & MASK)
		{
			buttonWasPressed = true;
			return true;
		}
	}
	return false;
}

bool isButtonReleased(bool& buttonWasPressed)
{
	bool result = !isButtonPressed(buttonWasPressed) && buttonWasPressed;
	buttonWasPressed = result;
	return result;
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
			else if(button == ButtonState::RELEASED)
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
			else if(button == ButtonState::RELEASED)
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
			if (button == ButtonState::RELEASED)
			{
				return LedColor::GREEN;
			}
		}
			break;
	}
}

ButtonState updateButtonState(bool& buttonWasPressed)
{
	if (isButtonPressed(buttonWasPressed))
	{
		return ButtonState::PRESSED;
	}
	if (isButtonReleased(buttonWasPressed))
	{
		return ButtonState::RELEASED;
	}
	return ButtonState::UNTOUCHED;
}

int main()
{
	DDRA |= (1 << PA0) | (1 << PA1);

	bool buttonWasPressed = false;
	ButtonState previousButtonState = ButtonState::UNTOUCHED;
	ButtonState currentButtonState = ButtonState::UNTOUCHED;
	LedColor currentColor = LedColor::RED;

	while (true)
	{
		while (currentButtonState == previousButtonState || currentButtonState == ButtonState::UNTOUCHED)
		{
			displayColor(currentColor);
			currentButtonState = updateButtonState(buttonWasPressed);
		}
		previousButtonState = currentButtonState;
		currentColor = updateColorState(currentColor, currentButtonState);
	}
}
