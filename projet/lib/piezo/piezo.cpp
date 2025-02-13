/*
 * Classe permettant l'utilisation du piezo du robot.
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

#include "piezo.h"


Piezo::Piezo(Pin& groundPin, Pin& signalPin, Timer0& delayTimer, Timer1& timer)
: groundPin_  (&groundPin)
, signalPin_  (&signalPin)
, delayTimer_ (&delayTimer)
, timer_      (&timer) 
{
    groundPin_->setWriteMode();
    groundPin_->write(0);
    signalPin_->setWriteMode();
    // TODO: Ultra mal fait.
    // Devrait dépendre d'un paramètre dans la 
    // construction de Piezo qui dicterait
    // si le mode Toggle ou Normal est activé
    switch (signalPin.getPinNumber())
    {
        case PD5:
            timer_->setToggleOC1A();
            break;
        case PD6:
            timer_->setToggleOC1B();
            break;
        default:
            // Note: Awful parce que ça overwrite e.g. 
            // les settings pour un piezo sur PD5/PD6
            timer_->setNormalPortOpA();
            timer_->setNormalPortOpB();
            break;
    }
    timer_->setClockSelect(Timer1::ClockSelect::clk_64);
    timer_->setInterruptOCIEA(false);
    // Timer1 always in CTC mode
}

Piezo::~Piezo() { signalPin_->write(0); }

float Piezo::convertNoteToFrequency(uint8_t note) // Hertz
{
    return (pow(2, ((note - A4_MIDI_NUMBER) / FREQUENCY_RATIO)) * A4_FREQUENCY);
}

void Piezo::playNote(uint8_t note)
{
    float noteFrequencyHz = convertNoteToFrequency(note);
    timer_->setDurationAUs(Util::ONE_SECOND_US / noteFrequencyHz);
    timer_->startTimer();
}

void Piezo::stopPlaying() { timer_->stopTimer(); }
