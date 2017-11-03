//    Cardmatic card generator - test_main1.cpp file
//    C++11 main function file

//    Test program to derive cardmatic settings for ECC83
//    It tests functions in classes Tube and TubeTests

//    Written by: cathug




#include "cardmatic_cardpos.h"
#include "cardmatic_globals.h"
#include "cardmatic_tube.h"
#include <iostream>



// Test program... testing ECC83
// TODO: write unit tests
int main(int argc, char* argv[])
{
    double heaterVolts = 6.3;
    unsigned int b_plus = 250;
    double current = 1.2;
    unsigned long gm = 1600;
    double vBias = -2.0;
    
    CardReader switches;
    TubeTests tests;
    
    Tube ECC83(
        "ECC83", 
        ECC83.PIN_2, ECC83.PIN_3, '\0', '\0', ECC83.PIN_1, '\0', 
        ECC83.PIN_4, ECC83.PIN_5, ECC83.PIN_9, 
        "B9A"
    );
	
	if ( ECC83.tubePinsAreValid() != ECC83.TUBE_PINS_OK )
	{
        std::cout << "Invalid Tube Pins" << std::endl;
	    return -1; 
	}
	
	
    ECC83.appendTubeSection(ECC83.PIN_7, ECC83.PIN_8, '\0', '\0', ECC83.PIN_6, '\0');
    ECC83.setTwinTubeSectionSwitches(
        tests.getClosedSwitches(), 
        ECC83.TRIODE, 
        false
    );
    
    tests.setHeaterVolts( heaterVolts );
    tests.adjustHeaterSettings( ECC83, tests.AC_HEATER, false, false, true, true );
    tests.gridBias( vBias, tests.SELF_BIAS, true );
    tests.B_plusVolts( b_plus, false, true );
    tests.B_plusCurrentCheck( b_plus, current );
    tests.umho_meterShunt( gm );
    tests.outputSwitchesClosed();
    
    return 0;
}

//TODO: implement pseudocode
/*
tube type

	if amplifying triode - gm
	1. if fixed bias (cathode grounded) 
		ac + negative bias applied to grid.  
	2. else if self-bias
		ac applied to grid.  ground through bias cap and shunt resistor
	3. plate connected to gm bridge, which is connected to B+


	else if amplifying pentode - gm
	1. if fixed bias (cathode grounded) 
		ac + negative bias applied to grid.
		suppressor grounded
	2. else if self-bias
		ac applied to grid.  ground through bias cap and shunt resistor
		suppressor connected to cathode
	3. plate connected to gm bridge, which is connected to B+
	4. screen connected to B+


	else if heptode - 2 gm tests/cards (2 control grid type)
	1. plate connected to gm bridge, which is connected to B+
	2. bias applied to control grids
	3. apply ac signal to EACH grid one at a time

	else if power pentodes - 2 cards
	1. apply amplifying pentode test
	2. apply knee test
		connect plate to negative meter
		positive meter to ground via capacitor
		B+ supply via drop resistor
		grid to cathode
		screen to B+
		suppressor to ground

	else if computer triode 
	1. apply AF triode test
	2. apply zero bias plate current (on test)
		grid to cathode
		plate to negative meter terminal
	3. apply high bias plate current (off test)
		grid to bias
		cathode to ground
		plate to negative meter terminal

	else if diodes
	0. if voltage regulator

	1. if full wave rectifier
		apply 250v ct AC across plates
		connect cathode to load resistor/filter capacitor
		meter connected to other end of resistor/capacitor and ground
	2. else if half-wave rectifier
		apply HT to plate
		connect cathode to load resistor
		meter connected to other end of resistor and ground
	3. else if high voltage rectifier
		apply HT to plate
		connect cathode to load resistor/filter capacitor
		connect other end of resistor/meter to positive dc ma meter
		connect other end of transformer to meter -ve
	4. else if high voltage diode
	5. else if high perveance detector diodes
	6. if low pervenance detector diodes


*/

