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

#include "motor.h"
#include "debug.h"

Motor::Motor(Pin& pinPWM, 
             Pin& pinControl, 
             PWMTimer& pwm)
: pinPWM_    (&pinPWM)
, pinControl_(&pinControl)
, pwm_       (&pwm)
{
    pinControl_->setWriteMode();
    pinPWM_->setWriteMode();
    turnOff();
    setDirMode(Util::Dir::FORWARD);
    pwm_->startTimer();
}

Motor::~Motor() { turnOff(); }

    // Power is handled by pwmTimer attribute
void Motor::turnOff()               { setPower(0); }
void Motor::setPower(uint8_t power) { pwm_->setPower(*pinPWM_, power); }

    // Control pin is responsible for direction
void Motor::setDirMode(Util::Dir direction)
{
    switch (direction)
    {
        case Util::Dir::FORWARD:
            pinControl_->write(0);
            break;
        case Util::Dir::BACKWARD:
            pinControl_->write(1);
            break;
    }
}

void Motor::invertDirection(uint8_t power)
{ 
    pinControl_->toggle(); 
    setPower(power);
}
