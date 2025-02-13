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

#pragma once

#include <avr/io.h>
#include "pin.h"

class PWMTimer
{
public:

    PWMTimer();
    PWMTimer(uint8_t powerA, uint8_t powerB);

    ~PWMTimer();

    // PD7
    void setPowerA(uint8_t power);
    // PD6
    void setPowerB(uint8_t power);
    // To change power output from motor class
    // PortID is always D, so unspecified
    void setPower(uint8_t pin, uint8_t power);
    void setPower(Pin&    pin, uint8_t power);

    void startTimer();
    void stopTimer();

    void activateInterruptsA();
    void activateInterruptsB();
    void deactivateInterruptsA();
    void deactivateInterruptsB();
};
