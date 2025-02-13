/*
 * Classe permettant d'utiliser un bouton mécanique
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
#include <avr/interrupt.h>
#include "pin.h"


/*
 * Classe Button:
 *      Constructor instanciates "Button" object connected to a specific pin,
 *      PD2 by default
*/

class Button
{
public:

        // Constructor & Destructor
    Button(Pin& pin                   ,
           bool isInverseLogic        ,
           bool useExternalInterrupts);
    ~Button();

        // Interrupt register setting
    void enableExternalInterrupts ();
    void disableExternalInterrupts();

        // Edge catch mode setting
    void catchRisingEdge ();
    void catchFallingEdge();
    void catchAll        ();

        // Get button state functions
    bool isPressed ();
    bool isReleased();
    
private:

        // Private member functions
    bool isMaskCorresponding   ();
    bool isMaskNotCorresponding();

    // Member attributes
        // Constants
    static const uint8_t DEBOUNCE_TIME_MS = 10;

        // Variable attributes
    Pin*    pin_                   ;
    uint8_t pinNumber_             ;
    bool    isInverseLogic_        ;
    bool    pressedOnce_    = false;

        // Register pertinent attributes
    uint8_t externalInterruptRequestEnable_;
    uint8_t interruptSenseControlMask0_    ;
    uint8_t interruptSenseControlMask1_    ;
};