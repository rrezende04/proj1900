/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: Allumer une LED en differentes couleurs en fonction des entrees du
 *              bouton poussoir et dun etat interne en utilisant les interrupts du ATmega324pa
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
| 0            | 0               | 0            | Rouge |
+--------------+-----------------+--------------+-------+
| 0            | 1               | 1            | Rouge |
+--------------+-----------------+--------------+-------+
| 1            | 1               | 1            | Ambre |
+--------------+-----------------+--------------+-------+
| 1            | 0               | 2            | Ambre |
+--------------+-----------------+--------------+-------+
| 2            | 0               | 2            | Vert  |
+--------------+-----------------+--------------+-------+
| 2            | 1               | 3            | Vert  |
+--------------+-----------------+--------------+-------+
| 3            | 1               | 3            | Rouge |
+--------------+-----------------+--------------+-------+
| 3            | 0               | 4            | Rouge |
+--------------+-----------------+--------------+-------+
| 4            | 0               | 4            | Off   |
+--------------+-----------------+--------------+-------+
| 4            | 1               | 5            | Off   |
+--------------+-----------------+--------------+-------+
| 5            | 1               | 5            | Vert  |
+--------------+-----------------+--------------+-------+
| 5            | 0               | 0            | Vert  |
+--------------+-----------------+--------------+-------+
*/

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void turnLedOff() // 00
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA &= ~(1 << PA0); // x0
}

void turnLedGreen() // 01
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA |= (1 << PA0);  // x1
}

void turnLedRed() // 10
{
    PORTA |= (1 << PA1);  // 1x
    PORTA &= ~(1 << PA0); // x0
}

void turnLedAmber() // Ambre pour 8 ms
{
    turnLedRed();
    _delay_ms(2);
    turnLedGreen();
    _delay_ms(5);
}

bool getBtnPressState()
{
    return PIND & (1 << PD2);
}

void initializePorts()
{
    cli();               // prevents interrupts
    DDRA |= (1 << PA1);  // PA1 sortie
    DDRA |= (1 << PA0);  // PA0 sortie
    DDRD &= ~(1 << PD2); // PD2 entree
    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC00);  //   x1 mode detection de fronts
    EICRA &= ~(1 << ISC01); //  0x montants et descendants
    sei();                  // resume interrupt handling
}

// Initialize state and button press status
volatile uint8_t state = 0;
volatile bool wasPressedBtn;

ISR(INT0_vect)
{
    // laisser un délai avant de confirmer la réponse du
    // bouton-poussoir: environ 30 ms (anti-rebond)

    _delay_ms(30);

    // se souvenir ici si le bouton est pressé ou relâché
    if (getBtnPressState() != wasPressedBtn)
    {
        // changements d'état tels que ceux de la
        // semaine précédente
        wasPressedBtn = !wasPressedBtn;
        state = (state + 1) % 6;

        // Voir la note plus bas pour comprendre cette instruction et son rôle
        EIFR |= (1 << INTF0);
    }
}

int main()
{
    initializePorts();

    // Initialize state and button press status

    // Machine a etat
    while (true)
    {
        // isPressedButton = debounceButton();
        switch (state)
        {
        case 0:
            turnLedRed();
            break;
        case 1:
            turnLedAmber();
            break;
        case 2:
            turnLedGreen();
            break;
        case 3:
            turnLedRed();
            break;
        case 4:
            turnLedOff();
            break;
        case 5:
            turnLedGreen();
            break;
        }
    }

    return 0;
}
