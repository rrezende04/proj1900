/*
 * Noms: Thierry Poulin & Renato Rezende
 *
 * Description: Faire tourner les roues a partir d'un signal PWM digital.
 *              La roue passera a travers tierNumbers paliers d'energie, chacun pour une duree de tierTimeS secondes.
 *              
 */

#define F_CPU 8000000
#define ON true
#define OFF false

#include <avr/io.h>
#include <util/delay.h>

const uint8_t delayResolutionUs = 20; // taille de delai
const uint8_t tierTimeS = 2; 
const uint8_t tierNumbers = 5;
uint8_t tierScaler = 100 / (tierNumbers - 1); // Trouver les paliers de PWM (dans ce cas : 25)

void turnLedGreen()     // 01
{
    PORTA &= ~(1 << PA1); // 0x
    PORTA |= (1 << PA0);  // x1
}

void turnMotor(bool onOff)
{
    if (onOff) PORTA |= (1 << PA2); // ON et OFF defini comme true et false au debut du fichier
    else PORTA &= ~(1 << PA2);
}

void delayUs(uint16_t Us)
{
    uint16_t cycles = Us / delayResolutionUs; //division entiere pour trouver combien de fois executer le delai
    for (uint16_t i = 0; i < cycles; i++) _delay_us(delayResolutionUs);
}

void PWM(uint8_t dutyCyclePercent, uint32_t periodUs) 
{
    // On trouve la duree en Us de tension haute (LED allumee) et basse (LED eteinte)
    uint16_t onTimeUs = periodUs * dutyCyclePercent / 100;
    uint16_t offTimeUs = periodUs - onTimeUs; // complement

    // Activate motor
    turnMotor(ON);
    delayUs(onTimeUs);

    // Deactivate motor
    turnMotor(OFF);
    delayUs(offTimeUs);
}

void gradualSpinUp(uint16_t frequencyHz, uint8_t powerDurationS)
{
    // frequence de frequencyHz sur powerDurationS secondes = nbr de cycles par palier
    uint16_t instrPerPower = frequencyHz * powerDurationS;  // chaque palier sera actif pour n iterations
    uint16_t totalInstr = instrPerPower * tierNumbers;      // nombre total d'iterations
    uint32_t periodUs = 1000000 / frequencyHz;
    for (uint16_t cycles = 0; cycles < totalInstr; cycles++) 
    {
        uint8_t dutyCyclePercent = (cycles / instrPerPower) * tierScaler;   // division entiere pour assurer
        PWM(dutyCyclePercent, periodUs);                                    // n iterations au niveau desire
    }
}

void initializePorts()
{
    DDRA |= (1 << PA0) | (1 << PA1);
    DDRA |= (1 << PA2) | (1 << PA3);
    PORTA |= (1 << PA3);
}

int main()
{
    initializePorts();
    turnLedGreen(); // Confirmation visuelle du demarrage du programme

    gradualSpinUp(60, tierTimeS);
    gradualSpinUp(400, tierTimeS);

    return 0;
}