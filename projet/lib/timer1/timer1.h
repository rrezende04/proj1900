/*
 * Classe permettant l'utilisation de la minuterie Timer1 du robot.
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

class Timer1
{
public:

	enum ClockSelect
	{
		clk_stopped		= (0 << CS12) | (0 << CS11) | (0 << CS10),
		clk_1			= (0 << CS12) | (0 << CS11) | (1 << CS10),
		clk_8			= (0 << CS12) | (1 << CS11) | (0 << CS10),
		clk_64			= (0 << CS12) | (1 << CS11) | (1 << CS10),
		clk_256			= (1 << CS12) | (0 << CS11) | (0 << CS10),
		clk_1024		= (1 << CS12) | (0 << CS11) | (1 << CS10),
		ext_clk_falling = (1 << CS12) | (1 << CS11) | (0 << CS10),
		ext_clk_rising	= (1 << CS12) | (1 << CS11) | (1 << CS10),
	};

private:

	ClockSelect clock_;

	uint16_t    OCR1A_cycle_;
	uint16_t    OCR1B_cycle_;

public:

	Timer1();

	~Timer1();

	void setClockSelect(ClockSelect clock);

	void setToggleOC1A();
	void setToggleOC1B();
	void setNormalPortOpA();
	void setNormalPortOpB();

	const uint16_t& getOCR1A() const;
	const uint16_t& getOCR1B() const;

	void setInterruptOCIEA(bool activated);
	void setInterruptOCIEB(bool activated);

	void setDurationMs(uint16_t durationA_ms, uint16_t durationB_ms);
	void setDurationAMs(uint16_t durationA_ms);
	void setDurationBMs(uint16_t durationB_ms);

	void setDurationUs(uint16_t durationA_us, uint16_t durationB_us);
	void setDurationAUs(uint16_t durationA_us);
	void setDurationBUs(uint16_t durationB_us);

	void startTimer();
	void stopTimer();

private:

	void setOCR1A(uint16_t cycleA);
	void setOCR1B(uint16_t cycleB);
};