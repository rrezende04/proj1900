/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: La LED est allumée selon l'intensité lumineuse mesurée par
 *              une photorésistance :
 *                  Faible intensité  (photorésistance cachée) = DEL verte;
 *                  Moyenne intensité (lumière ambiante)       = DEL ambrée;
 *                  Haute intensité   (lampe de poche)         = DEL rouge.
 * 
 *              LED branchée sur PB[1:0] : PB0+, PB1-
 *                  (1,0) = vert;
 *                  (0,1) = rouge.
 *              Fil de données (photorésistance) branché sur PA0.
 * 
 * Date: 7 octobre 2024
 */



#define F_CPU 8000000

#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can.cpp"

enum Class Brightness
{
    LOW,
    MID,
    HIGH
};

const uint8_t LOWER_BOUND = 32; // ~ un quart de 255
const uint8_t UPPER_BOUND = 255 - 48; // ~ 255 - un quart
const uint8_t SHIT_VALUE = 2;

void turnLedOff() // 00
{
    PORTB &= ~(1 << PB0); // 0x
    PORTB &= ~(1 << PB1); // x0
}

void turnLedGreen() // 01
{
    PORTB &= ~(1 << PB0); // 0x
    PORTB |= (1 << PB1);  // x1
}

void turnLedRed() // 10
{
    PORTB |= (1 << PB0);  // 1x
    PORTB &= ~(1 << PB1); // x0
}

void turnLedAmber() // Ambre pour 8 ms
{
    turnLedRed();
    _delay_ms(2);
    turnLedGreen();
    _delay_ms(5);
}

void initializeRegisters()
{
    cli();
    // LED
    DDRA &= ~(1 << PA0);
    DDRB |= (1 << PB0) | (1 << PB1);
    sei();
}

Brightness convertToState(uint8_t read)
{   
    Brightness brightness;
    if (read <= LOWER_BOUND)
        brightness = Brightness::LOW;
    else if (read <= UPPER_BOUND )
        brightness = Brightness::MID;
    else
        brightness = Brightness::HIGH;
    return brightness;
}

int main()
{
    can converter = can();
    Brightness brightness = Brightness::LOW;
    initializeRegisters();

    while (true)
    {
        brightness = convertToState(converter.lecture(PA0) >> SHIFT_VALUE);
        switch (brightness)
        {
            case Brightness::LOW : turnLedGreen(); break;
            case Brightness::MID : turnLedAmber(); break;
            case Brightness::HIGH: turnLedRed();   break;
        }
    }

    return 0;
}
