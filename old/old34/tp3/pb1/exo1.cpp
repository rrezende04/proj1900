#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

float const PERIOD_US = 1000;
float START_CYCLE_RATIO = 100;
float END_CYCLE_RATION = 0;
float const DURATION_MS = 3000;
float const MS_TO_US = 1000;
float const DELTA_DELAY_US = 10;
float const CENT = 100;




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

void waitDelayUs(int delay)
{
	int cycle = delay / DELTA_DELAY_US;
	for (int i = 0; i < cycle; i++)
	{
		_delay_us(DELTA_DELAY_US);
	}
}

double calculateHighTime(int cycleRatio)
{
	return PERIOD_US * (cycleRatio / CENT);
}

double calculateLowTime(int highTime)
{
	return PERIOD_US - highTime;
}

double calculateCurrentCycleRatio(long elapsedTime)
{
	return CENT - ((elapsedTime / (DURATION_MS * MS_TO_US)) * CENT);
}


int main()
{
	DDRA |= (1 << PA0) | (1 << PA1);

	int currentCycleRatio = START_CYCLE_RATIO;
	double highTime = 0;
	double lowTime = 0;
	double elapsedTime = 0;
	while (true)
	{
		elapsedTime += PERIOD_US;
		highTime = calculateHighTime(currentCycleRatio);
		lowTime = calculateLowTime(highTime);

		displayRed();
		waitDelayUs(highTime);

		displayOff();
		waitDelayUs(lowTime);

		currentCycleRatio = calculateCurrentCycleRatio(elapsedTime);

		if (elapsedTime >= (DURATION_MS * MS_TO_US))
		{
			break;
		}
	}

}