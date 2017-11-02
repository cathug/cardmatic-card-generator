//    Cardmatic card generator - test_main2.cpp file
//    C++11 main function file

//    It tests functions in classes DataConverter and Database

//    Written by: cathug



#include "cardmatic_sql.h"
#include "cardmatic_dataconvert.h"
#include <iostream>

// test!
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "Two arguments needed." << std::endl;
        return -1;
    }
    
    DataConverter d;
    // test cases for dataconverter functions
//    std::cout << d.getNumPins("5AA") << std::endl;
//    std::cout << d.getNumPins("A08") << std::endl;
//    std::cout << d.getNumPins("A10") << std::endl;
//    std::cout << d.getNumPins("B3G") << std::endl;
//    std::cout << d.getNumPins("B5") << std::endl;
//    std::cout << d.getNumPins("B5A") << std::endl;
//    std::cout << d.getNumPins("UX6") << std::endl;

//    std::cout << d.tubeHasTopCap("0Z") << std::endl;
//    std::cout << d.tubeHasTopCap("Y0") << std::endl;
//    std::cout << d.tubeHasTopCap("00") << std::endl;
//    std::cout << d.tubeHasTopCap("12") << std::endl;
//    std::cout << d.tubeHasTopCap("132") << std::endl;
    
//    for (char i = '0'; i <= '9'; i++)
//    {
//        std::cout << d.cardNumber(i) << std::endl;
//    }
//    
//    std::cout << d.cardNumber('X') << std::endl;
//    std::cout << d.cardNumber('Y') << std::endl;
//    std::cout << d.cardNumber('Z') << std::endl;
    
    
    
    Database db;
    if (db.dbOpen("cardmatic.sqlite", SQLITE_OPEN_READONLY) == false)
    //    db.dbOpen(fileName, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    {
        return -1;
    }
    
    
    const char* param[] = {
        argv[1]
    };
    int paramSize = argc - 1;
    
    db.dbQuery(param, paramSize, "avocardmatic");
    
    
    
//    std::string VCM_str[] = {"ECC83", "851 228 413 0000", "00", "B9A", "TT" };
//    double VCM_double[] = {6.3, 2, 250, 0, 1.2, 1.6};
    
//    d.parseAVOData(VCM_str, VCM_double);
    
    d.parseAVOData(db.getTubeData_str(), db.getTubeData_double());
    d.outputClosedCardmaticSwitches();
    
    
    
    
    
    
    db.dbClose();    // close database
    return 0;
}
