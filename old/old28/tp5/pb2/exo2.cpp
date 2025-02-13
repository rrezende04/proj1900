/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: TODO
 */

#define F_CPU 8000000

#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "memoire_24.h"

void turnLedOff() // 00
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA &= ~(1 << PA0); // x0
}

void turnLedGreen() // 01
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA |= (1 << PA0);  // x1
}

void turnLedRed() // 10
{
    PORTA |= (1 << PA1);  // 1x
    PORTA &= ~(1 << PA0); // x0
}

void initializePorts()
{
    DDRA |= (1 << PA0) | (1 << PA1);
    DDRD |= (1 << PD1);
    DDRD &= ~(1 << PD0);
}

void initialisationUART(void)
{
    // 2400 bauds. Nous vous donnons la valeur des deux
    // premiers registres pour vous éviter des complications.
    UBRR0H = 0;
    UBRR0L = 0xCF;

    // permettre la réception et la transmission par le UART0
    UCSR0A &= ~(1 << FE0) & ~(1 << DOR0) & ~(1 << UPE0);
    // UCSR0A |= (1 << RXC0) | (1 << TXC0);

    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    UCSR0B &= ~(1 << UCSZ02);

    // Format des trames: 8 bits, 1 stop bits, sans parité
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
    UCSR0C &= ~(1 << USBS0) & ~(1 << UPM01) & ~(1 << UPM00) & ~(1 << UMSEL01) & ~(1 << UMSEL00);
}

// Du USART vers le PC
void transmissionUART(uint8_t donnee)
{
    // attendre pour un transmit buffer vide
    while (!(UCSR0A & (1 << UDRE0)));
    
    UDR0 = donnee; // code from doc p.175
}

int main()
{
    initializePorts();
    initialisationUART();
    _delay_ms(2000);
    turnLedGreen();
    _delay_ms(1000);
    turnLedOff();

    char mots[21] = "Le robot en INF1900\n";
    uint8_t i, j;
    for (i = 0; i < 100; i++)
        for (j = 0; j < 20; j++)
        {
            transmissionUART(mots[j]);
            _delay_ms(5);
        }
    return 0;
}
