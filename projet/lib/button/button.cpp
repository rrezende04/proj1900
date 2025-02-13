#include "button.h"

    // General constructor
Button::Button(Pin&  pin           ,
               bool  isInverseLogic,
               bool  isInterrupts)
    // Initialization list
: pin_           (&pin)
, pinNumber_     (pin.getPinNumber())
, isInverseLogic_(isInverseLogic)
, isInterrupts_  (isInterrupts)
{
        // Retrieve information from pin object
    pin_->setReadMode();

        // Set registers depending on pin used
    if (pinNumber_ == PD2)
    {
        externalInterruptRequestEnable_ = (1 << INT0);
        interruptSenseControlMask0_     = (1 << ISC00);
        interruptSenseControlMask1_     = (1 << ISC01);
    }
    else if (pinNumber_ == PD3)
    {
        externalInterruptRequestEnable_ = (1 << INT1);
        interruptSenseControlMask0_     = (1 << ISC10);
        interruptSenseControlMask1_     = (1 << ISC11);
    }
    if(isInterrupts_)
    {
        catchAll();
        enableExternalInterrupts();
    }
}

Button::~Button() { disableExternalInterrupts(); }

void Button::enableExternalInterrupts()
{
        // Sets INTn value to 1 in External Interrupt Mask register
    EIMSK |= externalInterruptRequestEnable_;
}

void Button::disableExternalInterrupts()
{
        // Sets INTn value to 0 in External Interrupt Mask register
    EIMSK &= ~ externalInterruptRequestEnable_;
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

bool Button::debounce()
{
    bool bit = pin_->read();
    _delay_ms(DEBOUNCE_TIME_MS);
	return bit == pin_->read();
}

bool Button::isPressed()
{ return (pin_->read() == !isInverseLogic_) && debounce(); }

bool Button::isReleased()
{ return (pin_->read() == isInverseLogic_) && debounce(); }

uint8_t Button::getPinNumber() { return pinNumber_; }
