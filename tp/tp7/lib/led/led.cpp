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


#include "led.h"

    // Constructor
Led::Led(Pin&  positivePin,
         Pin&  negativePin)
    //member attributes
: positivePin_ (positivePin)
, negativePin_ (negativePin)
{
    positivePin_.setWriteMode();
    negativePin_.setWriteMode();
}

    // Destructor
Led::~Led()
{
    turnOff();
}

void Led::turnGreen()
{
    positivePin_.write(0);
    negativePin_.write(1);
}

void Led::turnRed()
{
    positivePin_.write(1);
    negativePin_.write(0);
}

void Led::turnOff()
{
    positivePin_.write(0);
    negativePin_.write(0);
}

void Led::toggle()
{
    positivePin_.toggle();
    negativePin_.toggle();
}