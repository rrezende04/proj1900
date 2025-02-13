#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <memoire_24.cpp>

const uint16_t BAUDS = 2400;

void initialisation(void)
{
    DDRA |= (1 << PA0) | (1 << PA1);

    // 2400 bauds. Nous vous donnons la valeur des deux
    // premiers registres pour vous éviter des complications.
    UBRR0H = 0;
    UBRR0L = 0xCF;

    // permettre la réception et la transmission par le UART0
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);

    // Format des trames: 8 bits, 1 stop bits, sans parité
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    // UBRR0 = BAUDS;
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

void displayOff()
{
    PORTA &= ~(1 << PA0) & ~(1 << PA1);
}

// Du USART vers le PC
void transmissionUART(uint8_t donnee)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    /* Put data into buffer, sends the data */
    UDR0 = donnee;
    /* Wait for data to be received */
    // while (!(UCSR0A & (1 << RXC0)));
    /* Get and return received data from buffer */
    // return UDR0;
}

int main()
{
    initialisation();
    char mots[21] = "Le robot en INF1900\n";

    uint8_t i, j;

    for (i = 0; i < 100; i++)
    {

        for (j = 0; j < 20; j++)
        {

            transmissionUART(mots[j]);
        }
    }
}
