/*
 * Noms: Gabriel Bruyere, Eliott Bonnefoy, Thierry Poulin & Renato Rezende
 * Description: Fichier de test des fonctions de la librairie statique.
 * 
 * Date: 7 octobre 2024
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "led.h"
#include "communicator.h"
#include "motor.h"
#include "pwmtimer.h"
#include "debug.h"

int main()
{
    // LED TESTS
    /*
    Led led = Led(Port(PORTA, DDRA), PA0, PA1);
    led.turnGreen();
    for(;;)
    return 0;
    */
    
    // COMMUNICATOR TESTS
    /*
    Communicator com = Communicator(DDRD, PORTD, PD0);
    uint8_t myChar = 'a';
    com.sendChar(myChar);
    for(;;) {}
    return 0;
    */

    // PWMTIMER + MOTOR TESTS
    
    PWMTimer pwm = PWMTimer(0x80, 0x10);
    Pin pinPWM(PortID::D, 7);
    Pin pinControl(PortID::D, 5);
    Motor motorLeft = Motor(pinPWM, pinControl, pwm);
    Motor motorRight = Motor(PortID::D, PD6, PD4, pwm);
    motorRight.setBackwardMode();
    pwm.startTimer();

    _delay_ms(2000);

    motorLeft.setPower(Motor::MEDIUM_POWER);
    motorRight.setPower(Motor::LOW_POWER);
    motorRight.setForwardMode();
    
    _delay_ms(2000);

    motorLeft.setPower(0x10);
    motorRight.setPower(0x80);
    motorRight.setBackwardMode();
    
    for (;;) {}
    return 0;

    // return 0;

    // DEBUG TESTS
    /*
          char     testChar  = 'O';
          char     testStr[] = "PolyMTL";
    const uint8_t  testLen   = 7;
          uint32_t testInt0  = -1;
          uint16_t testInt1  = -1;
          uint8_t  testInt2  = -1;

    setUsartRegisters();
    DEBUG(testChar);
    DEBUG_STR((testStr),(testLen));
    DEBUG((uint32_t(testInt0)));
    DEBUG((uint32_t(testInt1)));
    DEBUG((uint32_t(testInt2)));
    Led testLed = Led();
    testLed.turnGreen();
    while(true)
    {}
    */
}