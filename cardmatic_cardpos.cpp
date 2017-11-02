//    Cardmatic card generator - cardmatic_cardpos.h file
//    C++11 implementation file 

//    Models a vacuum tube with Tube class and Cardmatic test operations with
//      TubeTests class

//    Written by: cathug



#include <iostream>
#include <cmath>
#include <cassert>
#include "cardmatic_cardpos.h"  // header file for methods
#include "cardmatic_globals.h"  // global variables



//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

// default constructor
Tube::Tube(const std::string tubeID,
           char grid,
           char cathode,
           char screen,
           char suppressor,
           char plate,
           char aux,
           char filPlus,
           char filMinus,
           char filCommon,
           const std::string socket) :
    m_tubeID(tubeID),
    m_sections{ {grid, cathode, screen, suppressor, plate, aux} },
    m_filament{filPlus, filMinus, filCommon},
    m_socket(socket)
{

}

//------------------------------------------------------------------------------

//// alternative constructor
////i.e. Tube(
////        "tubeID",
////        {grid, cathode, screen, suppressor, plate, aux},
////        std::make_tuple(filPlus, filMinus, filCommon),
////        "socket"
////    )
//Tube::Tube(const std::string tubeID,
//           TubeSection ts,
//           std::tuple<char, char, char> filament,
//           const std::string socket) :                    
//    m_tubeID(tubeID),
//    m_sections(ts),
//    m_filament(filament),
//    m_socket(socket)
//{

//}

//------------------------------------------------------------------------------

// destructor
Tube::~Tube()
{

}

//------------------------------------------------------------------------------

// helper to append a tube section to the vector
// pre:     grid = pin 1...9
//          cathode = pin 1...9, Top cap
//          screen = pin 1...9
//          suppressor = pin 1...9, Top cap
//          plate = pin 1...9
//          aux = pin 1...8
//          m_sections.size() >= 1
// param:   grid - grid pin
//          cathode - cathode pin
//          screen - screen pin
//          suppressor - suppressor pin
//          plate - plate 
//          aux - auxiliary pin
// post: contents of tube are updated
bool Tube::appendTubeSection(char grid,
                             char cathode,
                             char screen,
                             char suppressor,
                             char plate,
                             char aux)
{
    if (grid == PIN_CAP || screen == PIN_CAP || plate == PIN_CAP || 
        aux == PIN_9 || aux == PIN_CAP || m_sections.size() < 1)
    {
        return false;
    }
    
    m_sections.push_back( {grid, cathode, screen, suppressor, plate, aux}
    );
        
    return true;
}

//------------------------------------------------------------------------------

//bool Tube::appendTubeSection(TubeSection ts)
//{
//    if (ts.grid == PIN_CAP || ts.screen == PIN_CAP || ts.plate == PIN_CAP || 
//        ts.aux == PIN_9 || ts.aux == PIN_CAP || m_sections.size() < 1)
//    {
//        return false;
//    }
//    
//    m_sections.emplace_back(ts);
//        
//    return true;
//}

//------------------------------------------------------------------------------

// function to activate switches corresponding to single section of a tube
// pre: m_sections.size() > 0
// post: required switches are activated
// see WE Cardmatic manual, sections 5.43-5.50 for more details
void Tube::setSingleTubeSectionSwitches(CardReader &switches,
                                        TubeType tubeType,
                                        bool hasFilament,
                                        bool isSpecialTube)
{
    char filPlus = std::get<FIL_POS>(m_filament);
    char filMinus = std::get<FIL_NEG>(m_filament);
    char filCommon = std::get<FIL_COM>(m_filament);
    
    if (hasFilament && 
        filPlus >= 'A' && filMinus <= 'J' && filPlus != 'I' && 
        filPlus >= 'A' && filMinus <= 'J' && filPlus != 'I' &&  
        filPlus != filMinus)
    {
        if (filCommon == '\0')  // if no center tap component
        {
            switches.insert(std::make_pair(filPlus, ROW_1));
            switches.insert(std::make_pair(filMinus, ROW_2));            
        }

        else // if (filCommon != '\0'), i.e. center tap component exists
        {
            // tie -ve and + heaters together, to row 1 (+ve)
            switches.insert(std::make_pair(filPlus, ROW_1));
            switches.insert(std::make_pair(filMinus, ROW_1));
            switches.insert(std::make_pair(filCommon, ROW_2));
        }
    }   // else do nothing
    
    
    if (tubeType >= DIODE)
    {
        switches.insert(std::make_pair(m_sections.front().plate, ROW_7));
        switches.insert(std::make_pair(m_sections.front().cathode, ROW_4));
        
        if (tubeType >= TRIODE)
        {
            switches.insert(std::make_pair(m_sections.front().grid, ROW_3));
        }
        
        if (tubeType >= TETRODE)
        {
            switches.insert(std::make_pair(m_sections.front().screen, ROW_5));
        }
        
        if (tubeType >= PENTODE)
        {
            switches.insert(std::make_pair(m_sections.front().suppressor, ROW_6));
        }
        
        if (isSpecialTube)
        {
            switches.insert(std::make_pair(m_sections.front().aux, ROW_8));
        }   
    }
}

//------------------------------------------------------------------------------

// function to activate switches corresponding to second section of a tube
// pre: tube.sections.size() > 0
// assert: assert(m_sections.front().cathode == m_sections.back().cathode) if common cathode
// post: required switches are activated
// see WE Cardmatic manual, sections 5.51 for more details
void Tube::setTwinTubeSectionSwitches(CardReader &switches,
                                      TubeType tubeType,
                                      bool isCommonCathode)
{
    if (tubeType != DIODE || tubeType != TRIODE) { return; }
    if (isCommonCathode && 
        m_sections.front().cathode != m_sections.back().cathode) { return; }

    // connection for first section of tube
    setSingleTubeSectionSwitches(switches, tubeType, true, false);


    // connection for second section of tube
    // to accomodate buttton No. 4 interchange mapping
    switches.insert(std::make_pair(m_sections.back().plate, ROW_5));
    switches.insert(std::make_pair(m_sections.back().cathode, ROW_8));    

    if (tubeType == TRIODE)
    {
        switches.insert(std::make_pair(m_sections.back().grid, ROW_6));
    }    
}

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

TubeTests::TubeTests() :
    #if CARDMATIC_MODEL == 15784 || CARDMATIC_MODEL == 1234
    m_regulated_Bplus_maxCurrentParameters{
        {V_REGBPLUS_MIN, 69}, {20, 72}, {30, 75}, {40, 76},
        {V_REGBPLUS_50, 80},  {60, 82}, {70, 86}, {80, 90},
        {90, 95}, {100, 100}, {V_REGBPLUS_110, 110}, {120, 119}, 
        {130, 129}, {140, 140}, {150, 140}, {V_REGBPLUS_160, 129}, 
        {170, 120}, {180, 110}, {190, 102}, {200, 94}, 
        {V_REGBPLUS_210, 85}, {220, 77}, {230, 68}, {240, 60}, 
        {250, 50}, {V_REGBPLUS_MAX, 42}
    }
    
    #elif CARDMATIC_MODEL == 118
    m_regulated_Bplus_maxCurrentParameters{
        {V_REGBPLUS_MIN, 69}, {20, 72}, {30, 75}, {40, 76},
        {V_REGBPLUS_50, 80}, {60, 82}, {70, 86}, {80, 90},
        {90, 95}, {100, 100}, {V_REGBPLUS_110, 110}, {120, 120}, 
        {130, 130}, {140, 140}, {150, 138}, {V_REGBPLUS_160, 129}, 
        {170, 120}, {180, 110}, {190, 102}, {200, 94}, 
        {V_REGBPLUS_210, 85}, {220, 77}, {230, 68}, {240, 60}, 
        {250, 50}, {V_REGBPLUS_MAX, 42} 
    }
    #endif
{

}

//------------------------------------------------------------------------------

// destructor
TubeTests::~TubeTests()
{

}

//------------------------------------------------------------------------------

void TubeTests::triodeTest()
{
	/*if amplifying triode - gm
	1. if fixed bias (cathode grounded) 
		ac + negative bias applied to grid.  
	2. else if self-bias
		ac applied to grid.  ground through bias cap and shunt resistor
	3. plate connected to gm bridge, which is connected to B+
	*/
	
	
}

//------------------------------------------------------------------------------

void TubeTests::outputSwitchesClosed()
{
    std::cout << "\nOutputing Set of Closed Switches" << std::endl;
    for(auto it = m_switches.begin(); it != m_switches.end(); it++)
    {
        std::cout << it->first << it->second << ", ";
    }
    
    std::cout << std::endl;
}

//------------------------------------------------------------------------------

// helper to reset the state of all switches
// post: all switches are deactivated
void TubeTests::resetSwitches()
{
    m_switches.clear();
}

//------------------------------------------------------------------------------
// End of public methods
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// Private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

// helper to determine if desired switch is already activated
// if not add that key into the list of activated switches
// param:   sLetter - letter of switch
//          sNumber - number of switch
// post: required switch is activated
void TubeTests::assertKeyClosed(char sLetter,
                                unsigned int sNumber)
{
    if (!searchSwitch(sLetter, sNumber))
    {
        m_switches.insert(std::make_pair(sLetter, sNumber));
    }    
}

//------------------------------------------------------------------------------

// helper to determine if desired switch is already deactivated
// if not add that key into the list of deactivated switches
// param:   sLetter - letter of switch
//          sNumber - number of switch
// post: required switch is deactivated
void TubeTests::assertKeyOpen(char sLetter,
                              unsigned int sNumber)
{
    // toggle switch if closed
    auto range = m_switches.equal_range(sLetter);
    auto it = range.first;
    while(it != range.second)   // perform search
    {
        if (it->second == sNumber)
        {
            m_switches.erase(it);
            break;
        }

        it++;
    }

    // if not found in list do nothing
}

//------------------------------------------------------------------------------

// search helper to see if the specified switch is in the closed set
// param:   sLetter - letter of switch
//          sNumber - number of switch
bool TubeTests::searchSwitch(char sLetter,
                             unsigned int sNumber)
{
    auto range = m_switches.equal_range(sLetter);
    auto it = range.first;
    while(it != range.second)   // perform search
    {
        if (it->second == sNumber)
        {
            return true;
        }
            
        it++;
    }

    return false; // if not found in list
}

//------------------------------------------------------------------------------

// function to set up connections for twin triode tubes
// pre: tube.sections.size() = 2
//      same filament voltage requested
//      tube to be tested must be a twin triode
//      do not call gridBias function again
//      setTwinTubeSectionSwitches() must be called prior
// param:   vGrid - grid bias required in volts
//          biasType - FIXED_BIAS or SELF_BIAS
//          gridSignal - .222V signal input, 1 for yes, 0 for no
// post: required switches are activated
// see WE Cardmatic manual, sections 5.51 for more details
void TubeTests::setTwinTriodeSwitches(double vGrid,
                                      Biasing biasType,
                                      bool gridSignal)
{
    m_switches.insert(std::make_pair('J', ROW_8));    // close dual testing lamp j8
    m_switches.insert(std::make_pair('K', ROW_8));    // make aux = aux cathode k8
    m_switches.insert(std::make_pair('J', ROW_15));   // apply regulated B+ to second plate

    // suppressor supply (second grid) considerations
    if (biasType == SELF_BIAS)
        m_switches.insert(std::make_pair('C', ROW_16)); // assures -50V through J16

    // call following for either fixed or self bias
    gridBias(vGrid, biasType, gridSignal);
    m_switches.insert(std::make_pair('J', ROW_16));     // bias-off untested section
}

//------------------------------------------------------------------------------

// function to set heater voltage and heater related stuff
// pre: vHeater 0 - 119.9 volts AC in 0.1V steps
//              0 - 50 volts DC in 0.1V steps
// param:   vHeater - voltage of heater filament
// post: required switches are activated
// See WE Cardmatic manual, section 5.52 for more details
void TubeTests::setHeaterVolts(double vHeater)
{
    char ascii_shift = 'A';
    char tens = ascii_shift + static_cast<unsigned int> (
        vHeater / V_REGBPLUS_INC) ;   // tens of volts
    char units = ascii_shift + static_cast<unsigned int> (
        fmod(vHeater, V_REGBPLUS_INC) ); // units of volts
    char tenths = ascii_shift + static_cast<unsigned int> (
        fmod(vHeater,1) * V_REGBPLUS_INC );  // tenths of volts

    // fix 'no I' switch naming problem
    if (tens >= 'I') { tens++; }
    if (units >= 'I') { units++; }
    if (tenths >= 'I') { tenths++; }

    m_switches.insert(std::make_pair(tens, ROW_9));
    m_switches.insert(std::make_pair(units, ROW_10));
    m_switches.insert(std::make_pair(tenths, ROW_11));
}

//------------------------------------------------------------------------------

// function to adjust heater settings
// pre: heater current <= 500mA if DC heater is selected
//      heater voltage is no more than 12.6V if ctRes is set to 1
//      tieFilToCat should be set to 0 for diodes and rectifiers
// param:   heaterType - AC_HEATER or DC_HEATER
//          tieFilToCat - tie -ve heater filament to cathode - 1= yes, 0 = no
//          ctRes - places 100 ohm ct resistor accross filament,
//                  ct is connected to cathode after quality button depressed
//                  choose 1 if yes, 0 if no
//          ampTube - input 1 if amplifier tube, 0 otherwise
//          filamentary - direct heater/filamentary tube - 1 = yes, 0 = no
// post: required switches are activated
// See WE Cardmatic manual, sections 5.52-5.53, 5.61 and 5.63 for more details
void TubeTests::adjustHeaterSettings(Tube &tube,
                                     Heater heaterType,
                                     bool tieFilToCat,
                                     bool ctRes,
                                     bool ampTube,
                                     bool filamentary)
{
    // choose ac or dc heater
    if (heaterType == AC_HEATER)
    {
        m_switches.insert(std::make_pair('A', ROW_12));  // a12	use AC filament supply
        m_switches.insert(std::make_pair('B', ROW_15));  // b15	use AC filament supply
        assertKeyOpen('K', ROW_1);
        assertKeyOpen('K', ROW_2);
    }

    else // if (heaterType == DC_HEATER)
    {
        m_switches.insert(std::make_pair('K', ROW_1));   // k1	use DC filament supply
        m_switches.insert(std::make_pair('K', ROW_2));   // k2	use DC filament supply
        assertKeyOpen('A', ROW_12);
        assertKeyOpen('B', ROW_15);
    }



    // if tie filament- to cathode
    // not for diodes and rectifiers
    if (tieFilToCat && ampTube)
    {
        // tie filament to cathode
        m_switches.insert(std::make_pair(std::get<tube.FIL_NEG>(tube.m_filament), ROW_4));

        // g17 closed to eliminate
        // DC heater-cathode leakage test voltage
        m_switches.insert(std::make_pair('G', ROW_17));
    }



    if (filamentary)
    {
        // places 100 ohm ct resistor across
        // filament if heater is no more than 12.6V
        // center tap of resistor is connected to cathode supply
        if (ctRes)
        {
            m_switches.insert(std::make_pair('L', ROW_11));  // l11	center-tapped fil resis to cathode supply
        }
            

        // close g17 if filamentary amplifier tube to prevent meter deflection
        // for normal tubes on the heater-cathode leakage test
        if (ampTube)
        {
            assertKeyClosed('G', ROW_17); // g17	disable leakage test lamp
        }
            
        // all filamentary tube types close switches a14, b14, c14
        m_switches.insert(std::make_pair('A', ROW_14));  // a14	leakage test shunt, +20 microamperes
        m_switches.insert(std::make_pair('B', ROW_14));  // b14	leakage test shunt, +50 microamperes
        m_switches.insert(std::make_pair('C', ROW_14));  // c14	leakage test shunt, +100 microamperes
    }
}

//------------------------------------------------------------------------------

// Function: determines the B+ voltage, in 10 volt increments
// pre: vBPlus must be 10 - 260 volts in 10 volt increments
// param:   vBPlus - required B+ voltage
//          screenConnect - connect to screen? yes = 1, no = 0
//          gmBridgeConnect - connect to gm bridge? yes = 1, no = 0
// post: required switches are closed
// See WE Cardmatic manual, sections 5.36, 5.54 for more details
void TubeTests::B_plusVolts(unsigned int vBPlus,
                            bool screenConnect,
                            bool gmBridgeConnect)
{
    unsigned int B_plusMax, voltDiff;

    // determine range first and close associated switch
    if (vBPlus > V_REGBPLUS_210)
    {
        B_plusMax = V_REGBPLUS_MAX;
    }

    else if (vBPlus > V_REGBPLUS_160 && vBPlus <= V_REGBPLUS_210)
    {
        m_switches.insert(std::make_pair('L', ROW_2));   // l2   210 - 170 reg B+ range
        B_plusMax = V_REGBPLUS_210;
    }

    else if (vBPlus > V_REGBPLUS_110 && vBPlus <= V_REGBPLUS_160)
    {
        m_switches.insert(std::make_pair('B', ROW_17));  // b17  160 - 120 reg B+ range
        B_plusMax = V_REGBPLUS_160;
    }

    else if (vBPlus > V_REGBPLUS_50 && vBPlus <= V_REGBPLUS_110)
    {
        m_switches.insert(std::make_pair('C', ROW_17));  // c17  110 - 60 reg B+ range
        B_plusMax = V_REGBPLUS_110;
    }

    else // if (vBPlus >= V_REGBPLUS_MIN && vBPlus <= V_REGBPLUS_50)
    {
        m_switches.insert(std::make_pair('D', ROW_17));  // d17  50 - 10 reg B+ range
        B_plusMax = V_REGBPLUS_50;
    }




    // figure out other switches to close to make up difference
    voltDiff = B_plusMax - vBPlus;
    if (voltDiff == 10 || voltDiff == 30)
    {
        m_switches.insert(std::make_pair('E', ROW_17));  // e17	subtract 10 v from reg B+
    }

    if (voltDiff >= 20)
    {
        m_switches.insert(std::make_pair('L', ROW_4));   // l4   subtract 20 v from reg B+
        if (voltDiff == 40)
        {
            m_switches.insert(std::make_pair('L', ROW_3));   // l3   subtract 20 v from reg B+ (use with l4)
        }
    }




    // and the boolean options
    if (screenConnect || gmBridgeConnect)
    {
        m_switches.insert(std::make_pair('J', ROW_15));  // j15	reg B+ to screen line
    }
        
    if (gmBridgeConnect)
    {
        // connect regulated B+
        // to one end of gm bridge
        m_switches.insert(std::make_pair('H', ROW_15));  // h15	plate current test from reg B+

        // complete gm bridge circuit
        m_switches.insert(std::make_pair('K', ROW_17));  // k17	plate line to top of Gm bridge
        m_switches.insert(std::make_pair('A', ROW_13));  // a13	Removes 100k ohm meter multiplier
        m_switches.insert(std::make_pair('B', ROW_13));  // b13	plate quality supply / bridge
        m_switches.insert(std::make_pair('H', ROW_13));  // h13	plate quality supply Gm bridge
    }
}

//------------------------------------------------------------------------------

// may be replaced with database
// Function: makes sure the B+ voltage does not exceed maximum allowable currents
// pre: vBPlus must be 10 - 260 volts in 10 volt increments
// param:   vBPlus - required B+ voltage
//          current - maximum current
// See WE Cardmatic manual, section 5.54 for more details
bool TubeTests::B_plusCurrentCheck(unsigned int vBPlus,
                                   unsigned int current)
{
    // key = Vreg, element = maximum rated current at Vreg
    if (current <= m_regulated_Bplus_maxCurrentParameters.find(vBPlus)->second)  // if current <= max rated current
    {
        return true;
    }

    return false;
}

//------------------------------------------------------------------------------

// umhometer shunt select function
// pre: full scale range of gm must be between 500 - 128,000
//      500-26,000 umho in 100 umho steps; 26,000-128,000 umho in 500 umho steps
// param:   gm - mutual conductance
// post: all necessary shunt switches are activated
// see WE Cardmatic manual, sections 5.55, 5.56 for more details
// some of the depressed switches mentioned in table in page 27 are incorrect
void TubeTests::umho_meterShunt(unsigned long gm)
{
    unsigned int dChoice;   // desired choice number

    if (gm <= METER_FS_GM_MAX_LOW)    // gm from 500 to 26000
    {
        dChoice = (gm - METER_FS_GM_MIN) / 100;    // desired choice - regular scale gm

        // l12	remove 1070 & 25,340 ohm multiplier resistors
        assertKeyClosed('L', ROW_12); // assert switch L12 is closed
        assertKeyOpen('L', ROW_7); // assert switch L7 is open)
    }

    else    // gm > 26,000
    {
        dChoice = (gm / METER_FS_GM_MIN) - 1;    // desired choice - extended scale gm

        // close l7 - shunt the 25,340 ohm mult resistor with the 1070 ohm resistor
        assertKeyClosed('L', ROW_7); // assert switch L7 is closed
        assertKeyOpen('L', ROW_12); // assert switch L12 is open)
    }

    // determine meter shunt
    assert(dChoice < NUM_POSSIBLE_GM_VALUES);
    meterShuntValue(dChoice);
}

//------------------------------------------------------------------------------

// helper to determine shunt value
// pre: dchoice must be between 0 to 255
// param:   dchoice - required choice number of meter shunt
// post: all necessary shunt switches are activated
// see WE Cardmatic manual, section 5.55 more details
void TubeTests::meterShuntValue(unsigned int dChoice)
{
    unsigned int pChoice = PRIMARY_CHOICE_MAX; // initialize primary choice
    char key = 'K';

    // determine combination of shunt choices C-K12
    while (dChoice != 0)
    {
        if (dChoice >= pChoice)
        {
            m_switches.insert(std::make_pair(key, ROW_12));
            dChoice -= pChoice;
        }

        key--;
        pChoice /= 2; // shave pChoice by half

        if (key == 'I')   // skip I
        {
            key--;
        }
    }
}

//------------------------------------------------------------------------------

// mA meter shunt select function
// pre: range must be between 100 - 510100 uA (0.1mA to 510.1mA)
// param:   fsCurrent - current in MICROamperes
// post: all necessary shunt switches are activated
// see WE Cardmatic manual, section 5.60 for more details
void TubeTests::ma_meterShunt(unsigned long fsCurrent)
{
    unsigned int choice, multiplier;


    if (fsCurrent > METER_FS_I_MAX_MID)    // dc current range > 25600uA
    {
        assertKeyOpen('L', ROW_7); // assert switch L7 is open
        assertKeyOpen('L', ROW_12); // assert switch L12 is open
        multiplier = 100;

    }

    else if (fsCurrent > METER_FS_I_MAX_LOW &&
             fsCurrent <= METER_FS_I_MAX_MID)    // dc current range between 5200 and 25600uA
    {
        m_switches.insert(std::make_pair('L', ROW_7));   // l7	shunts the 25,340 ohm mult resistor with the 1070 ohm resistor
        assertKeyOpen('L', ROW_12); // assert switch L12 is open
        multiplier = 50;
    }

    else    // if dc current range between 100 - 5200uA
    {
        m_switches.insert(std::make_pair('L', ROW_12));  // l12	removes 1070 & 25,340 ohm multiplier resistors
        assertKeyOpen('L', ROW_7); // assert switch L7 is open
        multiplier = 10;
    }

    // determine meter shunt
    choice = rint((.5 * fsCurrent - 50) / multiplier);
    meterShuntValue(choice);
}

//------------------------------------------------------------------------------

// Function: conduct plate current test with regulated B+ source
// pre: range must be between 0 - 510100 uA (0mA to 510.1mA)
// param:   fsCurrent - current in MICROamperes
// post: all necessary switches are activated
// see WE Cardmatic manual, section 5.59 for more details
void TubeTests::plateCurrentTest(unsigned long fsCurrent)
{
    m_switches.insert(std::make_pair('J', ROW_15));
    m_switches.insert(std::make_pair('K', ROW_15));
    m_switches.insert(std::make_pair('A', ROW_13));
    m_switches.insert(std::make_pair('C', ROW_13));
    m_switches.insert(std::make_pair('J', ROW_17));
    ma_meterShunt(fsCurrent);
}

//------------------------------------------------------------------------------

// helper to set decade resistor value
// pre: max dval = 70000 (71100) in ohms
//      dval <= 1k can operate <= DECADE_RES_IMAX_1K
//      1K < dval <= 70K  can operate <= DECADE_RES_VMAX
// param:   dVal - demanded resistance value
// post: all unactivated switches correspond to required resistance value
// see WE Cardmatic manual, sections 5.57, 5.58 for more details
void TubeTests::decadeResistor(unsigned long dVal)
{
    // replace sw insert map lines temp map lines if delay is preferred
    // if controlling switches with microcontrollers

    char sLetter = 'G';
    unsigned int i = 1;
    unsigned int sNumber = 15;

    // determine combination of shunt choices D-G15

//    std::cout << "dVal is " << dVal << endl;

    // first step close all shunt switches
    unsigned int n = 13;
    for (char c = 'D'; c <= sLetter; c++)
    {
        if (c != sLetter && n != 16)
        {
            m_switches.insert(std::make_pair(c, n));
            n++;
        }

//        std::cout << "works here" << endl;
    }

    unsigned int pVal = 30000; // max primary resistor value

    // reopen those that correspond to shunt choices
    while (dVal != 0)
    {
        if (dVal >= pVal)
        {
            assertKeyOpen(sLetter,sNumber);
            dVal -= pVal;
//            std::cout << "pVal is " << pVal << endl;
//            std::cout << "dVal is " << dVal << endl;
        }

        sNumber--;
        pVal -= (10000 / i); // shave pVal by i

        if (sNumber < 13)
        {
            sNumber = 16;
//            std::cout << "sNumber is " << sNumber << endl;
            sLetter--;
            pVal = (4000 / i);
//            std::cout << "pVal is " << pVal << endl;
            i *= 10;
        }
    }
}

//------------------------------------------------------------------------------

// grid bias set helper
// pre: abs(vGrid) <= V_BIAS_MAX in 0.1 volt steps
// param:   vGrid - grid bias required in volts
//          biasType - FIXED_BIAS or SELF_BIAS
//          gridSignal - .222V signal input, 1 for yes, 0 for no
// post: all necessary bias switches are activated
// see WE Cardmatic manual, sections 5.57, 5.58 for more details
bool TubeTests::gridBias(double vGrid,
                         Biasing biasType,
                         bool gridSignal)
{
	unsigned int Ec, decade_resistor_value;
	
    m_switches.insert(std::make_pair('H', ROW_14));  // h14	cathode supply to unreg B+
    m_switches.insert(std::make_pair('A', ROW_16));  // a14	leakage test shunt, +20 microamperes

    // select type of bias
    if (biasType == FIXED_BIAS)
    {
        m_switches.insert(std::make_pair('L', ROW_14));  // l14	cathode supply to 0V
        m_switches.insert(std::make_pair('C', ROW_16));  // c14	leakage test shunt, +100 microamperes
    }

    else // if (biasType == SELF_BIAS)
    {
        m_switches.insert(std::make_pair('K', ROW_14));  // k14	self-bias mode
        m_switches.insert(std::make_pair('C', ROW_15));  // c15	Gm bridge to 0V
    }

//    // if testing diodes and rectifiers other value for fixedBias

    // select signal on grid
    if (gridSignal == 0) // no .222V signal
    {
        m_switches.insert(std::make_pair('K', ROW_13));  // k13	grid supply to cathode w .222v
        assertKeyOpen('L', ROW_13); // assert switch L13 is open
    }


    else    // with .222V signal
    {
        m_switches.insert(std::make_pair('L', ROW_13));  // l13	grid supply to cathode w/o .222v
        assertKeyClosed('K', ROW_13); // assert switch K7 is closed
    }


    Ec = abs(vGrid);
    decade_resistor_value = Ec * 15000 / (150 - Ec);
    if (Ec > 200 && decade_resistor_value > 2000) { return false; }
    decadeResistor(decade_resistor_value);

    return true;
}

//------------------------------------------------------------------------------

// pre: setTwinTriodeSwitches function is not used
// assert: tube.sections.front().cathode...tube.sections.back().cathode
void TubeTests::leakageTest(Tube &tube,
                            unsigned int reqRejectCurrent,
                            unsigned int section)
{
//    list<TubeSection>::iterator it = tube.m_sections.begin();
    auto it = tube.m_sections.begin();

    for (unsigned int i = 0; i < section; i++)
        it++;

    if (!searchSwitch('G',ROW_17) &&
        std::get<tube.FIL_POS>(tube.m_filament) &&
        std::get<tube.FIL_NEG>(tube.m_filament) &&
        searchSwitch(it->cathode, 4))
    {
        leakageShunt(reqRejectCurrent);
    }
}

//------------------------------------------------------------------------------

// sets heater-cathode leakage meter sensitivity
// function selects leakage current shunts for proper display at rejection (10% fs)
// pre: G17 is open, rows 1 and 2 are used for a heater, row 4 used for a cathode
//      reqMeterValue must be 10, 20, 50, 70, 100, 150 - (165 is not implemented)
// param: reqRejectCurrent - current at rejection
// see WE Cardmatic manual, section 5.53, 5.63 for more details
void TubeTests::leakageShunt(unsigned int reqRejectCurrent)
{
    const unsigned int key = 14;

    if (reqRejectCurrent == I_NOM_HC_LEAKAGE_10)
    {
        assertKeyOpen('A', key);
        assertKeyOpen('B', key);
        assertKeyOpen('C', key);
    }

    if (reqRejectCurrent % I_NOM_HC_LEAKAGE_50 == I_NOM_HC_LEAKAGE_20)    // if 20, 70, 120
    {
        m_switches.insert(std::make_pair('A', key));  // a14	leakage test shunt, +20 microamperes
        reqRejectCurrent -= I_NOM_HC_LEAKAGE_20;
    }

    if (reqRejectCurrent != I_NOM_HC_LEAKAGE_100 &&
        reqRejectCurrent % I_NOM_HC_LEAKAGE_50 == 0)  // if 50, 150
    {
        m_switches.insert(std::make_pair('B', key));  // b14	leakage test shunt, +50 microamperes
        reqRejectCurrent -= I_NOM_HC_LEAKAGE_50;
    }

    if (reqRejectCurrent == I_NOM_HC_LEAKAGE_100)
    {
        m_switches.insert(std::make_pair('C', key));  // c14	leakage test shunt, +100 microamperes
    }
}

//------------------------------------------------------------------------------

// recommended heater-cathode leakage meter sensitivity for diodes and rectifiers
// function selects leakage current shunts for proper display at rejection (10% fs)
// see WE Cardmatic manual, section 5.53, 5.63 for more details
// param:   type - of diode/rectifier - see enum diodeType in cardmatic_globals.h for options
void TubeTests::rec_diodeLeakage(Tube &tube, DiodeType type)
{
    switch (type)
    {
        case DETECTOR_DIODE:
            leakageTest(tube, I_NOM_HC_LEAKAGE_20, 1);
            break;
            
        case POWER_RECTIFIER:
            leakageTest(tube, I_NOM_HC_LEAKAGE_150, 1);
            break;
            
        case DAMPER_DIODE:
            leakageTest(tube, I_NOM_HC_LEAKAGE_10, 1);
            break;
            
        default:	// FILAMENTARY_DIODE or FILAMENTARY_RECTIFIER
        	assertKeyClosed('A', ROW_14);
        	assertKeyClosed('B', ROW_14);
        	assertKeyClosed('C', ROW_14);
//			  m_switches.insert(std::make_pair('A', 14));
//            m_switches.insert(std::make_pair('B', 14));
//            m_switches.insert(std::make_pair('C', 14));
            break;
    }
}

//------------------------------------------------------------------------------

// tests high voltage diodes and high and low perveance diodes
// param:  fsCurrent - full scale current
//         b_plusType  REGULATED_BPLUS or AUXILIARY_BPLUS
//         currentLimiting - requires current limiting resistors?
//                              0 = no, 1 = yes
//         rCurrentLimiting - current limiting resistor value
// post: all necessary switches are activated
void TubeTests::diodeTest(unsigned int fsCurrent,
                          Bplus b_plusType,
                          bool currentLimiting,
                          unsigned int rCurrentLimiting)
{
    plateCurrentTest(fsCurrent);

    if (b_plusType == AUXILIARY_BPLUS)
    {
        assertKeyOpen('J', ROW_15);
        assertKeyOpen('K', ROW_5);
        assertKeyClosed('L', ROW_5);
    }


    if (!currentLimiting)
    {
        m_switches.insert(std::make_pair('L', ROW_14));
    }

    else    // if current limiting required
    {
        assertKeyOpen('L', ROW_14);
        assertKeyClosed('H', ROW_14);
        assertKeyClosed('A', ROW_16);
        assertKeyClosed('C', ROW_15);
        decadeResistor(rCurrentLimiting);
    }
}

//------------------------------------------------------------------------------

// tests half wave rectifiers
// pre: meter sensitivity set to 63 on 0-100 scale if max inverse voltage
//         is > HWTHRESHOLD_MAXINVRATING = 330V, and follow procedure in WE Cardmatic manual 5.64b)
// param:  rLoad - resistive load, in ohms
//         maxInvRating - maximum inverse rating of diode, in volts
//         mSensitiviy - meter sensitivity
// post: all necessary switches are activated
void TubeTests::halfWaveRectifierTest(unsigned int rLoad,
                                      unsigned int maxInvRating,
                                      unsigned int mSensitivity)
{
    m_switches.insert(std::make_pair('L', ROW_17));

    // cathode to ground circuit
    m_switches.insert(std::make_pair('H', ROW_14));
    decadeResistor(rLoad);
    m_switches.insert(std::make_pair('B', ROW_16));
    m_switches.insert(std::make_pair('C', ROW_13));
    m_switches.insert(std::make_pair('A', ROW_13));
    ma_meterShunt(mSensitivity);
    m_switches.insert(std::make_pair('J', ROW_13));

    if (maxInvRating > HWTHRESHOLD_MAXINVRATING)
    {
        m_switches.insert(std::make_pair('J', ROW_14));
    }
}

//------------------------------------------------------------------------------

// tests full wave rectifiers
// pre: maximum current limited by rating of decade resistor
//         see section 5.57-5.58 for more details
// param:  rLoad - resistive load, in ohms
//         maxInvRating - maximum inverse rating of diode, in volts
//         mSensitiviy - meter sensitivity
// post: all necessary switches are activated
void TubeTests::fullWaveRectifierTest(Tube &tube,
                                      unsigned int rLoad,
                                      unsigned int maxInvRating,
                                      unsigned int mSensitivity)
{

    halfWaveRectifierTest(rLoad,maxInvRating,mSensitivity);
    m_switches.insert(std::make_pair(tube.m_sections.back().plate, ROW_5)); // connect second plate to screen row 5
    assertKeyClosed('L', ROW_15);
    assertKeyClosed('J', ROW_14);   // place 4uf capacitor across load resistance
}

//------------------------------------------------------------------------------

// tests damper diodes
// pre: rLoad is <5500 ohms or 92mA
// param:  rLoad - resistive load, in ohms
//         maxInvRating - maximum inverse rating of diode, in volts
//         mSensitiviy - meter sensitivity
// post: all necessary switches are activated
void TubeTests::damperDiodeTest(Tube &tube,
                                unsigned int rLoad,
                                unsigned int mSensitivity)
{
    halfWaveRectifierTest(rLoad, DAMPER_MAXINVRATING, mSensitivity);
    m_switches.insert(std::make_pair('J', ROW_17));
    m_switches.insert(std::make_pair(tube.m_sections.front().plate, ROW_5)); // connect plate to screen row 5
    assertKeyOpen(tube.m_sections.front().plate, ROW_7);   // assuming this means no connection to row 7
    m_switches.insert(std::make_pair('L', ROW_15));
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// End of private methods
//------------------------------------------------------------------------------
