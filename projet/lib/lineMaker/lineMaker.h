/*
 * Classe permettant de définir un capteur de ligne.
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

#define S1 4
#define S2 3
#define S3 2
#define S4 1
#define S5 0

#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pin.h"
#include <util/delay.h>

enum class Turn
{
    NONE,
    LEFT,
    RIGHT,
    INTERSECTION
};

class LineMaker
{
public:
    LineMaker(Pin& pin0, Pin& pin1, Pin& pin2, Pin& pin3, Pin& pin4);

    bool isS1();
    bool isS2();
    bool isS3();
    bool isS4();
    bool isS5();

    uint8_t read();

    bool isThereLine();
    bool isThereT();

private:
    Pin* pin0_;
    Pin* pin1_;
    Pin* pin2_;
    Pin* pin3_;
    Pin* pin4_;
};