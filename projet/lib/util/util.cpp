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


#include "util.h"

uint16_t Util::convertMsToCycles(uint16_t millisecond, const Timer1::ClockSelect& clock)
{
	uint16_t div = 0;
	switch (clock)
	{
	case Timer1::ClockSelect::clk_1:
		div = CLK_1;
		break;
	case Timer1::ClockSelect::clk_8:
		div = CLK_8;
		break;
	case Timer1::ClockSelect::clk_64:
		div = CLK_64;
		break;
	case Timer1::ClockSelect::clk_256:
		div = CLK_256;
		break;
	case Timer1::ClockSelect::clk_1024:
		div = CLK_1024;
		break;
	default:
		div = CLK_1;
		break;
	}

	return ((CPU_FREQUENCY / ONE_SECOND_MS) * millisecond / div);
}

uint16_t Util::convertUsToCycles(uint16_t microsecond, const Timer1::ClockSelect& clock)
{
	uint16_t div = 0;
	switch (clock)
	{
	case Timer1::ClockSelect::clk_1:
		div = CLK_1;
		break;
	case Timer1::ClockSelect::clk_8:
		div = CLK_8;
		break;
	case Timer1::ClockSelect::clk_64:
		div = CLK_64;
		break;
	case Timer1::ClockSelect::clk_256:
		div = CLK_256;
		break;
	case Timer1::ClockSelect::clk_1024:
		div = CLK_1024;
		break;
	default:
		div = CLK_1;
		break;
	}

	return ((CPU_FREQUENCY / ONE_SECOND_US) * microsecond / div);
}

void Util::wait_ms(int time)
{
	for(int i = 0; i < time; i++)
		_delay_ms(1);
}
