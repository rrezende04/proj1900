#include "timer0.h"

Timer0::Timer0()  { startTimer(); }
Timer0::~Timer0() { stopTimer();  }

void Timer0::enableAmberInterrupts() 
{ 
    TCNT0 = 0;
    TIMSK0 |=  (1 << OCIE0A); 
}

void Timer0::enableDelayInterrupts()  { TIMSK0 |=  (1 << OCIE0B); }

void Timer0::disableAmberInterrupts() { TIMSK0 &= ~(1 << OCIE0A); }
void Timer0::disableDelayInterrupts() { TIMSK0 &= ~(1 << OCIE0B); }

void Timer0::startTimer()
{
    // Reset counter
    TCNT0 = 0;

    // Mode : PWM, Phase Correct
    TCCR0B &= ~(1 << WGM02);
    TCCR0A &= ~(1 << WGM01);
    TCCR0A |=  (1 << WGM00);

    // OC0A & OC0B (PB3, PB4) disconnected 
    // As PB4 is used for writing program to flash
    TCCR0A &= ~(1 << COM0A1) & ~(1 << COM0A0)
           &  ~(1 << COM0B1) & ~(1 << COM0B0);

    // Ensures interrupts are disabled for compare matches
    TIMSK0 &= ~(1 << OCIE0A) & ~(1 << OCIE0B);

    // Sets compare values
    // What will determine if they are active is
    // wether their interrupts are enabled.
    OCR0A = AMBER_COMPARE_VALUE;
    OCR0B = DELAY_COMPARE_VALUE;

    // Prescaler : 256 
    // 1024 does not produce amber &
    // still allows to count large delays
    TCCR0B |=  (1 << CS01) | (1 << CS00);
    TCCR0B &= ~(1 << CS02);
}

void Timer0::stopTimer()
{
    // Reset counter
    TCNT0 = 0;

    // Prescaler : 0 (stops counting) 
    TCCR0B &= ~(1 << CS02) & ~(1 << CS01) & ~(1 << CS00);
}