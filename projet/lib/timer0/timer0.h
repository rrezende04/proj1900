/*
 * Classe permettant l'utilisation de la minuterie Timer0 du robot.
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

class Timer0
{
public:

     Timer0();
    ~Timer0();

    void enableAmberInterrupts();
    void enableDelayInterrupts();

    void disableAmberInterrupts();
    void disableDelayInterrupts();

private:

    static constexpr uint8_t TIMER_MAX_VALUE = 255;
    static constexpr uint8_t AMBER_COMPARE_VALUE = 0xA0;
    static constexpr uint8_t DELAY_COMPARE_VALUE = TIMER_MAX_VALUE / 2;

	void startTimer();
	void stopTimer();
};