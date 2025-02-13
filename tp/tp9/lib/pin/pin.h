/*
 * Classe permettant de définir une pin.
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

enum class PortID { A, B, C, D };

class Pin
{
public:
        // Default = true because we mostly **control** devices
    Pin(PortID portID, uint8_t pinNumber, bool writeMode = true);
    ~Pin();

        // The microcontroller can only read this pin
    void setReadMode();
        // Returns the bit read from this pin
    bool read() const;

        // The microcontroller can only write to this pin
    void setWriteMode();
        // Write bit to this pin
    void write(bool bit);
        // Toggle the value of this pin
    void toggle();

    uint8_t getMask()      const;
    uint8_t getPinNumber() const;
    PortID  getPortID()    const;
    
    

    // TODO: to_str for DEBUG_PRINT

private:
    const PortID  PORT_ID_;
    const uint8_t MASK_;
    const uint8_t PIN_NUMBER_;
    const bool    OLD_DDR_;

    volatile uint8_t* getDDR();
        // Not volatile nor pointer because only for reading
    uint8_t           getPIN() const;
    volatile uint8_t* getPORT();

    void setDDR(bool bit);
};