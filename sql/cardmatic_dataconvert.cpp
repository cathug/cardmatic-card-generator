//    Cardmatic card generator - cardmatic_dataconvert.cpp file
//    C++11 implementation file

//    Function definitions in DataConverter class converts selected AVO VCM163 
//      test data to test card settings for Cardmatic Tube Testers.

//    Written by: cathug


#include <regex>
#include <utility>
#include "cardmatic_dataconvert.h"
#include <iostream>



//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------

// default constructor
DataConverter::DataConverter() :
    AVOSwitchCodeToCardmaticRow {
        {'0', 0},   // nothing to map.
        {'1', 4},   // map cathode to           cardmatic row 4
        {'2', 2},   // map heater- to           cardmatic row 2
        {'3', 1},   // map heater+ to           cardmatic row 1
        {'4', 3},   // map control grid 1 to    cardmatic row 3
        {'5', 6},   // map control grid 2 to    cardmatic row 6
        {'6', 6},   // map control grid 3 to    cardmatic row 6 (second card)
        {'7', 5},   // map screen grid to       cardmatic row 5
        {'8', 7},   // map anode to             cardmatic row 7
        {'9', 7},   // map 1st diode anode to   cardmatic row 7
        {'X', 7},   // map 2nd diode anode to   cardmatic row 7 (second card)
        {'Y', 7},   // map 3rd diode anode to   cardmatic row 7 (third card)
        {'Z', 7}    // map 4th diode anode to   cardmatic row 7 (forth card)
    }
{

}

//------------------------------------------------------------------------------

// default destructor
DataConverter::~DataConverter()
{
};   

//------------------------------------------------------------------------------

// function mapping tube data as per AVO23 manual to Cardmatic switch settings
// pre: VCM163_double and VCM163_text are in proper AVO VCM163 formatting, and
//      tube must be measurable by Hickok tester
// param:   VCM163_text - tube type, switch setting, topcap, base, class
//          VCM163_double - tube test parameters Vh, Vg1, Va, Vg2, Ia, gm 
// returns: true if parsing is sucessful, false otherwise
// TODO: add support to database
bool DataConverter::parseAVOData(const std::string* VCM163_text, 
                                 const double* VCM163_double)
//                                 const unsigned int num_entries)
{
    char switch_column_index = 'A';
    unsigned int numTubePins;
    TopCapStatus cap_status;
//    auto mapping;
//    auto it;

    if (VCM163_double == NULL || VCM163_text == NULL) { return false; }
  
    // Cardmatic can test only tubes with only max 9 pins.
    // Also each tube has at least 2 pins.
    numTubePins = getNumTubePins(VCM163_text[BASE]); 
    if (numTubePins < 2 || numTubePins > 9) { return false; }


    // otherwise, do the following
    cap_status = tubeHasTopCap(VCM163_text[TOP_CAP]);
    if (cap_status == CANNOT_TEST) { return false; }
    else if (cap_status == HAS_TOP_CAP)
    {
        std::cout << "Processing AVO VCM163 Top Cap Settings." << std::endl;
        for (auto it = VCM163_text[TOP_CAP].begin(); 
            it < VCM163_text[TOP_CAP].end(); it++)
        {
            if (setCardmaticSwitchUsingAVOSwitchCode(*it, 
                'K') == false) { return false; } 
        }
    } // do nothing if cap_status == NO_TOP_CAP
    
    
    std::cout << "Processing AVO VCM163 Switch Settings." << std::endl;
    for (auto it = VCM163_text[SWITCH_SETTINGS].begin(); 
        it < VCM163_text[SWITCH_SETTINGS].end(); it++)
    {
        if (*it != ' ')
        {
            if (setCardmaticSwitchUsingAVOSwitchCode(*it, 
                switch_column_index) == false) { return false; }
            
            // fix the missing 'I' column
            if (switch_column_index == 'H') { switch_column_index += 2; }
            else { switch_column_index++; }
        }
    }
    
    // TODO: need to add test parameters
    
    return true;
}

//------------------------------------------------------------------------------

// helper to check if tube has no top cap, one top cap,
//   or two top caps (cannot test)
// pre: string length of two
// param: AVOtopCapValue: top cap data from AVO settings manual
// returns: NO_TOP_CAP, HAS_TOP_CAP, or CANNOT_TEST
TopCapStatus DataConverter::tubeHasTopCap(const std::string AVOtopCapValue)
{
    if (AVOtopCapValue.length() == 2)
    {
        if (AVOtopCapValue == "00") { return NO_TOP_CAP; }
        
        // if one or the other character is non-zero
        if ( (*AVOtopCapValue.begin() != '0' && 
                *AVOtopCapValue.rbegin() == '0') || 
             (*AVOtopCapValue.begin() == '0' && 
                *AVOtopCapValue.rbegin() != '0') )
        {
            return HAS_TOP_CAP;
        }    
    }
    
    // else - unrecognized pattern or two non-zero characters 
    //  (tube with two top caps)
    return CANNOT_TEST;  
}

//------------------------------------------------------------------------------

// helper using switch code as per AVO23 manual to set Cardmatic switch
// param:   AVOSwitchCode - a switch setting substring of size 1 
//          cardmaticTubePinPos - tube pin position in cardmatic nomenclature
// returns: false if mapping not found; otherwise return true
bool DataConverter::setCardmaticSwitchUsingAVOSwitchCode(
    const char AVOSwitchCode, 
    const char cardmaticTubePinPos)
{
    auto mapping = AVOSwitchCodeToCardmaticRow.find(AVOSwitchCode);
    if (mapping == AVOSwitchCodeToCardmaticRow.end() ) { return false; }
            
    if (mapping->second != 0)
    {
        swClose.insert( std::make_pair(cardmaticTubePinPos, mapping->second) );
//        std::cout << "just inserted in swClose: " << cardmaticTubePinPos << 
//            mapping->second << std::endl;
    }
    
    return true;
}

//------------------------------------------------------------------------------

// helper to extract number of pins from AVO tube base information
// pre: the tube base must contain at least one digit
// param: tubeBase: tube base data from AVO settings manual
// returns: number of pins on tube base, or 0 if tube base is invalid
unsigned int DataConverter::getNumTubePins(const std::string tubeBase)
{
    // possible cases
    //    5AA, 7AA, 8SC, A08, A10, A12, B3G, B5, B4, B5A, B5B, B7, 
    //    B7A, B7G, B8A, B8B, B8G, B8D, B9, B9A, B9D, B9G, B10B,
    //    F8, M08, NV5, NV7, SM4, SM5, SM7, UX4, UX5, UX6, UX7 

    std::regex reg("\\d+"); // keep digits only
    std::smatch match;
    if (std::regex_search(tubeBase, match, reg) )
    {
        for (auto value: match) { return std::stoi(value, nullptr); }  
    }  
    
    return 0; 
}

//------------------------------------------------------------------------------

// helper to derive number of test cards required to test tube
// param: AVOSwitchCode: one of AVO switch code 1-9, X-Z
// returns: MAP_ERROR, MAP_CARD1, MAP_CARD2, MAP_CARD3, MAP_CARD4
MappingStatus DataConverter::cardNumber(const char AVOSwitchCode)
{
    switch (AVOSwitchCode)
    {
        case '0': case '1': case '2': case '3': case '4': 
        case '5': case '7': case '8': case '9': 
            break;
        case '6' : case 'X':   // control grid 3 or second diode anode
            return MAP_CARD2;      // map to cardmatic row 7 (second card)
        case 'Y':   // third diode anode
            return MAP_CARD3;      // map to cardmatic row 7 (third card)
        case 'Z':   // forth diode anode
            return MAP_CARD4;      // map to cardmatic row 7 (forth card)
        default:
            return MAP_ERROR;    
    }

    return MAP_CARD1;
    
    // TODO: finish this
}

//------------------------------------------------------------------------------

// function to print set of closed cardmatic switches
void DataConverter::outputClosedCardmaticSwitches()
{
    std::cout << "Outputing Cardmatic Switches to Close:" << std::endl;
    for (auto it = swClose.begin(); it != swClose.end(); it++)
    {
        std::cout << it->first << it->second << ","; 
    }
    std::cout << std::endl;
}

//------------------------------------------------------------------------------
// End of implementation
//------------------------------------------------------------------------------
