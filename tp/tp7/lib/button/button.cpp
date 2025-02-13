/*
 * Classe permettant d'utiliser un bouton mécanique
 *
 * Ecole Polytechnique de Montreal
 * Departement de genie informatique
 * Cours inf1900
 *
 * Eliott Bonnefoy, Gabriel Bruyere, Thierry Poulin, Renato Rezende
 * 2024
 *
 * Code qui n'est sous aucune license.
 *
 */


#include "button.h"

    // Constructor
Button::Button()
: pin_           (Pin(PortID::D, 2, 0))
, buttonMask_    (DEFAULT_BUTTON_MASK)
, isInverseLogic_(false)
{
    pinNumber_ = pin_.getPinNumber();
    pin_.setReadMode();
}

Button::Button(Pin&    pin           ,
               uint8_t buttonMask    ,
               bool    isInverseLogic)

    //member attributes
: pin_           (pin)
, buttonMask_    (buttonMask)
, isInverseLogic_(isInverseLogic)
{}

void Button::enableExternalInterrupts()
{
    if (pinNumber_ == PD2)
        EIMSK |= (1 << INT0);
    else if (pinNumber_ == PD3)
        EIMSK |= (1 << INT1);
}

void Button::disableExternalInterrupts()
{
    if (pinNumber_ == PD2)
        EIMSK &= ~(1 << INT0);
    else if (pinNumber_ == PD3)
        EIMSK &= ~(1 << INT1);
}

void Button::catchRisingEdge()
{
    if (pinNumber_ == PD2)
        EICRA |= (1 << ISC01) | (1 << ISC00);
    else if (pinNumber_ == PD3)
        EICRA |= (1 << ISC11) | (1 << ISC10);
}

void Button::catchFallingEdge()
{
    if (pinNumber_ == PD2)
    {
        EICRA |= (1 << ISC01);
        EICRA &= ~(1 << ISC00);
    }
    else if (pinNumber_ == PD3)
    {
        EICRA |= (1 << ISC11);
        EICRA &= ~(1 << ISC10);
    }
}

void Button::catchAll()
{
    if (pinNumber_ == PD2)
    {
        EICRA |= (1 << ISC00);
        EICRA &= ~(1 << ISC01);
    }
    else if (pinNumber_ == PD3)
    {
        EICRA |= (1 << ISC10);
        EICRA &= ~(1 << ISC11);
    }
}

bool Button::isMaskCorresponding()
{
    if (pinNumber_ & buttonMask_)
	{
		_delay_ms(10);
		if (pinNumber_ & buttonMask_)
		{
			return true;
		}
	}
	return false;
}

bool Button::isMaskNotCorresponding()
{
    if (!(pinNumber_ & buttonMask_))
	{
		_delay_ms(10);
		if (!(pinNumber_ & buttonMask_))
		{
			return true;
		}
	}
	return false;
}

bool Button::isPressed()
{
    if(isMaskCorresponding() && !isInverseLogic_)
    {
        pressedOnce = true;
        return true;
    }
    else if(isMaskNotCorresponding() && isInverseLogic_)
    {
        pressedOnce = true;
        return true;
    }

    return false;
}

bool Button::isReleased()
{

    if(pressedOnce)
    {
        if(isMaskNotCorresponding() && !isInverseLogic_)
        {
            pressedOnce = false;
            return true;
        }
        else if(isMaskCorresponding() && isInverseLogic_)
        {
            pressedOnce = false;
            return true;
        }
    }
    return false;
}

    // Destructor
Button::~Button()
{}