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
#include "util.h"

class Motor
{
public:
    // Constructor with pin references
    Motor(Pin& pinPWM, Pin& pinControl, PWMTimer& pwm);
    ~Motor();

    void turnOff();
    void setPower(uint8_t power);
    void setDirMode(Util::Dir direction);
    void invertDirection(uint8_t power);

    
    // Static attributes
    static constexpr uint8_t LOW_POWER          = 0x6A;
    static constexpr uint8_t PRESENTATION_POWER = 0x7B;
    static constexpr uint8_t MEDIUM_POWER       = 0x80;
    static constexpr uint8_t MAX_POWER          = 0xFF;

private:
    // Member attributes
    Pin* pinPWM_;
    Pin* pinControl_;
    PWMTimer* pwm_;
};
