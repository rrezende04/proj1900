/*
 * Classe permettant l'utilisation de DEL(s) a usage general
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

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include "pin.h"


/*
 * Classe Button:
 *      Constructor instanciates "Button" object connected to a specific pin,
 *      PD2 by default
*/

class Button
{
public:
    Button();

    Button(Pin&    pin,
           uint8_t  buttonMask,
           bool     isInverseLogic);

    ~Button();

    void enableExternalInterrupts();
    void disableExternalInterrupts();

    void catchRisingEdge();
    void catchFallingEdge();
    void catchAll();

    bool isPressed();
    bool isReleased();

private:
    //Member attributes
    const uint8_t DEBOUNCE_TIME_MS = 10;
    const uint8_t DEFAULT_BUTTON_MASK = (1 << PD2);
          Pin pin_;
          uint8_t pinNumber_;
          uint8_t buttonMask_;
          bool    isInverseLogic_;
          bool    pressedOnce;

          bool isMaskCorresponding();
          bool isMaskNotCorresponding();
};