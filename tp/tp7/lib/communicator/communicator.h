/*
 * Definitions de fonction permettant l'utilisation du protocole de communication
 * RS-232 via le robot.
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

    // Register Initialization function
void setUsartRegisters();

    // Communication functions
void sendString(uint8_t length, char* stringPtr);
void sendChar  (uint8_t aChar);

unsigned char receiveChar();



