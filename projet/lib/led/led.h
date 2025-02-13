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

#include <util/delay.h>
#include <avr/io.h>
#include "pin.h"
#include "timer0.h"


/*
 * Classe led:
 *      Constructor instanciates "led" object connected to specific pins
 *      Functions allow changing of color.
 * 
*/

class Led
{
public:
    enum class Colour {OFF, GREEN, RED, AMBER};

    Led(Pin&    positivePin,
        Pin&    negativePin,
        Timer0& delayTimer);
    ~Led();

    // Color change functions
    // & Color toggle functions (red <-> green)
    void turn(Colour colour);
    void turnAmberMs(uint16_t durationMs);
    // Call this function only if ISR(TIMER0_COMPA_vect) is defined
    void setAmberViaTimer();
    void toggle();

private:
    // STATIC CONSTANTS
    static constexpr uint8_t AMBER_RESOLUTION_TIME_MS = 7;
    static constexpr uint8_t AMBER_GREEN_TIME_MS      = 4;
    static constexpr uint8_t AMBER_RED_TIME_MS        = AMBER_RESOLUTION_TIME_MS - AMBER_GREEN_TIME_MS;

    static_assert(1 < AMBER_RESOLUTION_TIME_MS, "AMBER_RESOLUTION_TIME_MS needs to be at least two.");
    static_assert(0 < AMBER_GREEN_TIME_MS && AMBER_GREEN_TIME_MS < AMBER_RESOLUTION_TIME_MS,
                     "AMBER_GREEN_TIME_MS needs to be positive and less than AMBER_RESOLUTION_TIME_MS.");

    // MEMBER ATTRIBUTES
    Pin*    positivePin_;
    Pin*    negativePin_;
    Timer0* delayTimer_;
};