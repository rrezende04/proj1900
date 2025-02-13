/*
        Declarations des fonctions afin de charger en
    memoire externe le programme envoye par progmeme.
*/

#include <avr/io.h>
#include "memoire_24.h"
#include "communicator.h"

void     writeToMemoryLoop (uint16_t programSize);
uint16_t installProgram    ();
uint16_t readProgramSize   ();



