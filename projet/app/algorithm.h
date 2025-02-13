#pragma once

#include <avr/io.h>
#include "memoire_24.h"
#include "robot.h"
#include "util.h"

// // Figure 1: The environment the robot evolves in.
// /*
//     C      @@                      1     @@                    @@    3                      @@    5                                                
//         @@@                           @@@                        @@@                          @@@                                                  
//       @@@@                          @ @                           @ @                           @ @                                                
//      @    @                        @    @                       @@   @@                       @@   @                                               
//     @      @                     @@      @                     @       @                     @       @                                             
//             @@                  @         @@                  @         @                  @@         @                                            
//               @                @            @               @@            @               @            @@                                          
//                @             @               @             @               @             @               @                                         
//                  @          @                 @@          @                 @          @@                 @                                        
//                   @       @@                    @   A   @                     @   B   @                    @@       @                              
//                    @@    @                       @     @                       @     @                       @    @@                               
//                      @  @                         @@ @@                         @@ @@                         @  @                                 
//                       @                             @                             @                             @                                  
//                      @  @                          @ @@                         @@ @                          @  @                                 
//                    @@    @                       @     @                       @     @                       @    @@                               
//                   @       @@                    @       @                     @       @                     @       @                              
//                  @          @                 @@          @                 @@         @@                 @          @                             
//                @             @               @             @               @             @               @             @                           
//               @               @@            @               @             @               @            @@               @                          
//              @                  @         @@                  @         @                  @@         @                  @                         
//     @      @                     @@      @                     @       @                     @       @                     @      @                
//      @    @                        @    @                       @@   @@                       @    @                        @    @                 
//       @@@@                          @ @                           @ @                           @ @                          @@ @                  
//         @@                         @@@                           @@@                             @@@                        @@@                    
//     D      @@                   @@     2                      @@     4                         6     @@                   @@     E                 
// */

class Algorithm
{
public:
/*///////////////////// PUBLIC INTERFACE ///////////////////*/

	Algorithm(Robot& jerohm);

    void performTaskOne(uint8_t power = Motor::PRESENTATION_POWER);

	// Perform one step of task 2
	// 4 steps total need to be executed, 1 every time the button is pressed
    void performTaskTwoStep(uint8_t power = Motor::PRESENTATION_POWER);

	void performTaskThree();

	// Robot stored by reference
	Robot* bot_;
	static constexpr uint16_t RESULTS_START_ADDRESS = 1;
	static constexpr uint16_t T1_RESULTS_SIZE		= 4;
	static constexpr uint16_t T2P1_RESULTS_SIZE		= 3;
	static constexpr uint16_t START_ADDRESS_T2P1    = RESULTS_START_ADDRESS + T1_RESULTS_SIZE;
	static constexpr uint16_t START_ADDRESS_T2P2    = START_ADDRESS_T2P1 + T2P1_RESULTS_SIZE;

private:
/*///////////////////////// GENERAL ////////////////////////*/

	enum Node {Error = 88, A = 65, B, C, D, E};

	static constexpr uint8_t INERTIA_WAIT_MS = 100;
	static constexpr uint8_t BURST_TIME_MS = 30;

	// 10 cm +/- 1 cm, so this is fine
	// Plus it helps to stop in the right range, b/c we will sometimes
	// lose sight of the pole while moving, etc.
	static constexpr uint8_t POLE_CLOSE_THRESHOLD_CM     = 11;
    static constexpr uint8_t POLE_DETECTION_THRESHOLD_CM = 30;

	static_assert(POLE_CLOSE_THRESHOLD_CM < POLE_DETECTION_THRESHOLD_CM,
		"Nearby poles need to be closer by than further poles.");

// MOVEMENT

	enum class movementEndType {KEEP_GOING, GLIDE, HARD_STOP};

	// CALL THIS AFTER EVERY MOVEMENT FUNCTION IN Algorithm
		// Exception: When creating a new movement function in Algorithm,
		// 			  do not call for the last sub-movement and only that one
	void endMovement(movementEndType endType);
	// Keeps the wheels going as they are
	// Only to keep things readable
	void keepGoing();
	// Stops actively powering the wheels
	// Mainly used for debouncing
	void glide();
	// Stops on the spot
	void hardStop();

	// Adjust on axis
	void adjust(Util::Side fromTurnSide, bool firstCall = true);

	void turn(Util::Side side, uint8_t power = Motor::LOW_POWER);

	// Turns right, go across
	void turnAndGo(uint8_t power = Motor::LOW_POWER);
	// Reach next intersection
	void goToNextIntersection(uint8_t power = Motor::LOW_POWER);
	// Reach a little past next intersection
	void goPastNextIntersection(uint8_t power = Motor::LOW_POWER);
	// Left, straight, right, straight
	void executeVShape(uint8_t power = Motor::LOW_POWER);
	// Straight ahead through an intersection
	void traverseDiagonal(uint8_t power = Motor::LOW_POWER);
	// Small move forward, usually past an intersection point
	void inchForward(uint8_t power = Motor::LOW_POWER);
	// 180 degrees
	void turnAround(uint8_t power = Motor::LOW_POWER);


// SCANNING
	// Return intersection signature (2 bit code)
	// Takes some time to execute
	// To debounce, call after gliding from previous movement
	// bit 0 : 0 = no path right, 1 = path right
	// bit 1 : 0 = no path left,  1 = path left
	uint8_t getSignature();




/*///////////////////////// TASK 1 /////////////////////////*/
// Task 1: Pathfinding from unknown start location
	
	enum class Orientation {Error, A1, A2, A3, A4, B3, B4, B5, B6};

	// Setup for a call to scan
	void taskOneScanSetup(uint8_t power = Motor::LOW_POWER);

	// Returns the signature of the vertex up ahead
	uint8_t scan(uint8_t power = Motor::LOW_POWER);

	void writeResultsToMemory(Node startNode, uint8_t orientation);
	char* toResultsString(Node startNode, uint8_t direction);
	
	void pathfindFromA1(uint8_t power = Motor::LOW_POWER);
	void pathfindFromA2(uint8_t power = Motor::LOW_POWER);
	//   pathfindFromA3 : Not necessary, no use case
	void pathfindFromA4(uint8_t power = Motor::LOW_POWER);
	void pathfindFromB3(uint8_t power = Motor::LOW_POWER);
	void pathfindFromB4(uint8_t power = Motor::LOW_POWER);
	void pathfindFromB5(uint8_t power = Motor::LOW_POWER);
	void pathfindFromB6(uint8_t power = Motor::LOW_POWER);




/*///////////////////////// TASK 2 /////////////////////////*/
// Task 2: Go to E while finding the poles


	// Subroutines for task two
	bool    taskTwoPartZero (uint8_t power = Motor::LOW_POWER);
	uint8_t taskTwoPartOne  (bool& turned, uint8_t power = Motor::LOW_POWER);
	uint8_t taskTwoPartTwo  (bool& turned, uint8_t poleLocationContainer, uint8_t power = Motor::LOW_POWER);
	void    taskTwoPartThree(uint8_t poleLocationContainer, uint8_t power = Motor::LOW_POWER);

	// Returns true if orientation was changed
	bool orientToPole(Util::Side reorientationSide, bool zeroth = false);

	// Need I say more?
	void goToPole(uint8_t power = Motor::LOW_POWER);

public:
	// Small wiggle to confirm or deny presence of pole
	// Returns true iff pole found
	bool findPole();

private:
	// 8 x 150ms low-pitched, 50ms pauses
	void emitPoleDetectionSound();

	// 2 x 750ms high-pitched, 50ms pauses
	void emitEndSound();




/*///////////////////////// TASK 3 /////////////////////////*/

	void printReportPartOne();
	void printReportPartTwo();
	void printSignature();
};
