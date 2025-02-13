/*
 * Definitions de fonctions permettant l'utilisation de fonctions de deverminage
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

#ifdef DEBUG
#define DEBUG(x) debugPrint(x)
#define DEBUG_STR(x,y) debugPrintStr(x,y)
#else
#define DEBUG(x) do {} while (0)
#define DEBUG_STR(x,y) do {} while (0)


#endif

#pragma once

#include <avr/io.h>
#include "communicator.h"


void     debugPrint       (char  myChar);
void     debugPrintStr    (char* ptrStr, uint8_t length);
     
void     debugPrint       (uint32_t myInt);
     
uint32_t debugGetPowerOf10(uint8_t  exp);
    

