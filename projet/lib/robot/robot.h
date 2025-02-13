/*
 * Classe regroupant toutes les composantes du robot.
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
#include "button.h"
#include "communicator.h"
#include "debug.h"
#include "infrared.h"
#include "led.h"
#include "motor.h"
#include "piezo.h"
#include "pin.h"
#include "pwmtimer.h"
#include "timer0.h"
#include "timer1.h"
#include "lineMaker.h"
#include "util.h"

class Robot
{
public:
	
	Robot();


	// MOVEMENT
	// Move in direction at power
	void move(Util::Dir direction, uint8_t power = Motor::LOW_POWER);
	void invertDirection(uint8_t power = Motor::LOW_POWER);
	
	void turn(Util::Side side, uint8_t power = Motor::LOW_POWER);

	// Adjustments divert up to (1/inverseAdjustmentStrength) * power to one wheel
	void adjust(Util::Side side,
			    uint8_t power = Motor::LOW_POWER,
				uint8_t inverseAdjustmentStrength = 3);
	void softAdjust(Util::Side side, uint8_t power = Motor::LOW_POWER);
	void hardAdjust(Util::Side side, uint8_t power = Motor::LOW_POWER);
	void reverseAdjust(Util::Side side, uint8_t power = Motor::LOW_POWER);

	void followLine(uint8_t power = Motor::LOW_POWER);
	void moonWalk(uint8_t power = Motor::LOW_POWER);

	void stopMoving();


	// LINEMAKER
	uint8_t readLineMaker();
	bool s1();
	bool s2();
	bool s3();
	bool s4();
	bool s5();

	// INFRARED
	// Wait Infrared::READ_DELAY_MS between every measure
	float readDistanceCm();


	// LED
	void turnLed(Led::Colour colour);
	void turnLedAmberMs(uint16_t durationMs);

	// PIEZO
	void playNote(uint8_t note);
	void playMusicNote(uint8_t note, uint16_t type);
	void stopNote();
	void playFanfare();


	// GETTERS
	Timer0& getTimer0();
	Timer1& getTimer1();
	Button& getButton();
	Infrared& getInfrared();



private:
	// MEMBER ATTRIBUTES
	Pin buttonPin_;
	Pin infraRedPin_;
	Pin ledPosPin_;
	Pin ledNegPin_;
	Pin leftMotorPwmPin_;
	Pin leftMotorDirPin_;
	Pin rightMotorPwmPin_;
	Pin rightMotorDirPin_;
	Pin piezoGrndPin_;
	Pin piezoSignalPin_;
	Pin pinLm1_;
	Pin pinLm2_;
	Pin pinLm3_;
	Pin pinLm4_;
	Pin pinLm5_;

	PWMTimer pwmTimer_;
	Timer0 timer0_;
	Timer1 timer1_;
	Button button_;
	Infrared infrared_;
	Led led_;
	Motor leftMotor_;
	Motor rightMotor_;
	Piezo piezo_;
	LineMaker lineMaker_;

	// HELPER / GETTER
	Motor* getMotor(Util::Side side);
};
