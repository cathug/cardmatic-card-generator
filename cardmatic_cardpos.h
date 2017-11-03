//    Cardmatic card generator - cardmatic_cardpos.h file
//    C++11 header file 

//    Models a vacuum tube with Tube class and Cardmatic test operations with
//      TubeTests class

//    Written by: cathug



#ifndef CARDMATIC_CARDPOS_H
#define CARDMATIC_CARDPOS_H

#include "cardmatic_tube.h"
#include <map>
#include <vector>
#include <tuple>
#include <string>


class Tube
{
    public:
    
        friend class TubeTests;
        
        //----------------------------------------------------------------------
        //  enums and structs
        //----------------------------------------------------------------------        
        
        // maps socket pin to switch letters
        // See WE Cardmatic manual, section 5.43 for more details
        typedef enum TubeSocket : char
        {
            PIN_1 = 'A',
            PIN_2,
            PIN_3,
            PIN_4,
            PIN_5,
            PIN_6,
            PIN_7,
            PIN_8,
            PIN_9 = 'J',
            PIN_CAP,
        }TubeSocket;
        
        
        // model of a section in a tube
        typedef struct TubeSection
        {
            char grid;
            char cathode;
            char screen;
            char suppressor;
            char plate;
            char aux;
        }TubeSection;
        
        
        typedef enum TubeType
        {
            DIODE = 2,
            TRIODE,
            TETRODE,
            PENTODE,
            HEPTODE,
        }TubeType;
        
        
        typedef enum TubeFilament : size_t
        {
            FIL_POS,
            FIL_NEG,
            FIL_COM,            
        }TubeFilament;
        
        
        typedef enum TubeErrorCodes
        {
            TUBE_NULL_SECTION = -2,
            TUBE_INVALID_PIN_ASSIGN,
            TUBE_PINS_NOT_UNIQUE,
            TUBE_PINS_OK,
            
        }TubeErrorCodes;
        
        
        
        //----------------------------------------------------------------------
        //  constructors, destructor
        //----------------------------------------------------------------------  
        
        Tube(const std::string tubeID,
             char grid,
             char cathode,
             char screen,
             char suppressor,
             char plate,
             char aux,
             char filPlus,
             char filMinus,
             char filCommon,
             const std::string socket);  // default constructor
    
    
        Tube(const std::string tubeID,
             TubeSection ts,
             std::tuple<char, char, char> filament,
             const std::string socket);  // alternative constructor
        
        
        ~Tube();                         // destructor
        
    

        //----------------------------------------------------------------------
        //  member functions
        //---------------------------------------------------------------------- 
    
        // Function to check if tube pin assignments are valid, 
        // pre: To be called (immediately) after constructor
        // returns: TUBE_PINS_OK if pin assignments are VALID.  Otherwise
        //          TUBE_INVALID_PIN_ASSIGN, 
        //          TUBE_PINS_NOT_UNIQUE, or 
        //          TUBE_NULL_SECTION
        int tubePinsAreValid(); 
    
    
        // helper to append a tube section to the linked list
        // pre:     grid = pin 1...9
        //          cathode = pin 1...9, Top cap
        //          screen = pin 1...9
        //          suppressor = pin 1...9, Top cap
        //          plate = pin 1...9
        //          aux = pin 1...8
        //          tube.sections.size() >= 1
        // param:   grid - grid pin
        //          cathode - cathode pin
        //          screen - screen pin
        //          suppressor - suppressor pin
        //          plate - plate 
        //          aux - auxiliary pin
        // post: contents of tube are updated
        void appendTubeSection(char grid,
                               char cathode,
                               char screen,
                               char suppressor,
                               char plate,
                               char aux);
        

        // alternative version of the above 
        // but a TubeSection struct is passed instead
        void appendTubeSection(TubeSection ts); 
                               
        
        // function to activate switches corresponding to single section of a tube
        // pre: tube.sections.size() > 0
        // post: required switches are activated
        // see WE Cardmatic manual, sections 5.43-5.50 for more details
        void setSingleTubeSectionSwitches(CardReader &switches,
                                          TubeType tubeType,
                                          bool hasFilament,
                                          bool isSpecialTube);
        
        
        // function to activate switches corresponding to second section of a tube
        // pre: tube.sections.size() > 1
        // post: required switches are activated
        // see WE Cardmatic manual, sections 5.51 for more details
        void setTwinTubeSectionSwitches(CardReader &switches,
                                        TubeType tubeType,
                                        bool isCommonCathode);



    private:
        //----------------------------------------------------------------------
        //  helpers
        //----------------------------------------------------------------------

        // helper to append a non-null string to a string
        // used in tubePinsAreValid() only
        void appendCharToString(char ch,
                                std::string &str);

    
    
        //----------------------------------------------------------------------
        //  member variables
        //----------------------------------------------------------------------

        std::string m_tubeID;                       // tube nomenclature
        std::vector<TubeSection> m_sections;        // (semi)independent tubeSections stored in a vector
        std::tuple<char, char, char> m_filament;    // filament/heater pins; get<FIL_POS> = +ve, get<FIL_NEG> = -ve, get<FIL_COM> = center
        std::string m_socket;                       // tube socket
};





class TubeTests
{
    public:
        //----------------------------------------------------------------------
        //  constructors, destructor, init function
        //----------------------------------------------------------------------    
        
        TubeTests();   // constructor
        
        ~TubeTests();  // destructor
        

        //----------------------------------------------------------------------
        //  enums
        //----------------------------------------------------------------------
        
        typedef enum DiodeType
        {
            DETECTOR_DIODE,
            POWER_RECTIFIER,
            DAMPER_DIODE,
            FILAMENTARY_DIODE,
            FILAMENTARY_RECTIFIER,
        }DiodeType;


        typedef enum Bplus
        {
            REGULATED_BPLUS, 
            AUXILIARY_BPLUS,
        }Bplus;


        typedef enum Biasing
        {
            SELF_BIAS,
            FIXED_BIAS,
        }Biasing;


        typedef enum Heater
        {
            AC_HEATER,
            DC_HEATER
        }Heater;

        
        
        //----------------------------------------------------------------------                                                          
        //  member functions
        //----------------------------------------------------------------------

        // tests possible with Cardmatic testers
        void triodeTest();
        void pentodeTest();
        void heptodeTest();
        void kneeTest();
        void multivibratorTest();
        void highVoltageRectifierTest();
        void highVoltageDiodeTest();
        void highPerveanceDetectorDiodeTest();
        void votageRegulatorTest();
        void shortTest();
        void gasTest();
        
        void outputSwitchesClosed();
        
        
        // helper to reset the state of all switches
        // post: all switches are deactivated
        void resetSwitches(); 
        
        CardReader &getClosedSwitches() { return m_switches; } 

//    private:

        //----------------------------------------------------------------------
        //  member variables
        //----------------------------------------------------------------------
        
        CardReader m_switches;	// set of switches to close in cardreader

        const std::map<unsigned int, 
            unsigned int> m_regulated_Bplus_maxCurrentParameters;


        //----------------------------------------------------------------------                                                          
        //  member functions
        //----------------------------------------------------------------------

        // helper to determine if desired switch is already activated
        // if not add that key into the list of activated switches
        // param:   sLetter - letter of switch
        //          sNumber - number of switch
        // post: required switch is activated
        void assertKeyClosed(char sLetter,
                             unsigned int sNumber);


        // helper to determine if desired switch is already deactivated
        // if not add that key into the list of deactivated switches
        // param:   sLetter - letter of switch
        //          sNumber - number of switch
        // post: required switch is deactivated
        void assertKeyOpen(char sLetter,
                           unsigned int sNumber);


        // search helper to see if the specified switch is in the closed set
        // param:   sLetter - letter of switch
        //          sNumber - number of switch
        bool searchSwitch(char sLetter,
                      	  unsigned int sNumber);



        // function to set up connections for twin triode tubes
        // pre: tube, a linked list, is of size 2 (tube.size() = 2), same filament voltage requested
        //      tube to be tested must be a twin triode
        //      do not call gridBias function again
        // param:   vGrid - grid bias required in volts
        //          biasType - FIXED_BIAS or SELF_BIAS
        //          gridSignal - .222V signal input, 1 for yes, 0 for no
        // assert: assert(tube.sections.front().cathode == tube.sections.back().cathode) if common cathode
        // post: required switches are activated
        // see WE Cardmatic manual, sections 5.51 for more details
        void setTwinTriodeSwitches(double vGrid,
                                   Biasing biasType,
                                   bool gridSignal);


        // function to set heater voltage and heater related stuff
        // pre: vHeater 0 - 119.9 volts AC in 0.1V steps
        //              0 - 50 volts DC in 0.1V steps
        // param:   vHeater - voltage of heater filament
        // post: required switches are activated
        // See WE Cardmatic manual, section 5.52 for more details
        void setHeaterVolts(double vHeater);


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
        void adjustHeaterSettings(Tube &tube,
                                  Heater heaterType,
                                  bool tieFilToCat,
                                  bool ctRes,
                                  bool ampTube,
                                  bool filamentary);


        // Function: determines the B+ voltage, in 10 volt increments
        // pre: vBPlus must be 10 - 260 volts in 10 volt increments
        // param:   vBPlus - required B+ voltage
        //          screenConnect - connect to screen? yes = 1, no = 0
        //          gmBridgeConnect - connect to gm bridge? yes = 1, no = 0
        // post: required switches are closed
        // See WE Cardmatic manual, sections 5.36, 5.54 for more details
        void B_plusVolts(unsigned int vBPlus,
                         bool screenConnect,
                         bool gmBridgeConnect);


        // may be replaced with database
        // Function: makes sure the B+ voltage does not exceed maximum allowable currents
        // pre: vBPlus must be 10 - 260 volts in 10 volt increments
        // param:   vBPlus - required B+ voltage
        //          current - maximum current
        // See WE Cardmatic manual, section 5.54 for more details
        bool B_plusCurrentCheck(unsigned int vBPlus,
                                unsigned int current);


        // umhometer shunt select function
        // pre: full scale range of gm must be between 500 - 128,000
        //      500-26,000 umho in 100 umho steps; 26,000-128,000 umho in 500 umho steps
        // param:   gm - mutual conductance
        // post: all necessary shunt switches are activated
        // see WE Cardmatic manual, sections 5.55, 5.56 for more details
        // some of the depressed switches mentioned in table in page 27 are incorrect
        void umho_meterShunt(unsigned long gm);


        // helper to determine shunt value
        // pre: dchoice must be between 0 to 255
        // param:   dchoice - required choice number of meter shunt
        // post: all necessary shunt switches are activated
        // see WE Cardmatic manual, section 5.55 more details
        void meterShuntValue(unsigned int dchoice);


        // mA meter shunt select function
        // pre: range must be between 100 - 510100 uA (0.1mA to 510.1mA)
        // param:   fsCurrent - current in MICROamperes
        // post: all necessary shunt switches are activated
        // see WE Cardmatic manual, section 5.60 for more details
        void ma_meterShunt(unsigned long fsCurrent);


        // Function: conduct plate current test with regulated B+ source
        // pre: range must be between 0 - 510100 uA (0mA to 510.1mA)
        // param:   fsCurrent - current in MICROamperes
        // post: all necessary switches are activated
        // see WE Cardmatic manual, section 5.59 for more details
        void plateCurrentTest(unsigned long fsCurrent);


        // helper to set decade resistor value
        // pre: max dval = 70000 (71100) in ohms
        //      dval <= 1k can operate <= DECADE_RES_IMAX_1K
        //      1K < dval <= 70K  can operate <= DECADE_RES_VMAX
        // param:   dVal - demanded resistance value
        // post: all unactivated switches correspond to required resistance value
        // see WE Cardmatic manual, sections 5.57, 5.58 for more details
        void decadeResistor(unsigned long dVal);


        // grid bias set helper
        // pre: abs(vGrid) <= V_BIAS_MAX in 0.1 volt steps
        // param:   vGrid - grid bias required in volts
        //          biasType - FIXED_BIAS or SELF_BIAS
        //          gridSignal - .222V signal input, 1 for yes, 0 for no
        // post: all necessary bias switches are activated
        // see WE Cardmatic manual, sections 5.57, 5.58 for more details
        bool gridBias(double vGrid,
                      Biasing biasType,
                      bool gridSignal);


        // pre: setTwinTriodeSwitches function is not used
        // assert: tube.sections.front().cathode...tube.sections.back().cathode
        void leakageTest(Tube &tube,
                         unsigned int reqRejectCurrent,
                         unsigned int section);


        // sets heater-cathode leakage meter sensitivity
        // function selects leakage current shunts for proper display at rejection (10% fs)
        // pre: G17 is open, rows 1 and 2 are used for a heater, row 4 used for a cathode
        //      reqMeterValue must be 10, 20, 50, 70, 100, 150, (165/170)
        // param: reqRejectCurrent - current at rejection
        // see WE Cardmatic manual, section 5.53, 5.63 for more details
        void leakageShunt(unsigned int reqRejectCurrent);


        // recommended heater-cathode leakage meter sensitivity for diodes and rectifiers
        // function selects leakage current shunts for proper display at rejection (10% fs)
        // see WE Cardmatic manual, section 5.53, 5.63 for more details
        // param:   type - of diode/rectifier (see enum for options)
        void rec_diodeLeakage(Tube &tube, DiodeType type);


        // tests high voltage diodes and high and low perveance diodes
        // param:  fsCurrent - full scale current
        //         b_plusType - regulated_Bplus or auxiliary_Bplus
        //         currentLimiting - requires current limiting resistors?
        //                              0 = no, 1 = yes
        //         rCurrentLimiting - current limiting resistor value
        // post: all necessary switches are activated
        void diodeTest(unsigned int fsCurrent,
                       Bplus b_plusType,
                       bool currentLimiting,
                       unsigned int rCurrentLimiting);


        // tests half wave rectifiers
        // pre: meter sensitivity set to 63 on 0-100 scale if max inverse voltage
        //         is > 330V, and follow procedure in WE Cardmatic manual 5.64b)
        // param:  rLoad - resistive load, in ohms
        //         maxInvRating - maximum inverse rating of diode, in volts
        //         mSensitiviy - meter sensitivity
        // post: all necessary switches are activated
        void halfWaveRectifierTest(unsigned int rLoad,
                                   unsigned int maxInvRating,
                                   unsigned int mSensitivity);


        // tests full wave rectifiers
        // pre: maximum current limited by rating of decade resistor
        //         see section 5.57-5.58 for more details
        // param:  rLoad - resistive load, in ohms
        //         maxInvRating - maximum inverse rating of diode, in volts
        //         mSensitiviy - meter sensitivity
        // post: all necessary switches are activated
        void fullWaveRectifierTest(Tube &tube,
                                   unsigned int rLoad,
                                   unsigned int maxInvRating,
                                   unsigned int mSensitivity);


        // tests damper diodes
        // pre: rLoad is <5500 ohms or 92mA
        // param:  rLoad - resistive load, in ohms
        //         maxInvRating - maximum inverse rating of diode, in volts
        //         mSensitiviy - meter sensitivity
        // post: all necessary switches are activated
        void damperDiodeTest(Tube &tube,
                             unsigned int rLoad,
                             unsigned int mSensitivity);
};


#endif // CARDMATIC_CARDPOS_H
