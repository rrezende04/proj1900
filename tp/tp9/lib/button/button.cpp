#include "button.h"

    // General constructor
Button::Button( Pin& pin                  ,
               bool  isInverseLogic       ,
               bool  useExternalInterrupts)
    // Initialization list
: pin_           (&pin)
, pinNumber_     (pin.getPinNumber())
, isInverseLogic_(isInverseLogic)
{
        // Retrieve information from pin object
    pin_->setReadMode();

        // Set registers depending on pin used
    if (pinNumber_ == PD2)
    {
        externalInterruptRequestEnable_ = (1 << INT0) ;
        interruptSenseControlMask0_     = (1 << ISC00);
        interruptSenseControlMask1_     = (1 << ISC01);
    }
    else if (pinNumber_ == PD3)
    {
        externalInterruptRequestEnable_ = (1 << INT1) ;
        interruptSenseControlMask0_     = (1 << ISC10);
        interruptSenseControlMask1_     = (1 << ISC11);
    }

    if (useExternalInterrupts) { enableExternalInterrupts(); }
}

void Button::enableExternalInterrupts()
{
        // Sets INTn value to 1 in External Interrupt Mask register
    cli();
    EIMSK |= externalInterruptRequestEnable_;
    sei();
}

void Button::disableExternalInterrupts()
{
        // Sets INTn value to 0 in External Interrupt Mask register
    cli();
    EIMSK &= ~ externalInterruptRequestEnable_;
    sei();
}

void Button::catchRisingEdge()
{
        // Set ISCn1 & ISCn0 to 1
    EICRA |= interruptSenseControlMask0_
          |  interruptSenseControlMask1_;
}

void Button::catchFallingEdge()
{
        // Set ISCn1 to 1 & ISCn0 to 0
    EICRA |=   interruptSenseControlMask1_;
    EICRA &= ~ interruptSenseControlMask0_;
}

void Button::catchAll()
{
        // Set ISCn1 to 0 & ISCn0 to 1
    EICRA &= ~ interruptSenseControlMask1_;
    EICRA |=   interruptSenseControlMask0_;    
}

bool Button::isMaskCorresponding()
{
    if (pin_->read())
	{
		_delay_ms(DEBOUNCE_TIME_MS);
		if (pin_->read())
			return true;
	}
	return false;
}

bool Button::isMaskNotCorresponding()
{
    if (!(pin_->read()))
	{
		_delay_ms(DEBOUNCE_TIME_MS);
		if (!(pin_->read()))
			return true;
	}
	return false;
}

bool Button::isPressed()
{
    if(isMaskCorresponding() && !isInverseLogic_)
    {
        pressedOnce_ = true;
        return true;
    }
    else if(isMaskNotCorresponding() && isInverseLogic_)
    {
        pressedOnce_ = true;
        return true;
    }

    return false;
}

bool Button::isReleased()
{

    if(pressedOnce_)
    {
        if(isMaskNotCorresponding() && !isInverseLogic_)
        {
            pressedOnce_ = false;
            return true;
        }
        else if(isMaskCorresponding() && isInverseLogic_)
        {
            pressedOnce_ = false;
            return true;
        }
    }
    return false;
}

// bool Button::isPressed()
// {
//     //debounce
// }

// bool Button::debounce()
// {
//     bool initialRead = pin_->read();
//     _delay_ms(10);
//     if (pin_->read() == initialRead);
//         return initialRead ? enum::pressed : enumUnpressed
//     reutnr enum::undef
// }

    // Destructor
Button::~Button()
{}