/*
 * Noms: Gabriel Bruyere, Eliott Bonnefoy, Thierry Poulin & Renato Rezende
 * Description: Le code interprète les instructions téléversées par USB (RS232)
 *              et exécute les instructions.
 *
 * Date: 7 octobre 2024
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include "interpretor.h"
#include "debug.h"
#include "pin.h"
#include "pwmtimer.h"
#include "communicator.h"
#include "motor.h"
#include "memoire_24.h"

int main() 
{
    const uint32_t MAX_32_BITS_MINUS_ONE = -2;
    static const uint8_t  DELAY_RESOLUTION      = 10;

    Interpretor   interpretor = {};
    Memoire24CXXX mem;

    uint8_t operation[1];
    uint8_t operand[1];

    uint32_t i = 0;
    while (i < MAX_32_BITS_MINUS_ONE)
    {
        mem.lecture(i, operation);
        i++;

        _delay_ms(DELAY_RESOLUTION);

        mem.lecture(i, operand);
        i++;

        interpretor.instructionSwitch(*operation, *operand, i);
    }

    return 0;
}
