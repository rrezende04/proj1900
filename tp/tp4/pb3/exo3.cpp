/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: 
 * PD4, PD5 = PWM pour la roue gauche et droite respectivement.
*/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Initialize vars
const uint8_t delayResolutionUs = 10;
const uint8_t delayResolutionMs = 10;
const uint8_t gTop = 0xFF;

void delayLoopUs(uint8_t nTimes)
{
    for (uint8_t i = 0; i < nTimes; i++)
        _delay_us(delayResolutionUs);
}

void delayLoopMs(uint8_t nTimes)
{
    for (uint8_t i = 0; i < nTimes; i++)
        _delay_ms(delayResolutionMs);
}

void ajustementPwm (uint8_t pourcentagePwm)  
{
    // mise à un des sorties OC1A et OC1B sur comparaison
    // réussie en mode PWM 8 bits, phase correcte
    // et valeur de TOP fixe à 0xFF (mode #1 de la table 16-5
    // page 130 de la description technique du ATmega324PA)
    uint16_t compareValue = pourcentagePwm * gTop / 100;

    OCR1A = compareValue;
    OCR1B = compareValue;

    TCCR1A &= ~(1 << WGM11);                                // xxxx -- 0x
    TCCR1A |= (1 << WGM10);                                 // xxxx -- x1

    TCCR1A |= (1 << COM1A1) | (1 << COM1B1);    // set pwm compare mode : toggle on 
    TCCR1A &= ~(1 << COM1A0) & ~(1 << COM1B0);    // match without hard coding motor function

    TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12) & ~(1 << CS12) & ~(1 << CS10); // xx - 00 0x0
    TCCR1B |= (1 << CS11);                 // xx - xx x1x
    //resultat (TCCR1B[4:3] TCCR1A[1:0]) de 0001 : mode PWM phase correct 8b
    //010 pour indiquer une valeur de prescaler a 8

    TCCR1C = 0;
}

void initializePorts()
{
    cli();                 // prevents interrupts

    DDRA |= (1 << PA2) | (1 << PA3);
    DDRA |= (1 << PA4) | (1 << PA5);

    DDRD |= (1 << PD4) | (1 << PD5);
    DDRD |= (1 << PD6) | (1 << PD7);

    sei();                 // resume interrupt handling
}

int main()
{
    initializePorts();
    ajustementPwm(0);
    delayLoopMs(200);
    ajustementPwm(25);
    delayLoopMs(200);
    ajustementPwm(50);
    delayLoopMs(200);
    ajustementPwm(75);
    delayLoopMs(200);
    ajustementPwm(100);
    delayLoopMs(200);
    ajustementPwm(0);
}
