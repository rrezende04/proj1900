#include "algorithm.h"

#define NO_READ_STOP true // TODO: Refactor goToNextIntersection for 3 cases
#define LEFT Util::Side::LEFT
#define RIGHT Util::Side::RIGHT
#define FORWARD Util::Dir::FORWARD
#define BACKWARD Util::Dir::BACKWARD
#define FROM_LEFT_TURN LEFT
#define FROM_RIGHT_TURN RIGHT
#define sendChar(10) endl

Algorithm::Algorithm(Robot& jerohm)
: bot_(&jerohm) {}


// MOVEMENT
void Algorithm::endMovement(Algorithm::movementEndType endType)
{
    switch (endType)
    {
        case movementEndType::KEEP_GOING:
            keepGoing();
            break;
        
        case movementEndType::GLIDE:
            bot_->stopMoving();
            break;
        
        case movementEndType::HARD_STOP:
            hardStop();
            break;
    }
}

// Only to keep things readable
void Algorithm::keepGoing() {}

// Mainly used for debouncing
void Algorithm::glide() { bot_->stopMoving(); }

void Algorithm::hardStop()
{
    static constexpr uint8_t REVERSE_TIME_MS = 60;

    bot_->invertDirection(Motor::MAX_POWER);
    _delay_ms(REVERSE_TIME_MS);
    // Reset orientation of motors (control pins)
    // and stop moving
    bot_->invertDirection(0x00);
    _delay_ms(INERTIA_WAIT_MS);
}



void Algorithm::adjust(Util::Side fromTurnSide, bool firstCall)
{
    if (bot_->s5())
    {
        bot_->turn(RIGHT, Motor::MAX_POWER);
        _delay_ms(BURST_TIME_MS);
        endMovement(movementEndType::KEEP_GOING);

        bot_->turn(RIGHT);
        while(bot_->s5()) { continue; }
    }
    else if (bot_->s1())
    {
        bot_->turn(LEFT, Motor::MAX_POWER);
        _delay_ms(BURST_TIME_MS);
        endMovement(movementEndType::KEEP_GOING);

        bot_->turn(LEFT);
        while(bot_->s1()) { continue; }
    }

    // If we are lost, use the information of the last turn direction
    // to try to recover
    else if (bot_->readLineMaker() == 0)
    {
        switch (fromTurnSide)
        {
            case FROM_LEFT_TURN:
                turn(RIGHT);
                break;
            case FROM_RIGHT_TURN:
                turn(LEFT);
                break;
        }
    }

    // Once it adjusts, it will call 
    // itself for a second adjustement
    if (firstCall)
        adjust(fromTurnSide, false);
}

void Algorithm::turn(Util::Side side, uint8_t power)
{
    // Small MAX_POWER burst to overcome inertia
    bot_->turn(side, Motor::MAX_POWER);
    _delay_ms(BURST_TIME_MS);
    endMovement(movementEndType::KEEP_GOING);

    bot_->turn(side, power);
    while (bot_->readLineMaker() != 0) { continue; }
    while (!bot_->s3()) { continue; }
}

void Algorithm::turnAndGo(uint8_t power)
{
    turn(RIGHT);
    endMovement(movementEndType::HARD_STOP);

    traverseDiagonal(power);
}

void Algorithm::inchForward(uint8_t power)
{
    static constexpr uint8_t  STRAIGHT_LOOP_TOP = 90;
    static constexpr uint8_t  FOLLOW_LOOP_TOP   = 170;
    static constexpr uint16_t DELAY_TIME_MS     = 5;

    for (uint8_t i = 0; i < STRAIGHT_LOOP_TOP; i++)
    {
        bot_->move(FORWARD, power);
        Util::wait_ms(DELAY_TIME_MS);
    }
    endMovement(movementEndType::KEEP_GOING);

    for (uint8_t i = 0; i < FOLLOW_LOOP_TOP; i++)
    {
        bot_->followLine(power);
        Util::wait_ms(DELAY_TIME_MS);
    }
}

// TODO: Extract scanning from goToNextIntersection and refactor everywhere
void Algorithm::goToNextIntersection(uint8_t power) 
{
    while(!(bot_->s3() && (bot_->s1() | bot_->s5())))
    { bot_->followLine(power); }
}

void Algorithm::goPastNextIntersection(uint8_t power)
{
    goToNextIntersection(power);
    endMovement(movementEndType::KEEP_GOING);

    inchForward();
}

void Algorithm::executeVShape(uint8_t power)
{
    turn(LEFT);
    endMovement(movementEndType::HARD_STOP);

    goToNextIntersection(power);
    endMovement(movementEndType::KEEP_GOING);

    inchForward();
    endMovement(movementEndType::HARD_STOP);

    turn(RIGHT);
    endMovement(movementEndType::HARD_STOP);

    goToNextIntersection(power);
}

void Algorithm::traverseDiagonal(uint8_t power)
{
    goToNextIntersection(power);
    endMovement(movementEndType::KEEP_GOING);

    inchForward();
    endMovement(movementEndType::KEEP_GOING);

    goToNextIntersection(power);
}

void Algorithm::turnAround(uint8_t power)
{
    static constexpr uint16_t REVERSE_TIME_MS = 800;

    bot_->move(BACKWARD, power);
    _delay_ms(REVERSE_TIME_MS);
    endMovement(movementEndType::KEEP_GOING);
    
    turn(LEFT);
}


// SCANNING

uint8_t Algorithm::getSignature() 
{
    uint8_t signature = 0;
    static constexpr uint8_t BIT_SHIFT = 1;
    static constexpr uint8_t LOOP_TOP  = 30;
    // Documentation: Refresh rate = 200 Hz --> Refresh period = 5 ms
    static constexpr uint8_t DELAY_RES_MS = 5;

    for (uint8_t i = 0; i < LOOP_TOP; i++)
    {
        signature |= (bot_->s1() << BIT_SHIFT) | (bot_->s5());
        _delay_ms(DELAY_RES_MS);
    }

    return signature;
}

///////////////////////// TASK 1 /////////////////////////

void Algorithm::performTaskOne(uint8_t power)
{
    static constexpr uint8_t SIGNATURE_SIZE = 2;
    
    uint8_t code = 0;

    code |= scan(power) << SIGNATURE_SIZE;
    taskOneScanSetup(power);
    endMovement(movementEndType::HARD_STOP);

    code |= scan(power);

    if (code == 0b1111)
    {
        taskOneScanSetup(power);
        endMovement(movementEndType::HARD_STOP);
        code |= scan(power) << (2 * SIGNATURE_SIZE);
    }

    inchForward();
    endMovement(movementEndType::HARD_STOP);

    switch (code)
    {
        case 0b001101: // started A1 --> ended A2
            writeResultsToMemory(Node::A, 1);
            pathfindFromA2(power);
            bot_->turnLed(Led::Colour::GREEN);
            break;
        case 0b000111: // started A2 --> ended A4
            writeResultsToMemory(Node::A, 2);
            pathfindFromA4(power);
            bot_->turnLed(Led::Colour::AMBER);
            break;
        case 0b011111: // started A3 --> ended A2
            writeResultsToMemory(Node::A, 3);
            pathfindFromA2(power);
            bot_->turnLed(Led::Colour::GREEN);
            break;
        case 0b111111: // started A4 --> ended A1
            writeResultsToMemory(Node::A, 4);
            pathfindFromA1(power);
            bot_->turnLed(Led::Colour::GREEN);
            break;
        case 0b001001: // started B3 --> ended B4
            writeResultsToMemory(Node::B, 3);
            pathfindFromB4(power);
            bot_->turnLed(Led::Colour::GREEN);
            break;
        case 0b000110: // started B4 --> ended B6
            writeResultsToMemory(Node::B, 4);
            pathfindFromB6(power);
            bot_->turnLed(Led::Colour::AMBER);
            break;
        case 0b001110: // started B5 --> ended B3
            writeResultsToMemory(Node::B, 5);
            pathfindFromB3(power);
            bot_->turnLed(Led::Colour::GREEN);
            break;
        case 0b001011: // started B6 --> ended B5
            writeResultsToMemory(Node::B, 6);
            pathfindFromB5(power);
            bot_->turnLed(Led::Colour::AMBER);
            break;
        default:
            writeResultsToMemory(Node::Error, 0);
            bot_->turnLed(Led::Colour::OFF);
            return;
    }
    endMovement(movementEndType::HARD_STOP);
    emitEndSound();
}

void Algorithm::taskOneScanSetup(uint8_t power)
{
    turnAround(power);
    endMovement(movementEndType::HARD_STOP);

    goToNextIntersection(power);
    endMovement(movementEndType::KEEP_GOING);

    inchForward();
    endMovement(movementEndType::HARD_STOP);

    turn(RIGHT);
}

uint8_t Algorithm::scan(uint8_t power)
{
    goToNextIntersection();
    endMovement(movementEndType::GLIDE);
    // Debounced because gliding
    return getSignature();
}

void Algorithm::pathfindFromA1(uint8_t power) // To C
{ executeVShape(power); }

void Algorithm::pathfindFromA2(uint8_t power) // To C
{ turnAndGo(power); }

void Algorithm::pathfindFromA4(uint8_t power) // To E
{
    executeVShape(power);
    endMovement(movementEndType::KEEP_GOING);

    inchForward();
    endMovement(movementEndType::HARD_STOP);

    executeVShape(power);
}

void Algorithm::pathfindFromB3(uint8_t power) // To C
{
    executeVShape(power);
    endMovement(movementEndType::KEEP_GOING);

    inchForward();
    endMovement(movementEndType::HARD_STOP);

    executeVShape(power);
}

void Algorithm::pathfindFromB4(uint8_t power) // To C
{
    turnAndGo(power);
    endMovement(movementEndType::KEEP_GOING);

    inchForward();
    endMovement(movementEndType::HARD_STOP);

    executeVShape(power);
}

void Algorithm::pathfindFromB5(uint8_t power) // To E
{ turnAndGo(power); }

void Algorithm::pathfindFromB6(uint8_t power) // To E
{ executeVShape(power); }

void Algorithm::writeResultsToMemory(Node startNode, uint8_t direction)
{
    static constexpr uint8_t  RESULTS_SIZE = 4;
    Memoire24CXXX mem;

    char* startConfiguration = toResultsString(startNode, direction);
    mem.ecriture(RESULTS_START_ADDRESS, (unsigned char*)startConfiguration, RESULTS_SIZE);
}

char* Algorithm::toResultsString(Node startNode, uint8_t direction)
{
    switch (startNode)
    {
        case Node::A:
            switch (direction)
            {
                case 1:
                    return (char*)"ANOC";
                case 2:
                    return (char*)"ASOE";
                case 3:
                    return (char*)"ANEC";
                case 4:
                    return (char*)"ASEC";
                default:
                    return (char*)"XXXX";
            }
        case Node::B:
            switch (direction)
            {
                case 3:
                    return (char*)"BNOC";
                case 4:
                    return (char*)"BSOE";
                case 5:
                    return (char*)"BNEC";
                case 6:
                    return (char*)"BSEE";
                default:
                    return (char*)"XXXX";
            }
        default:
            return (char*)"XXXX";
    }
}



///////////////////////// TASK 2 /////////////////////////

bool Algorithm::taskTwoPartZero(uint8_t power)
{
    static constexpr bool ZEROTH = true;

    bool hasTurned = false;
    goPastNextIntersection(power);
    endMovement(movementEndType::HARD_STOP);

    // 1st parameter has no impact
    hasTurned = orientToPole(LEFT, ZEROTH);

    goToPole();
    endMovement(movementEndType::HARD_STOP);

    emitPoleDetectionSound();
    return hasTurned;
}

uint8_t Algorithm::taskTwoPartOne(bool& hasTurned, uint8_t power)
{
    static constexpr uint8_t  ASCII_NUM_SHIFT = 48;

    Memoire24CXXX mem;
    Node startNode = Node::Error;

    uint8_t tempSignature = 0;
    uint8_t poleLocation  = -1;
    uint8_t pole2Location = -1;
    Util::Side nextTurnSide = RIGHT;

    // Go past and read next intersection
    goToNextIntersection(power);
    endMovement(movementEndType::GLIDE);
    
    tempSignature = getSignature();

    inchForward();
    endMovement(movementEndType::HARD_STOP);

    if (tempSignature == 0b01)
    {
        poleLocation = 1;
        startNode = hasTurned ? Node::C : Node::D;
        // nextTurnSide = RIGHT;
    }
    else if (tempSignature == 0b11)
    {
        poleLocation = 2;
        startNode = hasTurned ? Node::D : Node::C;
        nextTurnSide = LEFT;
    }

    mem.ecriture(START_ADDRESS_T2P1, startNode);
    _delay_ms(Util::DEBOUNCE_TIME_MS);
    mem.ecriture(START_ADDRESS_T2P1 + 1, poleLocation + ASCII_NUM_SHIFT);

    turn(nextTurnSide);
    endMovement(movementEndType::HARD_STOP);

    goPastNextIntersection(power);
    endMovement(movementEndType::HARD_STOP);

    // Coming from 1, reorientation will be on your left
    // Coming from 2, reorientation will be on your right
    hasTurned = orientToPole(poleLocation == 1 ? LEFT : RIGHT);

    if (poleLocation == 1)
        pole2Location = hasTurned ? 3 : 4;
    else if (poleLocation == 2)
        pole2Location = hasTurned ? 4 : 3;
        
    mem.ecriture(START_ADDRESS_T2P1 + 2, pole2Location + ASCII_NUM_SHIFT);

    goToPole();
    endMovement(movementEndType::HARD_STOP);

    emitPoleDetectionSound();
    return pole2Location;
}

uint8_t Algorithm::taskTwoPartTwo(bool& hasTurned, uint8_t poleLocationContainer, uint8_t power)
{
    static constexpr uint8_t  ASCII_NUM_SHIFT = 48;

    Memoire24CXXX mem;
    uint8_t newPole = -1;

    goPastNextIntersection(power);
    endMovement(movementEndType::HARD_STOP);

    if (poleLocationContainer == 3)
    {
        turn(RIGHT);
        endMovement(movementEndType::HARD_STOP);
    }
    else if (poleLocationContainer == 4)
    {
        turn(LEFT);
        endMovement(movementEndType::HARD_STOP);
    }

    goPastNextIntersection(power);
    endMovement(movementEndType::HARD_STOP);

    // Coming from 3, reorientation will be on your left
    // Coming from 4, reorientation will be on your right
    hasTurned = orientToPole(poleLocationContainer == 3 ? LEFT : RIGHT);

    if (poleLocationContainer == 3)
        newPole = hasTurned ? 5 : 6;
    else if (poleLocationContainer == 4)
        newPole = hasTurned ? 6 : 5;

    mem.ecriture(START_ADDRESS_T2P2, newPole + ASCII_NUM_SHIFT);

    goToPole();
    endMovement(movementEndType::HARD_STOP);

    emitPoleDetectionSound();
    return newPole;
}

void Algorithm::taskTwoPartThree(uint8_t poleLocationContainer, uint8_t power)
{
    goPastNextIntersection(power);
    endMovement(movementEndType::HARD_STOP);

    // From 5 or from 6
    poleLocationContainer == 5 ? turnAndGo(power) : executeVShape(power);
    endMovement(movementEndType::HARD_STOP);

    emitEndSound();
}

void Algorithm::performTaskTwoStep(uint8_t power)
{
    static uint8_t numberOfExecutions = 0;
    static uint8_t poleLocationContainer = 0;

    static bool hasTurned = false;
    
    
    switch (numberOfExecutions)
    {
        case 0:
            hasTurned = taskTwoPartZero(power);
            break;
        case 1:
            poleLocationContainer = taskTwoPartOne(hasTurned, power);            
            break;
        case 2:
            poleLocationContainer = taskTwoPartTwo(hasTurned, poleLocationContainer, power);
            break;
        case 3:
            taskTwoPartThree(poleLocationContainer, power);
            break;
        default:
            break;
    }

    numberOfExecutions++;
}


bool Algorithm::orientToPole(Util::Side reorientationSide, bool zeroth)
{
    if (findPole())
        return false;
    else if (zeroth)
    {
        turn(LEFT);
        endMovement(movementEndType::HARD_STOP);
        if (!findPole())
        {
            turn(RIGHT);
            endMovement(movementEndType::KEEP_GOING);
            turn(RIGHT);
            endMovement(movementEndType::HARD_STOP);
        }
        return true;
    }

    // Fallthrough: reorientation necessary, but without search
    turn(reorientationSide);
    endMovement(movementEndType::HARD_STOP);
    return true;
}

bool Algorithm::findPole()
{
    static constexpr uint8_t SCAN_TURN_RESOLUTION_TIME_MS   = 20;
    static constexpr uint8_t STALL_FOR_NEXT_MEASURE_TIME_MS =
        Infrared::READ_DELAY_MS > SCAN_TURN_RESOLUTION_TIME_MS ? Infrared::READ_DELAY_MS - SCAN_TURN_RESOLUTION_TIME_MS : 2;
    static constexpr uint8_t POSITIVE_READ_THRESHOLD_COUNT  = 3;


    uint8_t positiveReadCount = 0;

    // Prepare for scanning by angling to the left a bit
    if(!bot_->s5())
    {
        bot_->turn(LEFT, Motor::MAX_POWER);
        _delay_ms(BURST_TIME_MS);
        endMovement(movementEndType::KEEP_GOING);

        bot_->turn(LEFT);
        while (!bot_->s5()) { continue; }
        endMovement(movementEndType::HARD_STOP);
    }

    // Scan while turning right
    while (!bot_->s1() && !(positiveReadCount >= POSITIVE_READ_THRESHOLD_COUNT))
    {
        bot_->turn(RIGHT, Motor::MAX_POWER);
        _delay_ms(SCAN_TURN_RESOLUTION_TIME_MS);
        endMovement(movementEndType::GLIDE); // Keep some momentum going

        if (bot_->readDistanceCm() < POLE_DETECTION_THRESHOLD_CM)
            positiveReadCount++;
        _delay_ms(STALL_FOR_NEXT_MEASURE_TIME_MS);
    }
    endMovement(movementEndType::HARD_STOP);
    
    // Was a pole there?
    return positiveReadCount >= POSITIVE_READ_THRESHOLD_COUNT;
}

void Algorithm::goToPole(uint8_t power) 
{
    // TODO: debounce
    while (bot_->readDistanceCm() > POLE_CLOSE_THRESHOLD_CM)
    { bot_->followLine(power); }
}



void Algorithm::emitPoleDetectionSound()
{
    static constexpr uint8_t SOUND_COUNT  = 8;
    static constexpr uint8_t MIDI_NOTE    = 48;
    static constexpr uint8_t PLAY_TIME_MS = 150;
    static constexpr uint8_t STOP_TIME_MS = 50;

    for (uint8_t i = 0; i < SOUND_COUNT; i++)
    {
        bot_->playNote(MIDI_NOTE);
        _delay_ms(PLAY_TIME_MS);
        bot_->stopNote();
        _delay_ms(STOP_TIME_MS);
    }
}

void Algorithm::emitEndSound() 
{
    static constexpr uint8_t  SOUND_COUNT  = 2;
    static constexpr uint8_t  MIDI_NOTE    = 84;
    static constexpr uint16_t PLAY_TIME_MS = 750;
    static constexpr uint8_t  STOP_TIME_MS = 50;

    for (uint8_t i = 0; i < SOUND_COUNT; i++)
    {
        bot_->playNote(MIDI_NOTE);
        _delay_ms(PLAY_TIME_MS);
        bot_->stopNote();
        _delay_ms(STOP_TIME_MS);
    }
}

void Algorithm::printReportPartOne()
{

    uint8_t currentAddress = RESULTS_START_ADDRESS;

    char titleTaskOne[] 	 = {'I','d','e','n','t','i','f','i','c','a','t','i','o','n',' ','d','e',' ','l','\'','e','x','t','r','e','m','i','t','e'};
	char separatorTaskOne[]	 = {'-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-'};
	char startingPoint[]     = {'P','o','i','n','t',' ','d','e',' ','d','e','p','a','r','t',' ',':',' '};
	char startingDirection[] = {'O','r','i','e','n','t','a','t','i','o','n',' ','d','e',' ','d','e','p','a','r','t',' ',':',' '};
	char foundExtremities[]  = {'E','x','t','r','e','m','i','t','e',' ','t','r','o','u','v','e','e',' ',':',' '};

    // Title
    sendString(sizeof(titleTaskOne), (unsigned char*)titleTaskOne); endl;
	sendString(sizeof(separatorTaskOne), (unsigned char*)separatorTaskOne); endl;

    // Starting point
	sendString(sizeof(startingPoint), (unsigned char*)startingPoint);
	printFromMemory(currentAddress++); endl; //starting point Pb1

    // Starting direction
    sendString(sizeof(startingDirection), (unsigned char*)startingDirection);
	printFromMemory(currentAddress++); //starting direction
	_delay_ms(Util::MEMORY_DELAY_MS);
	printFromMemory(currentAddress++); endl;

    // Extremities found
	sendString(sizeof(foundExtremities), (unsigned char*)foundExtremities);
	printFromMemory(currentAddress); endl; //found extremities
}

void Algorithm::printReportPartTwo()
{
    uint8_t currentAddress = START_ADDRESS_T2P1;

	char titleTaskTwo[] 	 = {'T','r','a','v','e','r','s','e','e',' ','d','u',' ','p','a','r','c','o','u','r','s'};
	char separatorTaskTwo[]  = {'-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-','-'};
	char firstPole[]         = {'P','o','i','n','t',' ','d','u',' ','p','o','t','e','a','u',' ','1',' ',':',' '};
	char secondPole[]        = {'P','o','i','n','t',' ','d','u',' ','p','o','t','e','a','u',' ','2',' ',':',' '};
	char thirdPole[]         = {'P','o','i','n','t',' ','d','u',' ','p','o','t','e','a','u',' ','3',' ',':',' '};

    // Title
    sendString(sizeof(titleTaskTwo), (unsigned char*)titleTaskTwo); endl;
	sendString(sizeof(separatorTaskTwo), (unsigned char*)separatorTaskTwo); endl;
    
    // Starting Point
    sendString(sizeof(startingPoint), (unsigned char*)startingPoint);
	printFromMemory(currentAddress++); endl;

    // First pole
	sendString(sizeof(firstPole), (unsigned char*)firstPole);
	printFromMemory(currentAddress++); endl;

    // Second pole
	sendString(sizeof(secondPole), (unsigned char*)secondPole);
	printFromMemory(currentAddress++); endl;

    // Third pole
	sendString(sizeof(thirdPole), (unsigned char*)thirdPole);
	printFromMemory(currentAddress);
}

void Algorithm::printSignature()
{
    static constexpr char SIGNATURE[] = {'2','8','3','4',' ','-',' ','J','e','r','-','O','h','m'};
    sendString(sizeof(SIGNATURE), (unsigned char*)SIGNATURE);
}

void Algorithm::performTaskThree()
{
    printReportPartOne();
    printReportPartTwo();
    printSignature();
}
