/*
 * Nom: Button Controller
 *
 */
#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

enum class PressCount
{
    ONE,
    TWO,
    THREE
};

const int TICK_MS = 10;
const int DEL_TIMER_MS = 2000;

void delGreen()
{
    PORTA |= (1 << PA0);
    PORTA &= ~(1 << PA1);
}

bool readingButton()
{
    if (PIND & 0x04)
    {
        _delay_ms(TICK_MS);
        if (PIND & 0x04)
        {
            return true;
        }
    }
    return false;
}

void nextStateSwitch(PressCount& currentState, PressCount& nextState)
{
    switch (currentState)
    {
    case (PressCount::ONE):
        nextState = PressCount::TWO;
        break;

    case (PressCount::TWO):
        nextState = PressCount::THREE;
        break;

    case (PressCount::THREE):
        nextState = PressCount::ONE;
        break;
    }
}

int main()
{
    DDRA |= (1 << PA0) | (1 << PA1);
    DDRD &= ~(1 << PD2);

    bool thirdPress = false;
    bool buttonPressed = false;
    PressCount currentState = PressCount::ONE;
    PressCount nextState;

    while (true)
    {
        PORTA &= ~(1 << PA1) & ~(1 << PA0);
        if (readingButton())                        //Si le bouton est bel et bien pressé, en tenant compte du rebond
        {
            buttonPressed = true;                   //On dit qu'il a été pressé une fois pour éviter plus tard de ré-exécuter le code qui résulte du bouton pressé lorsque qu'il reste pressé pendant plusieurs éxécutions de la boucle while
            if (currentState == PressCount::THREE)
                thirdPress = true;                  //Cette booléenne sert à allumer la LED seulement lorsque le bouton est relâché
        }
        else                                        //À partir d'ici le bouton n'est pas appuyé
        {
            if (buttonPressed)                      //Condition qui correspond à un bouton qui vient d'être relâché (quand buttonPressed = true)
            {
                nextStateSwitch(currentState, nextState);
                
                currentState = nextState;
                buttonPressed = false;
            }
            else if (thirdPress)
            {
                delGreen();
                _delay_ms(DEL_TIMER_MS);
                thirdPress = false;
            }
        }
    }

    return 0;
}
