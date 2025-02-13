/*
 * Classe permettant l'utilisation de moteurs
 * du robot.
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
#include "pwmtimer.h"
#include "pin.h"

class Motor
{
public:
    Motor(Pin& pinPWM, Pin& pinControl, PWMTimer& pwm);
    Motor(PortID portID, uint8_t pinPWM, uint8_t pinControl, PWMTimer& pwm);
    ~Motor();

    void turnOff();
    void setForwardMode ();
    void setBackwardMode();
    void setPower(uint8_t power);
    
    // Static attributes
    static const uint8_t LOW_POWER    = 0x40;
    static const uint8_t MEDIUM_POWER = 0x80;
    static const uint8_t MAX_POWER    = 0xFF;

    // Member attributes
private:
    Pin pinPWM_;
    Pin pinControl_;
    PWMTimer pwm_;
};
