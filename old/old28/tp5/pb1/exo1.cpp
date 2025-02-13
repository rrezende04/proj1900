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
    DDRA |= (1 << PA1);  // PA1 sortie
    DDRA |= (1 << PA0);  // PA0 sortie
}

int main()
{
    initializePorts();
    Memoire24CXXX memory;

    const char* str = "*P*O*L*Y*T*E*C*H*N*I*Q*U*E* *M*O*N*T*R*E*A*L*";
    uint8_t length = strlen(str)+1;
    memory.ecriture((uint16_t)0x0000, (uint8_t *)str, length);
    
    char readStr[128];
    memory.lecture((uint16_t)0x0000, (uint8_t *)readStr, length);

    if (!strcmp(str, readStr)) // Strings are equal
        turnLedGreen();
    else
        turnLedRed();

    return 0;
}
