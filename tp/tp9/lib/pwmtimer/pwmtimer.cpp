/*
 * Classe permettant la génération de signaux PWM par la minuterie 2.
 * Le PWM A sort sur PD7. Le PWM B sort sur PD6
 *
 * Ecole Polytechnique de Montreal
 * Departement de genie informatique
 * Cours inf1900
 *
 * Eliott Bonnefoy, Gabriel Bruyere, Thierry Poulin, Renato Rezende
 * 2024
 *
 * Code qui n'est sous aucune license.
 *
 */

#include <avr/io.h>
#include "pwmtimer.h"
#include "debug.h"

void PWMTimer::stopTimer()
{
    TCCR2B &= ~(1 << CS22) & ~(1 << CS21) & ~(1 << CS20);
}

void PWMTimer::startTimer()
{
    // Prescaler set to 8
    // Good enough for the motors
    // Don't hog the CPU
    TCCR2B &= ~(1 << CS22);
    TCCR2B |=  (1 << CS21);
    TCCR2B &= ~(1 << CS20);
}

PWMTimer::PWMTimer(uint8_t powerA, uint8_t powerB)
{
    OCR2A = powerA, OCR2B = powerB;

    TCCR2A |=  (1 << WGM20);   // Mode 1: PWM phase correct
    TCCR2A &= ~(1 << WGM21);
    TCCR2B &= ~(1 << WGM22);   // TOP = 0xFF

    TCCR2A |=  (1 << COM2A1) |  (1 << COM2B1);  // 
    TCCR2A &= ~(1 << COM2A0) & ~(1 << COM2B0);  // 

    // TODO: Utiliser Port?
    DDRD |= (1 << PD6) | (1 << PD7);

    stopTimer(); // Timer stopped

    // TIMSK2 &= ~(1 << OCIE2B) & ~(1 << OCIE2A);
}

PWMTimer::PWMTimer() : PWMTimer(0, 0) {}

PWMTimer::~PWMTimer()
{
    OCR2A = 0, OCR2B = 0;

    TCCR2A &= ~(1 << WGM21)  & ~(1 << WGM20);   // Mode 3: Fast PWM
    TCCR2B &= ~(1 << WGM22);                    // TOP = 0xFF

    TCCR2A &= ~(1 << COM2A1) & ~(1 << COM2B1);  // Clear OC2X on match
    TCCR2A &= ~(1 << COM2A0) & ~(1 << COM2B0);  // Set OC2X at BOTTOM

    stopTimer();

    TIMSK2 &= ~(1 << OCIE2B) & ~(1 << OCIE2A);
}

void PWMTimer::setPowerA(uint8_t power) { OCR2A = power; }
void PWMTimer::setPowerB(uint8_t power) { OCR2B = power; }
void PWMTimer::setPower(uint8_t pin, uint8_t power)
{
    switch (pin)
    {
    case PD7:
        setPowerA(power);
        break;
    case PD6:
        setPowerB(power);
        break;
    default:
        // DEBUG_PRINT("Cannot output PWM signals to pin")
        break;
    }
}

void PWMTimer::setPower(Pin& pin, uint8_t power)
{
    if(pin.getPortID() == PortID::D)
        setPower(pin.getPinNumber(), power);
    /*
    else
        DEBUG_PRINT("Cannot output PWM signals to pin")
    */
}
