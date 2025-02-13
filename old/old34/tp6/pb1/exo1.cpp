#define F_CPU 8000000


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


const uint8_t BUTTON_MASK = 0x04;
const uint8_t TICK_MS = 10;
const uint16_t HALF_SECOND_MS = 500;
const uint16_t SECOND_MS = 1000;
const uint16_t TWO_SECOND_MS = 2000;
const uint8_t HALF = 2;
const uint8_t MAX_COUNTER = 120;
const uint16_t ONE_TENTH_SECOND_CYCLE = 781;
volatile uint8_t counter = 0;


enum class State
{
	OFF,
	BLINK_RED,
	BLINK_GREEN,
	GREEN,
};

enum class ButtonState
{
	PRESSED,
	RELEASED,
};

volatile ButtonState buttonState = ButtonState::RELEASED;

void displayGreen()
{
	PORTB |= (1 << PB1);
	PORTB &= ~(1 << PB0);
}

void displayRed()
{
	PORTB |= (1 << PB0);
	PORTB &= ~(1 << PB1);
}

void displayOff()
{
	PORTB &= ~(1 << PB0) & ~(1 << PB1);
}

ISR(TIMER1_COMPA_vect)
{
	counter++;
}

ISR(INT0_vect)
{
	if (!(PIND & BUTTON_MASK))
	{
		_delay_ms(TICK_MS);
		if (!(PIND & BUTTON_MASK))
		{
			buttonState = ButtonState::PRESSED;
		}
	}
	else
	{
		_delay_ms(TICK_MS);
		if ((PIND & BUTTON_MASK))
		{
			buttonState = ButtonState::RELEASED;
		}
	}
}

void initialisation(void)
{
	cli();

	DDRB |= (1 << PB0) | (1 << PB1);
	DDRD &= ~(1 << PD2);

	EIMSK |= (1 << INT0);

	EICRA |= (1 << ISC01);

	sei();

	displayOff();
}

void startTimer(uint16_t duration)
{
	cli();
	counter = 0;

	TCNT1 = 0;

	OCR1A = duration;

	TCCR1A = 0;

	//TCCR1B = 0;
	TCCR1B |= (1 << CS12) | (1 << CS10) | (1 << WGM12);

	TCCR1C = 0;

	//TIMSK1 = 0;
	TIMSK1 |= (1 << OCIE1A);

	sei();
}

void stopTimer()
{
	TCCR1B = 0;
}


int main()
{
	initialisation();
	State currentState = State::OFF;
	State previousState = State::OFF;
	bool condition = true;
	while (true)
	{
		if (buttonState == ButtonState::PRESSED)
		{
			startTimer(ONE_TENTH_SECOND_CYCLE);
			do
			{
				;
			} while ((buttonState == ButtonState::PRESSED) && (counter < MAX_COUNTER));
			stopTimer();

			condition = true;
			currentState = State::BLINK_GREEN;
			previousState = currentState;

			while (condition) //state machine is executed completly
			{
		 		switch (currentState)
		 		{
		 		case(State::OFF):
		 			displayOff();
		 			if (previousState == State::BLINK_GREEN)
		 			{
		 				_delay_ms(TWO_SECOND_MS);
		 				previousState = currentState;
		 				currentState = State::BLINK_RED;
		 			}
		 			break;
		 		case(State::BLINK_GREEN):
		 			displayGreen();
		 			_delay_ms(HALF_SECOND_MS);
		 			previousState = currentState;
		 			currentState = State::OFF;
		 			break;
		 		case (State::BLINK_RED):
		 			for (int i = 0; i < counter / HALF; i++)
		 			{
		 				displayRed();
		 				_delay_ms(HALF_SECOND_MS / HALF);
		 				displayOff();
		 				_delay_ms(HALF_SECOND_MS / HALF);
		 			}
		 			previousState = currentState;
		 			currentState = State::GREEN;
		 			break;
		 		case (State::GREEN):
		 			displayGreen();
		 			_delay_ms(SECOND_MS);
		 			displayOff();
		 			previousState = currentState;
		 			currentState = State::OFF;
		 			condition = false;
		 			break;
		 		}
			}
		}
	}
}