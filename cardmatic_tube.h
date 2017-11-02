//    Cardmatic card generator - cardmatic_tube.h file
//    Structs model a tube and its test settings

//    Written by: cathug



#ifndef CARDMATIC_TUBE_H
#define CARDMATIC_TUBE_H


#include <unordered_map>


// the cardreader containing number and letter switches
typedef std::unordered_multimap<char,unsigned int> CardReader;
//typedef std::array<array<bool, 11>,17> CardReader;


typedef enum RowSwitches : unsigned int
{
    ROW_NONE,
    ROW_1,
    ROW_2,
    ROW_3,
    ROW_4,
    ROW_5,
    ROW_6,
    ROW_7,
    ROW_8,
    ROW_9,
    ROW_10,
    ROW_11,
    ROW_12,
    ROW_13,
    ROW_14,
    ROW_15,
    ROW_16,
    ROW_17,
}RowSwitches;


//typedef struct TestParam
//{
//    bool twin;              // twin tube?
//    unsigned int bPlus;     // B+ voltage
//    double heater;          // heater voltage
//    bool dcHeater;          // 0 off (ac heater) / 1 on (dc heater)
//    bool leakage;           // leakage test
//    unsigned int umhoFS;    // meter full scale umho
//}TestParam;






#endif // CARDMATIC_TUBE_H
