#include "lineMaker.h"

LineMaker::LineMaker(Pin& pin0,
                     Pin& pin1, 
                     Pin& pin2, 
                     Pin& pin3, 
                     Pin& pin4)
: pin0_(&pin0)
, pin1_(&pin1)
, pin2_(&pin2)
, pin3_(&pin3)
, pin4_(&pin4)
{
    pin0_->setReadMode();
    pin1_->setReadMode();
    pin2_->setReadMode();
    pin3_->setReadMode();
    pin4_->setReadMode();

    // Activer les interruptions sur les pins concernées
    // On assume ici que ce seront les pins C2 à C6

    // Active les interruptions sur les pins du Port C
    // cli();
    // PCICR  |= (1 << PCIE2  );
    // PCMSK2 |= (1 << PCINT18) | (1 << PCINT22);// | (1 << PCINT20) | (1 << PCINT21) | (1 << PCINT19);
    // sei();
}

bool LineMaker::isS1() { return pin0_->read(); }
bool LineMaker::isS2() { return pin1_->read(); }
bool LineMaker::isS3() { return pin2_->read(); }
bool LineMaker::isS4() { return pin3_->read(); }
bool LineMaker::isS5() { return pin4_->read(); }

uint8_t LineMaker::read()
{
    uint8_t i = 0;
    i |= (isS1() << S1) 
      |  (isS2() << S2)
      |  (isS3() << S3)
      |  (isS4() << S4)
      |  (isS5() << S5);
    return i;
}

bool LineMaker::isThereLine() { return (isS1() || isS2() || isS3() || isS4() || isS5()); } // Ordre pour limiter les get

bool LineMaker::isThereT() 
{
    static constexpr uint8_t DELAY_RES  = 2; 
    static constexpr uint8_t LOOP_TOP   = 100;
    static constexpr uint8_t COMP_VALUE = 0b11;
    uint8_t runningRead = 0; 

    for (uint8_t i = 0; i < LOOP_TOP; i++) 
    {
        runningRead |= (isS1() << 1) | (isS5());
        _delay_ms(DELAY_RES);
    }
    return (runningRead == COMP_VALUE);
}
