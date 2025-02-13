#include "pin.h"

// Public
Pin::Pin(PortID portID, uint8_t pinNumber, bool writeMode)
: PORT_ID_(portID)
, MASK_(1 << pinNumber)
, PIN_NUMBER_(pinNumber)
, OLD_DDR_(*getDDR() & MASK_)
{
    writeMode ? setWriteMode() : setReadMode();
}

Pin::~Pin()
{
    setDDR(OLD_DDR_);
}

void Pin::setReadMode()
{
    setDDR(false);
}

bool Pin::read() const
{
    uint8_t PIN = getPIN();
    return PIN & MASK_;    
}

void Pin::setWriteMode()
{
    setDDR(true);
}

void Pin::write(bool bit)
{
    volatile uint8_t* PORT = getPORT();
    if(bit)
        *PORT |= MASK_;
    else
        *PORT &= ~MASK_;
}

uint8_t Pin::getMask() const
{
    return MASK_;
}

uint8_t Pin::getPinNumber() const
{
    return PIN_NUMBER_;
}

PortID Pin::getPortID() const
{
    return PORT_ID_;
}

void Pin::toggle()
{
    volatile uint8_t* PORT = getPORT();
    *PORT ^= MASK_;
}

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
    volatile uint8_t* DDR = getDDR();
    if(bit)
        *DDR |= MASK_;
    else
        *DDR &= ~MASK_;
}
