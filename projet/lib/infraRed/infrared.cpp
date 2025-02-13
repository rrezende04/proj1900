#include "infrared.h"

// Public
Infrared::Infrared(Pin pin)
: pin_(pin)
{}

uint8_t Infrared::read()
{ return uint8_t(converter_.lecture(pin_.getPinNumber()) >> 2); }

float Infrared::readDistanceCm()
{
    // These come from the documentation and a linear approximation of
    // the relationship between 1/d and v, in cm^-1 and volts, respectively:
    //      d = CM_V_CONVERSION_CST / (v - INFINITY_VOLTAGE)
    // Also, the reading A from the ADC and the voltage v share this relationship:
    //      v = A * Aref_V / 256

    // Known point, ( 1/10cm, 2.3V ) (see Fig. 2 of page 5 in documentation )
    static constexpr float KNOWN_POINT_DISTANCE = 15.0F;
    static constexpr float KNOWN_POINT_VOLTAGE  = 1.625F;

    // v = CM_V_CONVERSION_CST * ( 1 / d ) + INFINITY_VOLTAGE
    // y Intercept value in the linear relation 
    static constexpr float INFINITY_VOLTAGE = 0.0F;
    // Slope of the linear equation
    static constexpr float CM_V_CONVERSION_CST = (KNOWN_POINT_VOLTAGE - INFINITY_VOLTAGE) * KNOWN_POINT_DISTANCE;

    float v = (float)read() * Aref_V_ / 256.0F;

    return CM_V_CONVERSION_CST / (v - INFINITY_VOLTAGE);
}
