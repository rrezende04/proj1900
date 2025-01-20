/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: Selon le temps pour lequel est maintenu
 *              enfoncé un bouton (qui vit sur le breadboard),
 *              la LED clignote rouge proportionnellement longtemps.
 *
 *              Quelques signaux lumineux intermédiaires indique le
 *              stade atteint dans le programme.
 *
 *              LED branchée sur PB[0:1] : PB0+, PB1-
 *                  (1,0) = vert;
 *                  (0,1) = rouge.
 *              Bouton branché sur PD2.
 *
 *      Tableau d'états de la machine finie représentant le code
 *
 *       +==============+=================+============+======+
 *       | État présent | Bouton poussoir | État futur |  LED |
 *       +==============+=================+============+======+
 *       | INIT         | Relâché         | INIT       |  OFF |
 *       +--------------+-----------------+------------+------+
 *       | INIT         | Appuyé          | PRESS      |  OFF |
 *       +--------------+-----------------+------------+------+
 *       | PRESS        | Appuyé          | PRESS(*)   |  OFF |
 *       +--------------+-----------------+------------+------+
 *       | PRESS        | Relâché         | DONE       |  OFF |
 *       +--------------+-----------------+------------+------+
 *       | DONE         | X               | INIT       | (**) |
 *       +--------------+-----------------+------------+------+
 *
 *       * Le compteur peut forcer l'état a DONE (si le compteur est rendu a 120)
 *
 *       ** La LED exécute une séquence dépendante de la valeur du compteur :
 *           Elle clignote vert pendant une demie seconde, puis elle clignote rouge (compteur / 2) fois
 *           Elle devient ensuite verte pendant une seconde, puis s'éteint.
 *
 * Date: 2 octobre 2024
 */

// TODO : ISR MODIFICATIONS, COUNTER CHECK IN MAIN

#define F_CPU 8000000

#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

enum class State
{
    INIT,
    PRESS,
    DONE
};

volatile State gState = State::INIT;
volatile uint8_t gCounter = 0;
volatile bool gTransitionToPress = false;
volatile bool gTransitionToDone = false;


const uint16_t CLK_PRESCALER = 1024;    // set by CSn[2:0]
const uint16_t INCREMENT_FREQUENCY_HZ = F_CPU / CLK_PRESCALER;  // 7812
const uint8_t INCREMENTS_HZ = 10;   // given by lab statement
const uint16_t TIMER_COMPARE = INCREMENT_FREQUENCY_HZ / INCREMENTS_HZ;
const uint8_t DELAY_RESOLUTION_MS = 100;
const uint16_t DELAY_2K_MS = 2000;
const uint16_t DELAY_1K_MS = 1000;
const uint16_t FLICKER_GREEN_TIME_MS = 500;
const uint8_t MAX_COUNTER = 120;
const uint8_t DEBOUNCE_DELAY_MS = 20;
const uint8_t COUNTER_DIVIDER = 2;

ISR(TIMER1_COMPA_vect)
{
    gCounter++;
}

ISR(INT0_vect)
{
    uint8_t preRead = (PIND & (1 << PD2));
    _delay_ms(DEBOUNCE_DELAY_MS);
    if (preRead != (PIND & (1 << PD2)))
        return;

    if (preRead == 0) // Inverse logic : the button is pressed
        gTransitionToPress = true;
    else
        gTransitionToDone = true;
}

void turnLedOff() // 00
{
    PORTB &= ~(1 << PB0); // 0x
    PORTB &= ~(1 << PB1); // x0
}

void turnLedGreen() // 01
{
    PORTB &= ~(1 << PB0); // 0x
    PORTB |= (1 << PB1);  // x1
}

void turnLedRed() // 10
{
    PORTB |= (1 << PB0);  // 1x
    PORTB &= ~(1 << PB1); // x0
}

void flickerGreen(uint16_t timeMs)
{
    const uint8_t HALF_FLICKER_TIME = 50;
    uint16_t loops = timeMs / DELAY_RESOLUTION_MS;
    for (uint16_t i = 0; i < loops; i++)
    {
        turnLedGreen();
        _delay_ms(HALF_FLICKER_TIME);
        turnLedOff();
        _delay_ms(HALF_FLICKER_TIME);
    }
}

void flickerRed()
{
    const uint8_t HALF_FLICKER_TIME = 250;
    gCounter /= COUNTER_DIVIDER;
    while (gCounter > 0)
    {
        turnLedRed();
        _delay_ms(HALF_FLICKER_TIME);
        turnLedOff();
        _delay_ms(HALF_FLICKER_TIME);

        (gCounter)--;
    }
}

void activateButtonInterrupts()
{
    EIMSK |= (1 << INT0); // External Interrupt Mask
}

void catchFallingEdgeButtonMode()
{
    EICRA &= ~(1 << ISC00); // x0 Falling edge
    EICRA |= (1 << ISC01);  // 1x detection only
}

void catchRisingEdgeButtonMode()
{
    EICRA |= (1 << ISC00); // x1 rising edge
    EICRA |= (1 << ISC01); // 1x detection only
}

void initializeRegisters()
{
    cli();
        // LED and external (breadboard) button on PD2
    DDRB |= (1 << PB0) | (1 << PB1);
    DDRD &= ~(1 << PD2);

    activateButtonInterrupts();
    catchFallingEdgeButtonMode();

        // Timer1
        //   clock divided by 1024 (prescaler)
        //   interruption after TIMER_COMPARE counts, corresponding
        //   to 10Hz TIMER1_COMPA interrupt flag frequency
    OCR1A = TIMER_COMPARE;
    TCCR1A &= ~(1 << WGM11) & ~(1 << WGM10);
    TCCR1B &= ~(1 << WGM13);
    TCCR1B |= (1 << WGM12);
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A); 
        // Timer Interrupt Mask OCIEA for timer compare interrupt
    sei();
}

void startTimer()
{
    TCNT1 = 0; // Timer/Counter 1 reinitialised to 0
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TCCR1B &= ~(1 << CS11); // Timer1 CTC mode
}

void stopTimer()
{
    TCNT1 = 0;
    TCCR1B &= ~(1 << CS12) & ~(1 << CS11) & ~(1 << CS10); // CS1[2:0] = 000
}

void deactivateButtonInterrupts()
{
    EIMSK &= ~(1 << INT0);
}

void init()
{
    cli();
    initializeRegisters();
    catchFallingEdgeButtonMode(); 
        // From here on, detect when button is pressed
    stopTimer();
    sei();
}

void runInitStateRoutine()
{
    if (gTransitionToPress)
    {
        cli();
        catchRisingEdgeButtonMode(); 
            // From here on, detect when button is unpressed
        startTimer();
        sei();
        gState = State::PRESS;
    }
}

void runPressStateRoutine()
{
    if ((gCounter >= MAX_COUNTER) | gTransitionToDone )
    {
        stopTimer();
        gState = State::DONE;
    }
}

void runDoneStateRoutine()
{
    cli(); // Stop interrupts

    flickerGreen(FLICKER_GREEN_TIME_MS);
    _delay_ms(DELAY_2K_MS);

    flickerRed();
    turnLedGreen();
    _delay_ms(DELAY_1K_MS);

    turnLedOff();                   //Prep for return to init state
    catchFallingEdgeButtonMode();
    stopTimer();
    sei(); // Resume interrupts

    gState = State::INIT;

    gTransitionToDone = false;
    gTransitionToPress = false;
}

void runStateMachine()
{
    switch (gState)
    {
        case State::INIT: 
            runInitStateRoutine();
            break;
        case State::PRESS:
            runPressStateRoutine();
            break;
        case State::DONE:
            runDoneStateRoutine();
            break;
        }
}

int main()
{
    init();
    while (true)
        runStateMachine();
}
