/*
 * Noms: Thierry Poulin & Renato Rezende
 *
 * Description: Faire tourner les roues a partir d'un signal PWM digital.
 *              La roue passera a travers TIER_COUNT paliers d'energie, chacun pour une duree de TIME_PER_TIER_S
 * secondes.
 *
 */

#define F_CPU 8000000
#define ON true
#define OFF false

#include <avr/io.h>
#include <util/delay.h>

static constexpr uint8_t DELAY_RESOLUTION_US = 20;
static constexpr uint8_t ONE_SECOND_US       = 1000000;
static constexpr uint8_t TIME_PER_TIER_S     = 2;
static constexpr uint8_t TIER_COUNT          = 5;

static constexpr uint8_t TIER_SCALER = 100 / (TIER_COUNT - 1);

void turnLedGreen() {
    PORTA &= ~(1 << PA1);
    PORTA |= (1 << PA0);
}

void turnMotor(bool on) {
    if (on)
        PORTA |= (1 << PA2);
    else
        PORTA &= ~(1 << PA2);
}

void delayUs(uint16_t delayTimeUs) {
    uint16_t cycles = delayTimeUs / DELAY_RESOLUTION_US;

    for (uint16_t i = 0; i < cycles; i++)
        _delay_us(DELAY_RESOLUTION_US);
}

void PWM(uint8_t dutyCyclePercent, uint32_t periodUs) {
    uint16_t onTimeUs  = periodUs * dutyCyclePercent / 100;
    uint16_t offTimeUs = periodUs - onTimeUs;

    turnMotor(ON);
    delayUs(onTimeUs);

    turnMotor(OFF);
    delayUs(offTimeUs);
}

void gradualSpinUp(uint16_t frequencyHz, uint8_t powerDurationS) {
    uint16_t instrPerPower = powerDurationS * frequencyHz;
    uint16_t totalInstr    = instrPerPower * TIER_COUNT;
    uint32_t periodUs      = ONE_SECOND_US / frequencyHz;

    for (uint16_t cycles = 0; cycles < totalInstr; cycles++) {
        uint8_t dutyCyclePercent = (cycles / instrPerPower) * TIER_SCALER;
        PWM(dutyCyclePercent, periodUs);
    }
}

void initializePorts() {
    DDRA |= (1 << PA0) | (1 << PA1);
    DDRA |= (1 << PA2) | (1 << PA3);
    PORTA |= (1 << PA3);
}

int main() {
    initializePorts();
    turnLedGreen();

    gradualSpinUp(60, TIME_PER_TIER_S);
    gradualSpinUp(400, TIME_PER_TIER_S);

    return 0;
}