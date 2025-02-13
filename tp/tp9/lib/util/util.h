/*
 * Classe permettant l'utilisation de methodes et constantes a usage general
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
#include <util/delay.h>
#include "timer1.h"


class Util
{
public:

	static const uint8_t  CENT          = 100;
	static const uint32_t CPU_FREQUENCY = 8000000;
	static const uint16_t ONE_SECOND_MS = 1000;
	static const uint32_t ONE_SECOND_US = 1000000;

private:

	static const uint8_t  CLK_1    = 1;
	static const uint8_t  CLK_8    = 8;
	static const uint8_t  CLK_64   = 64;
	static const uint16_t CLK_256  = 256;
	static const uint16_t CLK_1024 = 1024;

public:

	static uint16_t convertMsToCycles(uint16_t milisecond, const Timer1::ClockSelect& clock);

	static uint16_t convertUsToCycles(uint16_t microsecond, const Timer1::ClockSelect& clock);

	static void wait_ms(int time);

};