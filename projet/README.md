
# INF1900 : Projet initial de systeme embarqué

- Team : 2834
- Robot name : JerOhm

To use : First, go to the "projet/lib" directory. Then, use the compilation command

    % make

This compiles the static library  "libstatic.a" and generates the files necessary for the compilation of the main program.

Then, head over to the "projet/app" directory, compile the program and load it into the ATMega324PA's flash memory. **Ensure the robot is properly connected via USB before running the second command.** *Please use the terminal in full screen so that graphics display correctly.*

    % cd ../app
    % make install

The robot should initially be in the "Mode select" mode. If upon installation you notice the LED flashing any given color, the robot might already have had a mode written to its EEPROM memory. In this scenario, simply press reset to change into the "Mode Select" mode.

The robot can perform one of three tasks, which can be selected by pressing the yellow "Reset" button located on the motherboard while the corresponding color is shown on the LED.

    Green : Find Extremity
    Red   : Traverse Course
    Amber : Display Report

If the "Reset" button is pressed at any time during the execution of one of the aforementionned tasks, the robot will revert to the "Select mode" mode.

