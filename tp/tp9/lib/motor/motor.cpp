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

Motor::Motor(     Pin& pinPWM    , 
                  Pin& pinControl, 
             PWMTimer& pwm       )

: pinPWM_    (&pinPWM)
, pinControl_(&pinControl)
, pwm_       (&pwm)
{
    pinControl_->setWriteMode();
    pinPWM_->setWriteMode();
    setPower(0);
    setForwardMode();
    pwm_->startTimer();
}

/*
Motor::Motor(  PortID  portID    ,
              uint8_t  pinPWM    ,
              uint8_t  pinControl,
             PWMTimer& pwm       )

: pinPWM_    (Pin(portID, pinPWM))
, pinControl_(Pin(portID, pinControl))
, pwm_       (pwm)
{
    setForwardMode();
}
*/

Motor::~Motor() { setPower(0); }

    // Power is handled by pwmTimer attribute
void Motor::turnOff()               { setPower(0); }
void Motor::setPower(uint8_t power) { pwm_->setPower(*pinPWM_, power); }

    // Control pin is responsible for direction
void Motor::setBackwardMode() { pinControl_->write(1); }
void Motor::setForwardMode()  { pinControl_->write(0); }
