//    Cardmatic card generator - cardmatic_dataconvert.h file
//    C++11 header file

//    Function definitions in DataConverter class converts selected AVO VCM163 
//      test data to test card settings for Cardmatic Tube Testers.

//    Written by: cathug


#ifndef CARDMATIC_DATACONVERT_H
#define CARDMATIC_DATACONVERT_H

#include <string>
#include <unordered_map>

//------------------------------------------------------------------------------
//  enums
//------------------------------------------------------------------------------

typedef enum VCM163Param_text : unsigned int
{
    TUBE_ID,
    SWITCH_SETTINGS,
    TOP_CAP,
    BASE,
    CLASS,
}VCM163Param_text;


typedef enum VCM163Param_double
{
    HEATER,
    V_GRID1,
    V_ANODE,
    V_GRID2,
    I_ANODE,
    GM,
}VCM163Param_double;


typedef enum topCapStatus
{
    NO_TOP_CAP,
    HAS_TOP_CAP,
    CANNOT_TEST,
}TopCapStatus;


typedef enum mappingStatus
{
    MAP_ERROR,
    MAP_CARD1,
    MAP_CARD2,
    MAP_CARD3,
    MAP_CARD4,
}MappingStatus;



//------------------------------------------------------------------------------
//  Class
//------------------------------------------------------------------------------
class DataConverter
{
    public:
        //----------------------------------------------------------------------
        //  constructor and destructor
        //----------------------------------------------------------------------
   
        DataConverter();    // default constructor

        ~DataConverter();   // default destructor



        //----------------------------------------------------------------------                                                          
        //  member functions
        //----------------------------------------------------------------------
                
        // function mapping tube data as per AVO23 manual to Cardmatic switch
        //  settings
        // pre: VCM163_double and VCM163_text are in proper AVO VCM163
        //      formatting, and tube must be measurable by Hickok tester
        // param:  VCM163_text - tube type, switch setting, topcap, base, class
        //         VCM163_double - tube test parameters Vh, Vg1, Va, Vg2, Ia, gm
        // returns: true if parsing is sucessful, false otherwise
        bool parseAVOData(const std::string* VCM163_text, 
                          const double* VCM163_double);
//                          const unsigned int num_entries);
                          
        
        // function to print set of closed cardmatic switches
        void outputClosedCardmaticSwitches();




    private:
        //----------------------------------------------------------------------
        //  variables
        //----------------------------------------------------------------------
        
        // set of switches to close in cardreader
        std::unordered_multimap<char,unsigned int> swClose;  


        // a dictionary that maps switch code parameter to cardmatic row number
        const std::unordered_map<char, 
            unsigned int> AVOSwitchCodeToCardmaticRow;
        
        
        //----------------------------------------------------------------------
        //  helpers
        //----------------------------------------------------------------------
        
        // TODO: need to add test parameters
        // helper to check if tube has no top cap, one top cap, or two top 
        //  caps (cannot test)
        // pre: string length of two
        // param: AVOtopCapValue: top cap data from AVO settings manual
        // returns: NO_TOP_CAP, HAS_TOP_CAP, or CANNOT_TEST
        TopCapStatus tubeHasTopCap(const std::string AVOtopCapValue);
        
        
        // helper using switch code as per AVO23 manual to set Cardmatic switch
        // param:   AVOSwitchCode - a switch setting substring of size 1 
        //          cardmaticTubePinPos - tube pin position in cardmatic 
        //              nomenclature
        // returns: false if mapping not found; otherwise return true
        bool setCardmaticSwitchUsingAVOSwitchCode(
            const char AVOSwitchCode, 
            const char cardmaticTubePinPos);
            
            
        // helper to extract number of pins from AVO tube base information
        // pre: the tube base must contain at least one digit
        // param: tubeBase: tube base data from AVO settings manual
        // returns: number of pins on tube base, or 0 if tube base is invalid
        unsigned int getNumTubePins(const std::string tubeBase);
        

        // TODO: finish this
        // helper to derive number of test cards required to test tube
        // param: AVOSwitchCode: one of AVO switch code 1-9, X-Z
        // returns: MAP_ERROR, MAP_CARD1, MAP_CARD2, MAP_CARD3, MAP_CARD4
        MappingStatus cardNumber(const char AVOSwitchCode); 
};

#endif // CARDMATIC_DATACONVERT_H
