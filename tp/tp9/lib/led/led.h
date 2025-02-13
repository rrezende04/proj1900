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

#include <avr/io.h>
#include "pin.h"


/*
 * Classe led:
 *      Constructor instanciates "led" object connected to specific pins
 *      Functions allow changing of color.
 * 
*/

class Led
{
public:

    Led(Pin&  positivePin,
        Pin&  negativePin);

    ~Led();

    // Color change functions
    // & Color toggle functions (red <-> green)

    void turnGreen();
    void turnRed();
    void turnOff();
    void toggle();

private:

        //Member attributes
    Pin* positivePin_;
    Pin* negativePin_;
};