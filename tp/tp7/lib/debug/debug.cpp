#include "debug.h"
#include <stdlib.h>

void debugPrint(char myChar)
{
    const uint8_t NEW_LINE = 10;

    sendChar(myChar);
    sendChar(NEW_LINE);
}

void debugPrintStr (char* ptrStr, uint8_t length)
{
    const uint8_t NEW_LINE = 10;

    sendString(length, ptrStr);
    sendChar(NEW_LINE);
}


void debugPrint (uint32_t myInt)
{
    const uint8_t DIGITS_IN_32_BIT    = 10;
    const uint8_t DELAY_RESOLUTION    = 5;
    const uint8_t NEW_LINE            = 10;
          char    str[]               = "";
    const uint8_t BASE_10             = 10;

        //reduire nombre d'appels de fct.
    uint32_t storePower  = 0;
    uint8_t  storeInt    = 0;
    bool     digitIsZero = true;
    for (int i = DIGITS_IN_32_BIT; i > 0; i--)
    {
        storePower = debugGetPowerOf10(i);
        storeInt   = myInt / storePower;

            // Les "Leading zeros" sont alors ignores
        if (storeInt != 0)
            digitIsZero = false;

            // On ne print pas avant
            // d'eliminer les 0 devant
        if (! digitIsZero)
        {
            sendChar(*itoa(storeInt, str, BASE_10));
            _delay_ms(DELAY_RESOLUTION);
        }
        myInt = myInt % storePower;
    }
    sendChar(NEW_LINE);
}


uint32_t debugGetPowerOf10(uint8_t exp)
{
    uint32_t powerOf10 = 1;
    for (int i = 0; i < exp - 1 ; i++)
        powerOf10 *= 10;

        //utilise pour les divisions et modulos des printInt
    return powerOf10;
}