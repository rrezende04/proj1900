/*
 * Noms: Thierry Poulin & Renato Rezende
 * Description: Ce programme teste les reflexes de l'utilisateur.
 *              La lumiere LED allume vert au depart du programme pour
 *              indiquer que le programme fonctionne. Ensuite, apres un
 *              delai de 10 secondes, la lumiere clignote, indiquant a
 *              l'utilisateur qu'il peut maintenant appuyer pour tester
 *              ses reflexes. S'il appuye avant qu'une seconde s'ecoule
 *              apres le flash initial, la lumiere deviens rouge pour
 *              indiquer un echec. sinon la lumiere deviens verte.
 */

/*
Nous utilisons les ports PA1 (+) et PA0 (-) en sortie pour controler la LED
Nous utilisons le port PD2 comme entree du bouton poussoir * avec cavalier

En sortie sur (A1,A0), une LED de couleur:
    verte correspond a 01;
    rouge a 10;
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

bool getBtnPressState()
{
    return PIND & (1 << PD2);
}

// Initialize timer var and button press status
volatile uint8_t gMinuterieExpiree = 0;
volatile uint8_t gBoutonPoussoir = 0;
const uint16_t clkPreScaler = 1024;
const uint16_t reflexTime = F_CPU / clkPreScaler;

ISR(TIMER1_COMPA_vect)
{
    gMinuterieExpiree = 1;
}

ISR(INT0_vect)
{
    // délai avant de confirmer la réponse du bouton-poussoir
    _delay_ms(30);

    // se souvenir ici si le bouton est pressé ou relâché
    if (getBtnPressState())
        gBoutonPoussoir = 1;
    // Voir la note plus bas pour comprendre cette instruction et son rôle
    EIFR |= (1 << INTF0);
}

void partirMinuterie(uint16_t duree)
{

    gMinuterieExpiree = 0;

    // mode normal du timer 1 avec horloge divisée par 1024 (prescaler)
    // interruption après la durée spécifiée

    TCNT1 = 0; // initialisation timer
    OCR1A = duree;
    TCCR1A &= ~(1 << WGM11) & ~(1 << WGM10);                // 00 WGM
    TCCR1B &= ~(1 << WGM13) & ~(1 << WGM12) & ~(1 << CS11); // 00 -- x0x    00 pour indiquer WGMn3 et WGMn2 a 0 (controle de mode)
    TCCR1B |= (1 << CS12) | (1 << CS10);                    // xx -- 1x1    101 pour indiquer une valeur de prescaler a 1024
    TCCR1C = 0;
    TIMSK1 |= (1 << OCIE1A); // Timer Interrupt Mask OCIEA for timer compare interrupt
}

void initializePorts()
{
    cli();                 // prevents interrupts
    DDRA |= (1 << PA1);    // PA1 sortie
    DDRA |= (1 << PA0);    // PA0 sortie
    DDRD &= ~(1 << PD2);   // PD2 entree
    EIMSK |= (1 << INT0);  // External Interrupt Mask
    EICRA |= (1 << ISC00); // x1 mode detection de fronts
    EICRA |= (1 << ISC01); // 1x  montants seulement
    sei();                 // resume interrupt handling
}

int main()
{
    initializePorts();
    cli(); // button presses ignored until test starts
    turnLedGreen();
    _delay_ms(1000); // indicates program started correctly
    turnLedOff();
    _delay_ms(10000); // wait 10 seconds

    turnLedRed();
    _delay_ms(100);
    turnLedOff();

    partirMinuterie(reflexTime);
    sei(); // button presses taken into account
    do
    {
        // wait -_-
    } while (!gMinuterieExpiree && !gBoutonPoussoir);
    cli();
    if (gBoutonPoussoir)
        turnLedGreen();
    else
        turnLedRed();

    return 0;
}
