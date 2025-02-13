#include "communicator.h"

void setUsartRegisters()
{
    //initialization overrides normal port operation
    //set registers :

        //baud rate
    UBRR0H = 0;
    UBRR0L = 0xCF;

        //Uncomment if write to UCSR0A required
    //UCSR0A &= ~(1 << FE0) & ~(1 << DOR0) & ~(1 << UPE0);

        //Receiver En, Transmitter En,
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

        //Char size specification
    UCSR0B &= ~(1 << UCSZ02);
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

        //Stop bit, Parity mode
    UCSR0C &= ~(1 << USBS0) & ~(1 << UPM01) & ~(1 << UPM00);

        //Mode select
    UCSR0C &= ~(1 << UMSEL01) & ~(1 << UMSEL00);

        //Current setup specifies 8 bits, 1 stop bit,
        //no parity and a baud rate of 2400.
}


void sendChar(uint8_t aChar)
{
        // wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = aChar; // code from doc p.175
}

void sendString(uint8_t length, char* stringPtr)
{
    for(uint8_t i = 0; i < length; i++)
    {
        sendChar(uint8_t(stringPtr[i]));
        _delay_ms(5);
    }
}

unsigned char receiveChar()
{
        // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));
        // Get and return received data from buffer
    return UDR0;
}