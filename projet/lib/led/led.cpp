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
Led::Led(Pin&    positivePin,
         Pin&    negativePin,
         Timer0& delayTimer)
    //member attributes
: positivePin_ (&positivePin)
, negativePin_ (&negativePin)
, delayTimer_  (&delayTimer)
{
    positivePin_->setWriteMode();
    negativePin_->setWriteMode();
}

    // Destructor
Led::~Led()
{
    turn(Colour::OFF);
    positivePin_ = nullptr;
    negativePin_ = nullptr;
}

void Led::turn(Colour colour)
{
    if (colour == Colour::AMBER)
        setAmberViaTimer();
    else
    {
        delayTimer_->disableAmberInterrupts();

        positivePin_->write((uint8_t)colour >> 1);
        negativePin_->write((uint8_t)colour %  2);
    }
}

// Uses delays (blocking function)
void Led::turnAmberMs(uint16_t duration) 
{
	uint16_t cycles = duration / AMBER_RESOLUTION_TIME_MS;
    turn(Colour::RED);
	for (uint16_t i = 0; i < cycles; i++)
	{
		toggle();
		_delay_ms(AMBER_GREEN_TIME_MS);
		toggle();
		_delay_ms(AMBER_RED_TIME_MS);
	}
}

// TODO: factour out into timer0.
void Led::setAmberViaTimer()
{
    turn(Colour::RED);
    delayTimer_->enableAmberInterrupts();
}

void Led::toggle()
{
    positivePin_->toggle();
    negativePin_->toggle();
}