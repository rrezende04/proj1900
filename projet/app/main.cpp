/*
 * Noms: Gabriel Bruyere, Eliott Bonnefoy, Thierry Poulin & Renato Rezende
 * Description: 
 *
 * Date: 8 novembre 2024
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
// #include <stdlib.h> // (for itoa)
#include "util.h"
#include "robot.h"
#include "memoire_24.h"
#include "algorithm.h"
#include "debug.h"
#include "communicator.h"
#include "motor.h"

static constexpr uint16_t TWO_SECOND_MS            = 2000;
static constexpr uint8_t  FLASH_FREQUENCY_HZ       = 4;
static constexpr uint8_t  FLASH_DELAY              = Util::ONE_SECOND_MS /  (2 * FLASH_FREQUENCY_HZ);
static constexpr uint8_t  FLASH_COUNTER            = FLASH_FREQUENCY_HZ * 2;
static constexpr uint8_t  MODE_MEMORY_ADDRESS      = 0;
static constexpr char     SIGNATURE[]              = {'2','8','3','4',' ','-',' ','J','e','r','-','O','h','m'};

// Every even indexed entry is a number of consecutive chars.
// Every odd-indexed entry is an ASCII code.
static constexpr uint8_t TRUE_SIGNATURE[] = {2, ' ', 6, '@', 30, ' ', 6, '@', 1, '\n',
											 6, ' ', 2, '@', 29, ' ', 2, '@', 4, ' ', 2, '@', 1, '\n',
											 6, ' ', 2, '@', 6, ' ', 6, '@', 6, ' ', 2, '@', 1, ' ', 5, '@', 2, ' ', 2, '@', 6, ' ', 2, '@', 1, '\n',
											 6, ' ', 2, '@', 4, ' ', 3, '@', 4, ' ', 2, '@', 5, ' ', 3, '@', 7, ' ', 2, '@', 6, ' ', 2, '@', 1, '\n',
											 6, ' ', 2, '@', 4, ' ', 9, '@', 5, ' ', 2, '@', 8, ' ', 2, '@', 6, ' ', 2, '@', 1, '\n',
											 6, ' ', 2, '@', 4, ' ', 2, '@', 12, ' ', 2, '@', 8, ' ', 2, '@', 6, ' ', 2, '@', 1, '\n',
											 6, ' ', 2, '@', 4, ' ', 3, '@', 11, ' ', 2, '@', 9, ' ', 2, '@', 4, ' ', 2, '@', 1, '\n',
											 7, '@', 7, ' ', 7, '@', 5, ' ', 2, '@', 8, ' ', 4, '@', 2, ' ', 4, '@', 1, '\n'};

/*
  @@@@@@                              @@@@@@\n
      @@                             @@    @@\n
      @@      @@@@@@      @@ @@@@@  @@      @@\n
      @@    @@@    @@     @@@       @@      @@\n
      @@    @@@@@@@@@     @@        @@      @@\n
      @@    @@            @@        @@      @@\n
      @@    @@@           @@         @@    @@\n
@@@@@@@       @@@@@@@     @@        @@@@  @@@@\n}
*/

static_assert(0 < FLASH_FREQUENCY_HZ, "FLASH_FREQUENCY_HZ needs to be positive.");


volatile bool runTaskStep = false;


ISR(INT0_vect)
{ 
	uint8_t preRead = PIND & (1 << PD2);
	_delay_ms(Util::DEBOUNCE_TIME_MS);

	if (preRead == (PIND & (1 << PD2)))
		runTaskStep = true;
}

// Used for amber light.
ISR(TIMER0_COMPA_vect)
{
	// Toggle Led pins (Green <-> Red)
	PORTB ^= (1 << PB2) ^ (1 << PB3);
}

enum class OperatingMode
{
	SELECT_MODE       = 0,
	FIND_EXTREMITY    = 1,
	TRAVERSE_COURSE   = 2,
	DISPLAY_REPORT    = 3
};

void flashColor(Led::Colour colour, Robot& bot);
//void turnAmber(Led& Led, uint16_t duration);

void confirmationFlash(OperatingMode mode, Robot& bot)
{
	switch(mode)
	{
		case OperatingMode::SELECT_MODE:
			break;
		case OperatingMode::FIND_EXTREMITY:
			flashColor(Led::Colour::GREEN, bot);
			_delay_ms(TWO_SECOND_MS);
			break;
		case OperatingMode::TRAVERSE_COURSE:
			flashColor(Led::Colour::RED, bot);
			break;
		case OperatingMode::DISPLAY_REPORT:
			flashColor(Led::Colour::AMBER, bot);
			break;
	}
}

void flashColor(Led::Colour colour, Robot& bot)
{
	for(uint8_t i = 0; i < FLASH_COUNTER; i++)
	{
		bot.turnLed(colour);
		_delay_ms(FLASH_DELAY);
		bot.turnLed(Led::Colour::OFF);
		_delay_ms(FLASH_DELAY);
	}
}


// Function that saves the selected mode in the EEPROM.
void saveOperatingMode(OperatingMode mode)
{
	Memoire24CXXX mem;
	mem.ecriture(MODE_MEMORY_ADDRESS, (uint8_t)mode);
}

// Function that reads the operating mode in the EEPROM (if none return SELECT_MODE) btw a short hand if... else might be useful
OperatingMode readSavedOperatingMode(uint8_t* container) 
{
	static constexpr uint8_t MAX_MODE_VALUE = 3;
	Memoire24CXXX mem;
	mem.lecture(MODE_MEMORY_ADDRESS, container);

	// FAILSAFE IF MEMORY INVALID
	if (*container > MAX_MODE_VALUE)
		return OperatingMode::SELECT_MODE;

	return static_cast<OperatingMode>(*container);
}

void resetOperatingMode()
{
	Memoire24CXXX mem;
	mem.ecriture(MODE_MEMORY_ADDRESS, (uint8_t)OperatingMode::SELECT_MODE);
}

// Execution of mode SELECTMODE
void selectMode(Robot& bot)
{
	while(true)
	{
		saveOperatingMode(OperatingMode::FIND_EXTREMITY);
		bot.turnLed(Led::Colour::GREEN);
		_delay_ms(TWO_SECOND_MS);   
		saveOperatingMode(OperatingMode::TRAVERSE_COURSE);
		bot.turnLed(Led::Colour::RED);
		_delay_ms(TWO_SECOND_MS);
		saveOperatingMode(OperatingMode::DISPLAY_REPORT);
		bot.turnLed(Led::Colour::AMBER);
		_delay_ms(TWO_SECOND_MS);
	}
}

// Execution of mode FIND_EXTREMITY
void findExtremity(Algorithm& A) 
{
	resetOperatingMode();
	confirmationFlash(OperatingMode::FIND_EXTREMITY, *A.bot_);
	
	A.performTaskOne(Motor::PRESENTATION_POWER); 
	while(true) {}
}
   
// Execution of mode TRAVERSE_COURSE
void traverseCourse(Algorithm& A) 
{
	resetOperatingMode();
	confirmationFlash(OperatingMode::TRAVERSE_COURSE, *A.bot_);
	
	while(true)
	{
		while(!runTaskStep) {}
		runTaskStep = false;
		_delay_ms(TWO_SECOND_MS);
		A.performTaskTwoStep(Motor::PRESENTATION_POWER);
	}
}

void printFromMemory(uint16_t address)
{
	uint8_t memContainer[1];
	Memoire24CXXX mem;
	mem.lecture(address, memContainer);
	sendChar(memContainer[0]);
}

void displayReport(Algorithm& A, Robot& b) 
{
	resetOperatingMode();
	A.performTaskThree();
	displayJerohm();
	b.playFanfare();
}

void displayJerohm()
{
	static constexpr uint8_t NEW_LINE   = 10;
	sendChar(NEW_LINE);

	for (uint8_t i = 0; i < sizeof(TRUE_SIGNATURE); i += 2)
	{
		for (uint8_t j = 0; j < TRUE_SIGNATURE[i]; j++)
		{
			sendChar(TRUE_SIGNATURE[i+1]);
			_delay_ms(Util::USART_DELAY_MS);
		}
	}
}

int main() 
{
	cli();

	Robot JerΩ = {};
	Algorithm A = Algorithm(JerΩ);

	uint8_t memContainer[1];
	OperatingMode mode = readSavedOperatingMode(memContainer);

	sei();

	switch (mode)
	{
		case OperatingMode::SELECT_MODE:
			selectMode(JerΩ);
			break;
		case OperatingMode::FIND_EXTREMITY:
			findExtremity(A);
			break;   
		case OperatingMode::TRAVERSE_COURSE:
			traverseCourse(A);
			break;
		case OperatingMode::DISPLAY_REPORT:
			displayReport(A, JerΩ);
			break;
	}
}
