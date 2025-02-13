#include "pin.h"

// Public
Pin::Pin(PortID portID, uint8_t pinNumber, bool writeMode)
: PORT_ID_   (portID)
, MASK_      (1 << pinNumber)
, PIN_NUMBER_(pinNumber)
, OLD_DDR_   (*getDDR() & MASK_)
{ writeMode ? setWriteMode() : setReadMode(); }

Pin::~Pin()
{ setDDR(OLD_DDR_); }

    // Read related functions
void Pin::setReadMode()
{ setDDR(0); }

bool Pin::read() const
{ return getPIN() & MASK_; }

    // Write related functions
void Pin::setWriteMode()
{ setDDR(1); }

void Pin::write(bool bit)
{
    if(bit)
        *getPORT() |= MASK_;
    else
        *getPORT() &= ~MASK_;
}

void Pin::toggle()
{ *getPORT() ^= MASK_; }

    // Getters for member attributes
uint8_t Pin::getMask()      const { return MASK_;       }
uint8_t Pin::getPinNumber() const { return PIN_NUMBER_; }
PortID  Pin::getPortID()    const { return PORT_ID_;    }

    // Private
volatile uint8_t* Pin::getDDR()
{
    volatile uint8_t* DDR = nullptr;
    switch(PORT_ID_)
    {
        case PortID::A:
            DDR = &DDRA;
            break;
        
        case PortID::B:
            DDR = &DDRB;
            break;
        
        case PortID::C:
            DDR = &DDRC;
            break;
        
        case PortID::D:
            DDR = &DDRD;
            break;
    }
    return DDR;
}

uint8_t Pin::getPIN() const
{
    uint8_t PIN = 0;
    switch(PORT_ID_)
    {
        case PortID::A:
            PIN = PINA;
            break;
        
        case PortID::B:
            PIN = PINB;
            break;
        
        case PortID::C:
            PIN = PINC;
            break;
        
        case PortID::D:
            PIN = PIND;
            break;
    }
    return PIN;
}

volatile uint8_t* Pin::getPORT()
{
    volatile uint8_t* PORT = nullptr;
    switch(PORT_ID_)
    {
        case PortID::A:
            PORT = &PORTA;
            break;
        
        case PortID::B:
            PORT = &PORTB;
            break;
        
        case PortID::C:
            PORT = &PORTC;
            break;
        
        case PortID::D:
            PORT = &PORTD;
            break;
    }
    return PORT;
}

void Pin::setDDR(bool bit)
{
    if(bit)
        *getDDR() |= MASK_;
    else
        *getDDR() &= ~MASK_;
}
