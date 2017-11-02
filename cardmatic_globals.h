//    Cardmatic card generator - cardmatic_globals.h file

//    declare global constants and macros for all files (almost)

//    Written by: cathug



#ifndef CARDMATIC_GLOBALS_H
#define CARDMATIC_GLOBALS_H


#define CARDMATIC_MODEL 15784	// Western Electric model KS15784A or KS15784B
//#define CARDMATIC_MODEL 1234	// Hickok model 1234A or 1234B
//#define CARDMATIC_MODEL 118	// military model USM118


// card reader switch
#define SW_LETTER_MIN 'A'
#define SW_LETTER_MAX 'L'
//#define SW_LETTER_MIN Sletter::A + 1;
//#define SW_LETTER_MAX Sletter::L + 1;
//#define SW_NUM_MIN 1
//#define SW_NUM_MAX 17
//const uint8_t* K_SW_NUM_RANGE = {3,4,6,7};
#define ATOH_SW_NUM_MAX 8
#define J_SW_NUM_MAX 7


// filament
#define V_HEATER_MIN 0.0        // min heater volts
#define V_HEATER_MAX_AC 119.9   // max AC heater volts
#define V_HEATER_MAX_DC 50.0    // max DC heater volts
#define V_HEATER_MAX_CT 12.6    // max DC Volts if CT chosen
#define V_HEATER_INC 0.1        // increment
#define IMAX_V_HEATER_DC 500    // max supply current in mA
#define IMAX_V_HEATER_120V 100  // max supply current 21-119.9V in mA < 5% drop
#define IMAX_V_HEATER_20V 600   // max supply current 5.1-20V in mA < 5% drop
#define IMAX_V_HEATER_5V 4000   // max supply current 5V in mA <15% drop


// regulated B+ volts
#define V_REGBPLUS_MAX 260      // maximum B+ range
#define V_REGBPLUS_MIN 10       // minimum B+ range
#define V_REGBPLUS_INC 10       // B+ increment step
#define V_REGBPLUS_210 210
#define V_REGBPLUS_160 160
#define V_REGBPLUS_110 110
#define V_REGBPLUS_50 50


// bias
#if CARDMATIC_MODEL == 118
    #define V_BIAS_MAX 120.0        // maximum bias voltage
#elif CARDMATIC_MODEL == 15784 || CARDMATIC_MODEL == 1234
    #define V_BIAS_MAX 100.0        // maximum bias voltage
#endif


// meter range
#define METER_FS_I_MIN 100              // min fs in uA
#define METER_FS_I_MAX_LOW 5200         // max fs low range in uA
#define METER_FS_I_MAX_MID 25600        // max fs mid range in uA
#define METER_FS_I_MAX_HIGH 510100      // max fs high range in uA
#define METER_FS_GM_MIN 500             // minimum gm range in mho
#define METER_FS_GM_MAX_LOW 26000       // max lower gm range in mho
#define METER_FS_GM_MAX_HIGH 128000     // max upper gm range in mho
#define METER_FS_GM_INC_LOW 100         // for 500-26000 uho
#define METER_FS_GM_INC_HIGH 500        // for 26000-128000 uho
#define NUM_POSSIBLE_GM_VALUES 256      // 256 different possible gm values
#define PRIMARY_CHOICE_MIN 1            // smallest primary choice
#define PRIMARY_CHOICE_MAX 128          // largest primary choice


// nominal heater-cathode leakage values
#define I_NOM_HC_LEAKAGE_10 10
#define I_NOM_HC_LEAKAGE_20 20
#define I_NOM_HC_LEAKAGE_50 50
#define I_NOM_HC_LEAKAGE_70 70
#define I_NOM_HC_LEAKAGE_100 100
#define I_NOM_HC_LEAKAGE_150 150
#if CARDMATIC_MODEL == 118
    #define I_NOM_HC_LEAKAGE_165 165	
#endif	// this is listed in the usm118 spec sheet and not other models

// decade resistor
#define DECADE_RES_MAX 70000
#define DECADE_RES_INC 10
#define DECADE_RES_IMAX_1K 200
#define DECADE_RES_VMAX 200


#define DAMPER_MAXINVRATING 1200
#define HWTHRESHOLD_MAXINVRATING 330



#endif // CARDMATIC_GLOBALS_H
