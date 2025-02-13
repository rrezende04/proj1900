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

#pragma once

#include <avr/io.h>
#include <math.h>
#include "pin.h"
#include "timer1.h"
#include "util.h"
#include "timer0.h"

class Piezo
{
public:

    const uint8_t A4_MIDI_NUMBER = 69;
    const float A4_FREQUENCY = 440;
    const float FREQUENCY_RATIO = 12;

    Piezo(Pin& groundPin,
          Pin& signalPin,
          Timer0& delayTimer,
          Timer1& timer);

    ~Piezo();

    void playNote(uint8_t note);

    void stopPlaying();

private: 

    float convertNoteToFrequency(uint8_t note);

    Pin*    groundPin_;
    Pin*    signalPin_;
    Timer0* delayTimer_;
    Timer1* timer_;
    
};
