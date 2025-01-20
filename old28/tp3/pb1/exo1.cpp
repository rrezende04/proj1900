/*
 * Noms: Thierry Poulin & Renato Rezende
 *
 * Description: Eteindre une LED graduellement sur une duree
 *              de temps donnee, en utilisant un signal PWM logiciel
 *              Nous utilisons les ports PA1 (+) et PA0 (-) en sortie pour controler la LED
 *              Lorsqu'elle est allumee, la LED sera toujours de couleur rouge, i.e. 0x10              
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

const uint16_t frequency = 1000; // en n par secondes
const uint16_t periodUs = 1000; // periode b en microsecondes
const uint8_t delayResolutionUs = 10; // taille de delai
const uint16_t numberOfCycles = 3000; // periode de 1ms donc 3 secondes = 3000 cycles
//const uint8_t cycleTime = 1;

void turnLedOff()        // 00
{
    PORTA &= ~(1 <<PA1); // 0x
    PORTA &= ~(1 <<PA0); // x0
}

void turnLedGreen()     // 01
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA |= (1 << PA0);  // x1
}

void turnLedRed()        // 10
{
    PORTA |= (1 << PA1);   // 1x
    PORTA &= ~(1 << PA0);  // x0
}

void delayUs(uint16_t Us)
{
    uint16_t cycles = Us / delayResolutionUs; //division entiere pour trouver combien de fois executer le delai
    for (uint16_t i = 0; i < cycles; i++) _delay_us(delayResolutionUs);
}

void PWM(uint8_t dutyCyclePercent)
{
    // On trouve la duree en Us de tension haute (LED allumee) et basse (LED eteinte)
    uint16_t onTimeUs = dutyCyclePercent * periodUs / 100;
    uint16_t offTimeUs = 1000 - onTimeUs; // complement

    turnLedRed();
    delayUs(onTimeUs);

    turnLedOff();
    delayUs(offTimeUs);
}

void slowTurnLedOff()
{
    for (uint32_t cyclesPWM = numberOfCycles ; cyclesPWM > 0; cyclesPWM--)
    {
        uint8_t percentPWM = cyclesPWM * 100 / numberOfCycles; // multiplication par 100 afin d'obtenir un pourcentage
        PWM(percentPWM);
    }
}

int main()
{
    // Setup entrees et sortie
    DDRA |= (1 << PA0) | (1 << PA1);
    
    turnLedOff();  
    turnLedRed(); //on allume la LED (pas obligatoire)
    slowTurnLedOff();
    
    
    return 0;
}