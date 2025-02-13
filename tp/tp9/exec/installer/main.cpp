/*
 * Noms: Gabriel Bruyere, Eliott Bonnefoy, Thierry Poulin & Renato Rezende
 * Description: Le code telecharge les instructions donnees par USB (RS232).
 * 
 * Date: 7 octobre 2024
 */

#define F_CPU 8000000

#include <string.h>
#include "installer.h"
#include "pin.h"
#include "debug.h"
#include "button.h"
#include "memoire_24.h"

volatile bool wait = false;

ISR(INT0_vect)
{
    wait = true;
}

int main()
{
    installProgram();

    // Uncomment to test using RS232 (shows bytes on screen)


    // uint8_t container[1];
    // Memoire24CXXX mem;
    // uint16_t progSize = installProgram();
    // Pin pa2 = Pin(PortID::A, 2);
    // pa2.setReadMode();

    // while(! wait) { wait = pa2.read(); }

    // for (uint16_t i = 0; i < progSize - 2; i++)
    // {
    //     mem.lecture(i, container);
    //     DEBUG((uint32_t(container[0])));
    //     if (i % 2) { DEBUG((char(0))); }
    // }
}