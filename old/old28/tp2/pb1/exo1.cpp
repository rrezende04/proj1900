/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: Utiliser une Led comme compteur (modulo 3) pour des pressions du bouton poussoir
 * 
 */

/*
Nous utilisons les ports PA1 (+) et PA0 (-) en sortie pour controler la LED
Nous utilisons le port PD2 comme entree du bouton poussoir
Lorsqu'elle est allumee, la LED sera toujours de couleur verte, i.e. 0x01


Tableau des etats du programme, avec sortie sur la LED

+--------------+-----------------+--------------+-----+
| Etat present | Bouton poussoir | Etat suivant | LED |
+--------------+-----------------+--------------+-----+
| INIT         | 0               | INIT         | Off |
+--------------+-----------------+--------------+-----+
| INIT         | 1               | ON1          | Off |
+--------------+-----------------+--------------+-----+
| ON1          | 1               | ON1          | Off |
+--------------+-----------------+--------------+-----+
| ON1          | 0               | OFF1         | Off |
+--------------+-----------------+--------------+-----+
| OFF1         | 0               | OFF1         | Off |
+--------------+-----------------+--------------+-----+
| OFF1         | 1               | ON2          | Off |
+--------------+-----------------+--------------+-----+
| ON2          | 1               | ON2          | Off |
+--------------+-----------------+--------------+-----+
| ON2          | 0               | OFF2         | Off |
+--------------+-----------------+--------------+-----+
| OFF2         | 0               | OFF2         | Off |
+--------------+-----------------+--------------+-----+
| OFF2         | 1               | ON3          | Off |
+--------------+-----------------+--------------+-----+
| ON3          | 1               | ON3          | Off |
+--------------+-----------------+--------------+-----+
| ON3          | 0               | OFF3         | Off |
+--------------+-----------------+--------------+-----+
| OFF3         | X               | INIT         | On  |
+--------------+-----------------+--------------+-----+
*/

#define F_CPU 8000000

#include <avr/io.h> 
#include <util/delay.h>


void turnLedOff()        // 00
{
    PORTA &= ~(1 <<PA1); // 0x
    PORTA &= ~(1 <<PA0); // x0
}

void turnLedGreen()       // 01
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA |= (1 << PA0);  // x1
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

enum class State {INIT, ON1, OFF1, ON2, OFF2, ON3, OFF3};

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
                if (isPressedButton) state = State::ON1;
                break;
            case State::ON1 :
                if (!(isPressedButton)) state = State::OFF1;
                break;
            case State::OFF1 :
                if (isPressedButton) state = State::ON2;
                break;
            case State::ON2 :
                if (!(isPressedButton)) state = State::OFF2;
                break;
            case State::OFF2 :
                if (isPressedButton) state = State::ON3;
                break;
            case State::ON3 :
                if (!(isPressedButton)) state = State::OFF3;
                break;
            case State::OFF3 :
                turnLedGreen();
                _delay_ms(2000);
                state = State::INIT;
                turnLedOff();
                break;
        }
    }
    
    return 0;
}
