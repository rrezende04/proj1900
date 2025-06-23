/*
 * Noms: Thierry Poulin & Renato Rezende
 *
 * Description: Eteindre une LED graduellement sur une duree
 *              de temps donnee, en utilisant un signal PWM logiciel
 *              Nous utilisons les ports PA1 (+) et PA0 (-) en sortie pour controler la LED
 *              Lorsqu'elle est allumee, la LED sera toujours de couleur rouge, i.e. 0x10              
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

static constexpr uint8_t  TOTAL_GRADIENT_TIME_S = 3;
static constexpr uint8_t  DELAY_RESOLUTION_US   = 10;

static constexpr uint16_t FREQUENCY_HZ     = 1000;
static constexpr uint32_t N_US_PER_SECOND  = 1000000;
static constexpr uint16_t PERIOD_US        = N_US_PER_SECOND / FREQUENCY_HZ; // Period (b) in microseconds
static constexpr uint16_t NUMBER_OF_CYCLES = TOTAL_GRADIENT_TIME_S * FREQUENCY_HZ;
//const uint8_t cycleTime = 1;

void initializePorts()
{   
    DDRA |= (1 << PA0);
    DDRA |= (1 << PA1);
}

void turnLedOff()       
{
    PORTA &= ~(1 << PA1);
    PORTA &= ~(1 << PA0);
}

void turnLedGreen()    
{
    PORTA &= ~(1 << PA1);
    PORTA |=  (1 << PA0); 
}

void turnLedRed()       
{
    PORTA |=  (1 << PA1);  
    PORTA &= ~(1 << PA0); 
}

void delayUs(uint16_t delayTimeUs)
{
    uint16_t nCycles = delayTimeUs / DELAY_RESOLUTION_US;

    for (uint16_t i = 0; i < nCycles; i++) 
        _delay_us(DELAY_RESOLUTION_US);
}

void executePwmCycle(uint8_t dutyCyclePercent)
{
    uint16_t onTimeUs = dutyCyclePercent * periodUs / 100;
    uint16_t offTimeUs = 1000 - onTimeUs; // complement

    turnLedRed();
    delayUs(onTimeUs);

    turnLedOff();
    delayUs(offTimeUs);
}

void turnLedOffSlowly()
{
    for (uint32_t pwmCycles = NUMBER_OF_CYCLES ; pwmCycles > 0; pwmCycles--)
    {
        uint8_t pwmPercentage = pwmCycles * 100 / NUMBER_OF_CYCLES;
        executePwmCycle(pwmPercentage);
    }
}

int main()
{
    initializePorts();
    
    turnLedOff();  
    turnLedRed();
    turnLedOffSlowly();
    
    return 0;
}