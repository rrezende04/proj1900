/*
 * Classe permettant d'utiliser le capteur infrarouge SHARP GP2Y0A21YK0F.
 * Basee sur la documentation de SHARP : https://global.sharp/products/device/lineup/data/pdf/datasheet/gp2y0a21yk_e.pdf
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
 * 
*/

#pragma once

#include <avr/io.h>
#include "can.h"
#include "pin.h"
#include "util.h"

class Infrared
{
public:
    Infrared(Pin pin); // TODO: Pas Pin, mais const Pin&, et pointeur gardé en attribut
    // TODO? void setArefVolts(float Aref_V);

    // Used to access the ADC via the infraRed class
    uint8_t read();

    // Returns distance read by infrared sensor
    float readDistanceCm();

    // 38.3 ms +/- 9.6 ms between reads according to documentation
    static constexpr uint8_t READ_DELAY_MS = 50;

private:
    can converter_ = can();
    Pin pin_;
    float Aref_V_ = 3.2F; // TODO: Read this at execution?
};
