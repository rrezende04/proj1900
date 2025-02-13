#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <memoire_24.cpp>


uint8_t const STARTING_WRITE_ADRESS = 0x0000;
uint8_t const WRITING_TIME_MS = 10;

void initialisation(void)
{
	DDRA |= (1 << PA0) | (1 << PA1);
}

void displayRed()
{
	PORTA |= (1 << PA1);
	PORTA &= ~(1 << PA0);
}

void displayGreen()
{
	PORTA |= (1 << PA0);
	PORTA &= ~(1 << PA1);
}

void displayOff()
{
	PORTA &= ~(1 << PA0) & ~(1 << PA1);
}

int main()
{
	initialisation();

	uint8_t sentence[] = {'P', 'O', 'L', 'Y', 'T', 'E', 'C', 'H', 'N', 'I', 'Q', 'U', 'E', ' ', 'M', 'O', 'N', 'T', 'R', 'E', 'A', 'L',};

	uint8_t endingCharacter = 0x00;
	bool areMatching = true;
	int sentenceLength = sizeof(sentence)/sizeof(char);
	Memoire24CXXX memoire; 
	uint8_t charLu;


	for(int i = 0; i < sentenceLength; i++)
	{
		memoire.ecriture((STARTING_WRITE_ADRESS + (uint16_t)i), sentence[i]);
		_delay_ms(WRITING_TIME_MS);
	}

	memoire.ecriture((STARTING_WRITE_ADRESS + (uint16_t)(sentenceLength)), endingCharacter);
	_delay_ms(WRITING_TIME_MS);

	for (int i = 0; i < sentenceLength; i++)
	{
		memoire.lecture((STARTING_WRITE_ADRESS + (uint16_t)i), &charLu);
		if(charLu != sentence[i])
		{
			areMatching = false;
			displayRed();
			_delay_ms(100);
			displayOff();
			_delay_ms(100);
		}
		else
		{
			displayGreen();
			_delay_ms(100);
			displayOff();
			_delay_ms(100);
		}
		_delay_ms(WRITING_TIME_MS);
	}

	if(areMatching)
		displayGreen();
	else
		displayRed();




}