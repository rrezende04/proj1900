/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: Allumer une LED en differentes couleurs en fonction des entrees du bouton poussoir et dun etat interne
 * 
 */

/*
Nous utilisons les ports PA1 (+) et PA0 (-) en sortie pour controler la LED
Nous utilisons le port PD2 comme entree du bouton poussoir

En sortie sur (A1,A0), une LED de couleur:
    verte correspond a 01;
    rouge a 10;
    ambree a une alternance a haute frequence entre vert et rouge


Tableau des etats du programme, avec sortie sur la LED

+--------------+-----------------+--------------+-------+
| Etat present | Bouton poussoir | Etat suivant | LED   |
+--------------+-----------------+--------------+-------+
| INIT         | 0               | INIT         | Rouge |
+--------------+-----------------+--------------+-------+
| INIT         | 1               | ON1          | Rouge |
+--------------+-----------------+--------------+-------+
| ON1          | 1               | ON1          | Ambre |
+--------------+-----------------+--------------+-------+
| ON1          | 0               | OFF1         | Ambre |
+--------------+-----------------+--------------+-------+
| OFF1         | 0               | OFF1         | Vert  |
+--------------+-----------------+--------------+-------+
| OFF1         | 1               | ON2          | Vert  |
+--------------+-----------------+--------------+-------+
| ON2          | 1               | ON2          | Rouge |
+--------------+-----------------+--------------+-------+
| ON2          | 0               | OFF2         | Rouge |
+--------------+-----------------+--------------+-------+
| OFF2         | 0               | OFF2         | Off   |
+--------------+-----------------+--------------+-------+
| OFF2         | 1               | ON3          | Off   |
+--------------+-----------------+--------------+-------+
| ON3          | 1               | ON3          | Vert  |
+--------------+-----------------+--------------+-------+
| ON3          | 0               | INIT         | Vert  |
+--------------+-----------------+--------------+-------+
*/

#define F_CPU 8000000

#include <avr/io.h> 
#include <util/delay.h>


void turnLedOff()        // 00
{
    PORTA &= ~(1 <<PA1); // 0x
    PORTA &= ~(1 <<PA0); // x0
}

void turnLedGreen()     // 01
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA |= (1 << PA0);  // x1
}

void turnLedRed()        // 10
{
    PORTA |= (1 << PA1);   // 1x
    PORTA &= ~(1 << PA0);  // x0
}


void turnLedAmber() // Ambre pour 8 ms
{
    turnLedRed();
    _delay_ms(2);
    turnLedGreen();
    _delay_ms(5);
}

bool debounceButton() // Bouton est en PD2
{
    if (PIND & (1 << PD2))
    {
        _delay_ms(10);
        return (PIND & (1 << PD2));
    }
    return false;
}

void initializePorts()
{
    DDRA |= ( 1 << PA1); // PA1 sortie
    DDRA |= ( 1 << PA0); // PA0 sortie
    DDRD &= ~(1 << PD2); // PD2 entree
}

enum class State {INIT, ON1, OFF1, ON2, OFF2, ON3};

int main()
{   
    initializePorts();

    // Initialize state and button press status
    State state = State::INIT;
    bool isPressedButton = false;

    // Machine a etat
    while (true)
    {   
        isPressedButton = debounceButton();
        switch (state)
        {
            case State::INIT :
                turnLedRed();
                if (isPressedButton) state = State::ON1;
                break;
            case State::ON1 :
                turnLedAmber();
                if (!(isPressedButton)) state = State::OFF1;
                break;
            case State::OFF1 :
                turnLedGreen();
                if (isPressedButton) state = State::ON2;
                break;
            case State::ON2 :
                turnLedRed();
                if (!(isPressedButton)) state = State::OFF2;
                break;
            case State::OFF2 :
                turnLedOff();
                if (isPressedButton) state = State::ON3;
                break;
            case State::ON3 :
                turnLedGreen();
                if (!(isPressedButton)) state = State::INIT;
                break;
        }
    }
    
    return 0;
}
