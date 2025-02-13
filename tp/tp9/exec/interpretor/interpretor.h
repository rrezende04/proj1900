#pragma once

#include <avr/io.h>
#include "pin.h"
#include "led.h"
#include "communicator.h"
#include "motor.h"
#include "pwmtimer.h"
#include "piezo.h"
#include "timer1.h"

class Interpretor
{
private:
    // Déclaration des pins qui seront utilisés:
    // Led :
    Pin pinA0 = Pin(PortID::A, 0);
    Pin pinA1 = Pin(PortID::A, 1);

    // Moteurs :
    Pin pinPWMRight     = Pin(PortID::D, 7);
    Pin pinControlRight = Pin(PortID::C, 3);
    Pin pinPWMLeft      = Pin(PortID::D, 6);
    Pin pinControlLeft  = Pin(PortID::C, 2);

    // Timer PWM pour les moteurs :
    PWMTimer pwm = PWMTimer();

    Led led = Led(pinA1, pinA0);

    Motor motorRight = Motor(pinPWMRight, pinControlRight, pwm);
    Motor motorLeft  = Motor(pinPWMLeft,  pinControlLeft,  pwm);

    // Piezo
    Pin    pinGroundPiezo = Pin(PortID::D, 3);
    Pin    pinPWMPiezo    = Pin(PortID::D, 5);
    Timer1 timerPiezo     = Timer1();
    Piezo  piezo          = Piezo(pinGroundPiezo, pinPWMPiezo, timerPiezo);

    enum Commandes
    {
        dbt  = 0x01,
        att  = 0x02,
        dal  = 0x44,
        det  = 0x45,
        sgo  = 0x48,
        sar  = 0x09,
        mar0 = 0x60,
        mar1 = 0x61, // nécessaire ?
        mav  = 0x62,
        mre  = 0x63,
        trd  = 0x64,
        trg  = 0x65,
        dbc  = 0xC0,
        fbc  = 0xC1,
        fin  = 0xFF
    };

             bool debut            = false;
         uint16_t loopStartAddress = 0;
          uint8_t loopCounter      = 0;
    const uint8_t DELAY_SCALER     = 25;
    const uint8_t TURN_90_TIME     = 44;

    void att_f(uint8_t operande);
    void dal_f(uint8_t operande);
    void det_f();
    void sgo_f(uint8_t operande);
    void sar_f();
    void mar_f();
    void mav_f(uint8_t operande);
    void mre_f(uint8_t operande);
    void trd_f();
    void trg_f();
    void dbc_f(uint8_t operande, uint32_t adresse);
    void fbc_f(uint32_t& adresse);
    void fin_f(uint32_t& adresse);

public:
    Interpretor() = default;
    void instructionSwitch(uint8_t instruction, uint8_t operande, uint32_t& adresse);
};
