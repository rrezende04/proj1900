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


#include "timer1.h"
#include "util.h"


Timer1::Timer1()
{
	TCNT1 = 0;

	OCR1A_cycle_ = 0;
	OCR1A = 0;

	OCR1B_cycle_ = 0;
	OCR1B = 0;

	TCCR1A = 0;

	setClockSelect(ClockSelect::clk_stopped);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= ClockSelect::clk_stopped;

	TCCR1C = 0;
	TIMSK1 = 0;
}

Timer1::~Timer1()
{
	TCNT1 = 0;
	OCR1A = 0;
	OCR1B = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	TIMSK1 = 0;
}

void Timer1::setToggleOC1A() { TCCR1A &= ~(1 << COM1A1); TCCR1A |= (1 << COM1A0); }

void Timer1::setToggleOC1B() { TCCR1A &= ~(1 << COM1B1); TCCR1B |= (1 << COM1B0); }

void Timer1::setNormalPortOpA() { TCCR1A &= ~(1 << COM1A1) & ~(1 << COM1A0); }

void Timer1::setNormalPortOpB() { TCCR1A &= ~(1 << COM1B1) & ~(1 << COM1B0); }

void Timer1::setClockSelect(ClockSelect clock)
{
	clock_ = clock;
}

const uint16_t& Timer1::getOCR1A() const
{
	return OCR1A_cycle_;
}

const uint16_t& Timer1::getOCR1B() const
{
	return OCR1B_cycle_;
}

void Timer1::setOCR1A(uint16_t cycleA)
{
	OCR1A_cycle_ = cycleA;
}

void Timer1::setOCR1B(uint16_t cycleB)
{
	OCR1B_cycle_ = cycleB;
}

void Timer1::setInterruptOCIEA(bool activated)
{
	if (activated)
		TIMSK1 |= (1 << OCIE1A);
	else
		TIMSK1 &= ~(1 << OCIE1A);
}

void Timer1::setInterruptOCIEB(bool activated)
{
	if (activated)
		TIMSK1 |= (1 << OCIE1B);
	else
		TIMSK1 &= ~(1 << OCIE1B);
}

void Timer1::setDurationMs(uint16_t durationA_ms, uint16_t durationB_ms)
{
	setOCR1A(Util::convertMsToCycles(durationA_ms, clock_));
	setOCR1B(Util::convertMsToCycles(durationB_ms, clock_));
	if (OCR1A_cycle_ <= TCNT1)
		TCNT1 = 0;
	OCR1A = OCR1A_cycle_;
	OCR1B = OCR1B_cycle_;
}

void Timer1::setDurationAMs(uint16_t durationA_ms)
{
	setOCR1A(Util::convertMsToCycles(durationA_ms, clock_));
	if (OCR1A_cycle_ <= TCNT1)
		TCNT1 = 0;
	OCR1A = OCR1A_cycle_;
}

void Timer1::setDurationBMs(uint16_t durationB_ms)
{
	setOCR1B(Util::convertMsToCycles(durationB_ms, clock_));
	OCR1B = OCR1B_cycle_;
}

void Timer1::setDurationUs(uint16_t durationA_us, uint16_t durationB_us)
{
	setOCR1A(Util::convertUsToCycles(durationA_us, clock_));
	setOCR1B(Util::convertUsToCycles(durationB_us, clock_));
	if (OCR1A_cycle_ <= TCNT1)
		TCNT1 = 0;
	OCR1A = OCR1A_cycle_;
	OCR1B = OCR1B_cycle_;
}

void Timer1::setDurationAUs(uint16_t durationA_us)
{
	setOCR1A(Util::convertUsToCycles(durationA_us, clock_));
	if (OCR1A_cycle_ <= TCNT1)
		TCNT1 = 0;
	OCR1A = OCR1A_cycle_;
}

void Timer1::setDurationBUs(uint16_t durationB_us)
{
	setOCR1B(Util::convertUsToCycles(durationB_us, clock_));
	OCR1B = OCR1B_cycle_;
}

void Timer1::startTimer() 
{
	TCNT1 = 0;
	TCCR1B |= clock_;
}

void Timer1::stopTimer()
{
	TCCR1B = ClockSelect::clk_stopped;
	TCCR1B |= (1 << WGM12);
}
