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

#include "robot.h"

Robot::Robot()
: buttonPin_ 	   (PortID::D, 2)
, infraRedPin_	   (PortID::A, 0)
, ledPosPin_ 	   (PortID::B, 2)
, ledNegPin_ 	   (PortID::B, 3)
, leftMotorPwmPin_ (PortID::D, 6)
, leftMotorDirPin_ (PortID::B, 0)
, rightMotorPwmPin_(PortID::D, 7)
, rightMotorDirPin_(PortID::B, 1)
, piezoGrndPin_    (PortID::D, 3)
, piezoSignalPin_  (PortID::D, 5)
, pinLm1_		   (PortID::C, 2)
, pinLm2_		   (PortID::C, 3)
, pinLm3_		   (PortID::C, 4)
, pinLm4_		   (PortID::C, 5)
, pinLm5_		   (PortID::C, 6)
, pwmTimer_  ()
, timer0_    ()
, timer1_    ()
, button_    (buttonPin_, false, true)
, infrared_  (infraRedPin_)
, led_       (ledPosPin_, ledNegPin_, timer0_)
, leftMotor_ (leftMotorPwmPin_, leftMotorDirPin_, pwmTimer_)
, rightMotor_(rightMotorPwmPin_, rightMotorDirPin_, pwmTimer_)
, piezo_     (piezoGrndPin_, piezoSignalPin_, timer0_, timer1_)
, lineMaker_ (pinLm1_, pinLm2_, pinLm3_, pinLm4_, pinLm5_) 
{ 
	setUsartRegisters();
	button_.catchRisingEdge();
}

// MOVEMENT

void Robot::move(Util::Dir direction, uint8_t power)
{
	leftMotor_.setDirMode(direction);
	rightMotor_.setDirMode(direction);
	leftMotor_.setPower(power);
	rightMotor_.setPower(power);
}

void Robot::invertDirection(uint8_t power)
{ 
	leftMotor_.invertDirection(power);
	rightMotor_.invertDirection(power);
}

void Robot::turn(Util::Side side, uint8_t power)
{
	switch (side)
	{
		case Util::Side::LEFT:
			leftMotor_.setDirMode(Util::Dir::BACKWARD);
			rightMotor_.setDirMode(Util::Dir::FORWARD);
			break;
		case Util::Side::RIGHT:
			leftMotor_.setDirMode(Util::Dir::FORWARD);
			rightMotor_.setDirMode(Util::Dir::BACKWARD);
			break;
	}
	leftMotor_.setPower(power);
	rightMotor_.setPower(power);
}

void Robot::adjust(Util::Side side, uint8_t power, uint8_t inverseAdjustmentStrength)
{
	// Motor::MAX_POWER / (1 + 1/inverseAdjustmentStrength)
	const uint16_t MAX_REGULARLY_ADJUSTABLE_POWER =
		Motor::MAX_POWER * inverseAdjustmentStrength / (inverseAdjustmentStrength + 1);
	
	uint8_t adjustment = power / inverseAdjustmentStrength;

	Motor* increaseMotor = 0;
	Motor* decreaseMotor = 0;
	switch (side)
	{
		case Util::Side::LEFT:
			increaseMotor = getMotor(Util::Side::RIGHT);
			decreaseMotor = getMotor(Util::Side::LEFT);
			break;
		case Util::Side::RIGHT:
			increaseMotor = getMotor(Util::Side::LEFT);
			decreaseMotor = getMotor(Util::Side::RIGHT);
			break;
	}

	if (power < MAX_REGULARLY_ADJUSTABLE_POWER)
	{
		increaseMotor->setPower(power + adjustment);
		decreaseMotor->setPower(power - adjustment);
	} else {
		increaseMotor->setPower(Motor::MAX_POWER);
		// Converting any excess increase into a decrease in the other motor,
		// even up to inverting the direction of the other motor to allow
		//     decreasedPower = power - power / inverseAdjustmentStrength - (IP - M)
		//     where IP is increasedPower if it could go beyond 0xFF, i.e.
		//     IP = power + adjustment
		//     So decreasedPower = M - 2 * adjustment
		//	   where decreasedPower < 0 is converted to speed in the opposite direction
		int16_t decreasedPower = Motor::MAX_POWER - 2 * adjustment;
		if (decreasedPower >= 0)
			decreaseMotor->setPower(decreasedPower);
		else
			decreaseMotor->invertDirection(-decreasedPower);
	}
}

void Robot::softAdjust(Util::Side side, uint8_t power)
{ 
	static constexpr uint8_t SOFT_ADJUSTMENT_STRENGHT_INVERSE = 6;
	adjust(side, power, SOFT_ADJUSTMENT_STRENGHT_INVERSE); 
}

void Robot::hardAdjust(Util::Side side, uint8_t power)
{ 
	static constexpr uint8_t HARD_ADJUSTMENT_STRENGHT_INVERSE = 3;
	adjust(side, power, HARD_ADJUSTMENT_STRENGHT_INVERSE); 
}

void Robot::reverseAdjust(Util::Side side, uint8_t power)
{
	switch (side)
	{
		case Util::Side::LEFT:
			rightMotor_.setPower(power - power / 3);
			break;
		case Util::Side::RIGHT:
			leftMotor_.setPower(power - power / 3);
			break;
	}
}

void Robot::followLine(uint8_t power)
{
	if (s3()) // See center
	{
		move(Util::Dir::FORWARD, power);
		if (s2()) // Slightly to the left
			softAdjust(Util::Side::LEFT, power);
		else if (s4()) // Slightly to the right
			softAdjust(Util::Side::RIGHT, power);
	}
	else if (s2() || s1()) // More to the left
		hardAdjust(Util::Side::LEFT, power);
	else if (s4() || s5()) // More to the right
		hardAdjust(Util::Side::RIGHT, power);
	else // Useful for inching forward using this function
	     // after having gone past an intersection
		move(Util::Dir::FORWARD, power);
}

void Robot::moonWalk(uint8_t power)
{
	if (s3()) // Si on a le milieu
	{
		move(Util::Dir::BACKWARD, power);
		if (s2())
			reverseAdjust(Util::Side::LEFT, power);
		else if (s4())
			reverseAdjust(Util::Side::RIGHT, power);
	}
	else if (s2() | s1())
		reverseAdjust(Util::Side::LEFT, power);
	else if (s4() | s5())
		reverseAdjust(Util::Side::RIGHT, power);
	else
		move(Util::Dir::BACKWARD, power);
}

void Robot::stopMoving()
{
	leftMotor_.setPower(0);
	rightMotor_.setPower(0);
}


// LINEMAKER

uint8_t Robot::readLineMaker() {return lineMaker_.read(); }
bool Robot::s1() { return lineMaker_.isS1(); }
bool Robot::s2() { return lineMaker_.isS2(); }
bool Robot::s3() { return lineMaker_.isS3(); }
bool Robot::s4() { return lineMaker_.isS4(); }
bool Robot::s5() { return lineMaker_.isS5(); }


// INFRARED

float Robot::readDistanceCm()
{ return infrared_.readDistanceCm(); }


// LED

void Robot::turnLed(Led::Colour colour)
{ led_.turn(colour); }

void Robot::turnLedAmberMs(uint16_t duration)
{ led_.turnAmberMs(duration); }


// PIEZO

void Robot::playNote(uint8_t note)
{ piezo_.playNote(note); }

void Robot::playMusicNote(uint8_t note, uint16_t type)
{
	playNote(note);
	Util::wait_ms(type);
	stopNote();
}

void Robot::stopNote()
{ piezo_.stopPlaying(); }

void Robot::playFanfare()
{
	// Partition utilisée: https://musescore.com/user/2551621/scores/894371

	// Avec un bpm d'environ 140 :
	static constexpr uint16_t TRIOLET_MS = 143; // 143,33...
	static constexpr uint16_t CROCHE_MS  = 215;
	static constexpr uint16_t NOIRE_MS   = 430;
	static constexpr uint16_t BLANCHE_MS = 860;
	static constexpr uint16_t RONDE_MS   = 1720;

	bool firstPass = true;

	for(int i = 0; i < 3; i++)
	{
		playMusicNote(77, TRIOLET_MS);
	}

	playNote(77);
	_delay_ms(NOIRE_MS);

	playMusicNote(73, NOIRE_MS);
	playMusicNote(75, NOIRE_MS);
	playMusicNote(77, CROCHE_MS);
	_delay_ms(CROCHE_MS); // Silence 1 demi-temps
	playMusicNote(75, CROCHE_MS);
	playMusicNote(77, (BLANCHE_MS + NOIRE_MS)); // Blanche pointée

	while(true)
	{
		playMusicNote(72, NOIRE_MS);
		playMusicNote(70, NOIRE_MS);
		playMusicNote(72, NOIRE_MS);
		playMusicNote(70, CROCHE_MS);
		playMusicNote(75, NOIRE_MS);
		playMusicNote(75, CROCHE_MS);
		playMusicNote(74, NOIRE_MS);
		playMusicNote(75, CROCHE_MS);
		playMusicNote(74, NOIRE_MS);
		playMusicNote(74, CROCHE_MS);
		playMusicNote(72, NOIRE_MS);
		playMusicNote(70, NOIRE_MS);

		if(firstPass)
		{
			playMusicNote(69, NOIRE_MS);
			playMusicNote(70, CROCHE_MS);
			playMusicNote(67, (NOIRE_MS + RONDE_MS));
			firstPass = false;
		}
		else
		{
			playMusicNote(72, NOIRE_MS);
			playMusicNote(75, CROCHE_MS);
			playMusicNote(77, (NOIRE_MS + RONDE_MS));
			firstPass = true;
		}
	}
}


// GETTERS
Timer0& Robot::getTimer0() { return timer0_; }
Timer1& Robot::getTimer1() { return timer1_; }
Button& Robot::getButton() { return button_; }
Infrared& Robot::getInfrared() { return infrared_; }


///// PRIVATE /////

Motor* Robot::getMotor(Util::Side side)
{	if (side == Util::Side::LEFT)
		return &leftMotor_;
	else
		return &rightMotor_;
}
