#include "interpretor.h"
#include "util.h"

void Interpretor::instructionSwitch(uint8_t instruction, uint8_t operande, uint32_t& adresse)
{
    switch(instruction)
    {
        case Commandes::dbt :
            debut = true;
            break;
        case Commandes::att :
            if(debut)
                att_f(operande);
            break;
        case Commandes::dal :
            if(debut)
                dal_f(operande);
            break;
        case Commandes::det :
            if(debut)
                det_f();
            break;
        case Commandes::sgo :
            if(debut)
                sgo_f(operande);
            break;
        case Commandes::sar :
            if(debut)
                sar_f();
            break;
        case Commandes::mar0 :
            if(debut)
                mar_f();
            break;
        case Commandes::mar1 :
            if(debut)
                mar_f();
            break;
        case Commandes::mav :
            if(debut)
                mav_f(operande);
            break;
        case Commandes::mre :
            if(debut)
                mre_f(operande);
            break;
        case Commandes::trd :
            if(debut)
                trd_f();
            break;
        case Commandes::trg :
            if(debut)
                trg_f();
            break;
        case Commandes::dbc :
            if(debut)
                dbc_f(operande, adresse);
            break;
        case Commandes::fbc :
            if(debut)
                fbc_f(adresse);
            break;
        case Commandes::fin :
            if(debut)
                fin_f(adresse);
            break;
    }
}

void Interpretor::att_f(uint8_t operande) 
{
    const uint16_t DELAY_TIME = DELAY_SCALER * operande;
    Util::wait_ms(DELAY_TIME);
}

void Interpretor::dal_f(uint8_t operande)
{
    if(operande == 1)
        led.turnGreen();
    else if(operande == 2)
        led.turnRed();
}

void Interpretor::det_f() { led.turnOff(); }

void Interpretor::sgo_f(uint8_t operande) { piezo.playNote(operande); }

void Interpretor::sar_f() { piezo.stopPlaying(); }

void Interpretor::mar_f()
{
    motorLeft.turnOff();
    motorRight.turnOff();
}

void Interpretor::mav_f(uint8_t operande)
{
    motorLeft.setForwardMode();
    motorRight.setForwardMode();
    motorLeft.setPower(operande);
    motorRight.setPower(operande);
}

void Interpretor::mre_f(uint8_t operande)
{
    motorLeft.setBackwardMode();
    motorRight.setBackwardMode();
    motorLeft.setPower(operande);
    motorRight.setPower(operande);
}

void Interpretor::trd_f()
{
    motorLeft.setForwardMode();
    motorRight.setBackwardMode();
    motorLeft.setPower( Motor::MEDIUM_POWER);
    motorRight.setPower(Motor::MEDIUM_POWER);
    att_f(TURN_90_TIME);
    mar_f();
}

void Interpretor::trg_f()
{
    motorLeft.setBackwardMode();
    motorRight.setForwardMode();
    motorLeft.setPower( Motor::MEDIUM_POWER);
    motorRight.setPower(Motor::MEDIUM_POWER);
    att_f(TURN_90_TIME);
    mar_f();
}

void Interpretor::dbc_f(uint8_t operande, uint32_t adresse)
{
        // On retournera a l'instr suivante
        // pour eviter de reassigner les variables
    loopStartAddress = adresse;
    loopCounter      = operande + 1;
}

void Interpretor::fbc_f(uint32_t& adresse)
{
    loopCounter--;
    if(loopCounter != 0)
        adresse = loopStartAddress;
}

    // adresse (la variable de boucle i par reference)
    // deviens l'entier (2^32)-1
    // condition de boucle : i < -2 donc i < (2^32) -2 
void Interpretor::fin_f(uint32_t& adresse) { adresse = -1; }