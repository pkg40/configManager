////////////////////////////////////////////////////////////////////////
// EEPROM Address Definitions - X-Macro Pattern for Robust Synchronization
//-----------------------------------------------------------------------------------------------------------
// This file uses the X-Macro pattern to ensure enum values, addressMap entries,
// and string arrays stay perfectly synchronized. Define addresses once here with
// their section/key/name, and everything is generated automatically.
//
// Usage:
//   1. Add new addresses to EEPROM_ADDRESS_LIST below
//   2. Include this file where you need the enum or arrays
//   3. Use GENERATE_EEPROM_ENUM() to generate the enum
//   4. Use GENERATE_EEPROM_ADDRESS_MAP() to generate the addressMap array
//   5. Use GENERATE_EEPROM_STRING_ARRAY() to generate the string array
////////////////////////////////////////////////////////////////////////

#pragma once

// Define all EEPROM addresses with their metadata (section, key, name, enumName)
// Format: EEPROM_ADDRESS(section, key, name, enumName)
// If name is empty string "", the enum name will be used as the name
#define EEPROM_ADDRESS_LIST(X) \
    X("display", "dummy", "eeDUMMY", eeDUMMY) \
    X("display", "mode", "eeDISPMODE", eeDISPMODE) \
    X("display", "brightness", "eeDISPBRIGHT", eeDISPBRIGHT) \
    X("display", "sleep", "eeDISPSLEEP", eeDISPSLEEP) \
    X("testmode", "ota_mode", "eeOTA", eeOTA) \
    X("application", "role", "eeROLEIS", eeROLEIS) \
    X("display", "menubuttons", "eeBUTTONMENU", eeBUTTONMENU) \
    X("display", "fullscreen", "eeFULLSCREEN", eeFULLSCREEN) \
    X("flags", "config", "eeCONFIG", eeCONFIG) \
    X("flags", "wireless", "eeWIRELESS", eeWIRELESS) \
    X("input", "direction", "eeDIRECTION", eeDIRECTION) \
    X("setpoints", "deviceon", "eeDEVICEEN", eeDEVICEEN) \
    X("setpoints", "idle", "eeIDLE", eeIDLE) \
    X("setpoints", "present", "eePRESENT", eePRESENT) \
    X("setpoints", "last", "eeLAST", eeLAST) \
    X("setpoints", "max", "eeMAX", eeMAX) \
    X("auto", "step1", "eeAUTO1", eeAUTO1) \
    X("auto", "step2", "eeAUTO2", eeAUTO2) \
    X("auto", "step3", "eeAUTO3", eeAUTO3) \
    X("auto", "step4", "eeAUTO4", eeAUTO4) \
    X("auto", "step5", "eeAUTO5", eeAUTO5) \
    X("auto", "step6", "eeAUTO6", eeAUTO6) \
    X("auto", "step7", "eeAUTO7", eeAUTO7) \
    X("auto", "step8", "eeAUTO8", eeAUTO8) \
    X("auto", "period", "eeAUTOPERIOD", eeAUTOPERIOD) \
    X("auto", "steps", "eeAUTOSTEPS", eeAUTOSTEPS) \
    X("auto", "repeats", "eeAUTOREPEATS", eeAUTOREPEATS) \
    X("memory1", "address1", "eeMEM1A", eeMEM1A) \
    X("memory1", "address2", "eeMEM1B", eeMEM1B) \
    X("memory1", "address3", "eeMEM1C", eeMEM1C) \
    X("memory1", "address4", "eeMEM1D", eeMEM1D) \
    X("memory1", "address5", "eeMEM1E", eeMEM1E) \
    X("memory2", "address1", "eeMEM2A", eeMEM2A) \
    X("memory2", "address2", "eeMEM2B", eeMEM2B) \
    X("memory2", "address3", "eeMEM2C", eeMEM2C) \
    X("memory2", "address4", "eeMEM2D", eeMEM2D) \
    X("memory2", "address5", "eeMEM2E", eeMEM2E) \
    X("memory3", "address1", "eeMEM3A", eeMEM3A) \
    X("memory3", "address2", "eeMEM3B", eeMEM3B) \
    X("memory3", "address3", "eeMEM3C", eeMEM3C) \
    X("memory3", "address4", "eeMEM3D", eeMEM3D) \
    X("memory3", "address5", "eeMEM3E", eeMEM3E) \
    X("memory4", "address1", "eeMEM4A", eeMEM4A) \
    X("memory4", "address2", "eeMEM4B", eeMEM4B) \
    X("memory4", "address3", "eeMEM4C", eeMEM4C) \
    X("memory4", "address4", "eeMEM4D", eeMEM4D) \
    X("memory4", "address5", "eeMEM4E", eeMEM4E) \
    X("memory5", "address1", "eeMEM5A", eeMEM5A) \
    X("memory5", "address2", "eeMEM5B", eeMEM5B) \
    X("memory5", "address3", "eeMEM5C", eeMEM5C) \
    X("memory5", "address4", "eeMEM5D", eeMEM5D) \
    X("memory5", "address5", "eeMEM5E", eeMEM5E) \
    X("memory6", "address1", "eeMEM6A", eeMEM6A) \
    X("memory6", "address2", "eeMEM6B", eeMEM6B) \
    X("memory6", "address3", "eeMEM6C", eeMEM6C) \
    X("memory6", "address4", "eeMEM6D", eeMEM6D) \
    X("memory6", "address5", "eeMEM6E", eeMEM6E) \
    X("calibration", "offset", "eeCALOFFSET", eeCALOFFSET) \
    X("calibration", "limit", "eeCALLIMIT", eeCALLIMIT) \
    X("calibration", "stepsize", "eeCALSTEP", eeCALSTEP) \
    X("calibration", "scalefactor", "eeCALSCALE", eeCALSCALE) \
    X("calibration", "direction", "eeCALDIR", eeCALDIR) \
    X("calibration", "period", "eeCALPERIOD", eeCALPERIOD) \
    X("calibration", "calibrate", "eeCALIBRATE", eeCALIBRATE) \
    X("flags", "rebootflag", "eeREBOOT", eeREBOOT) \
    X("testmode", "stresstest", "eeSTRESSTEST", eeSTRESSTEST) \
    X("flags", "factoryreset", "eeFACTORY", eeFACTORY) \
    X("flags", "onoff", "eeONOFF", eeONOFF) \
    X("flags", "diagnostic", "eeDIAGNOSTIC", eeDIAGNOSTIC) \
    X("flags", "sound", "eeSOUND", eeSOUND) \
    X("flags", "exit", "eeEXIT", eeEXIT) \
    X("flags", "commit", "eeCOMMIT", eeCOMMIT) \
    X("calibration", "power", "eePOWER", eePOWER)

// Macro to generate enum entry (extract just the enum name)
#define EEPROM_ENUM_ENTRY(section, key, name, enumName) enumName,

// Macro to generate addressMap entry
// Note: Empty string "" in name field means use enum name - handled by providing enum name in list
#define EEPROM_ADDRESS_MAP_ENTRY(section, key, name, enumName) {section, key, name},

// Macro to generate string array entry
#define EEPROM_STRING_ENTRY(section, key, name, enumName) name,

// Macro to generate enum
#define GENERATE_EEPROM_ENUM() \
    typedef enum : uint8_t { \
/*        eeDUMMY = 0, \ */ \
        EEPROM_ADDRESS_LIST(EEPROM_ENUM_ENTRY) \
        EEPROMRECORDS \
    } eePromAddress_t;

// Macro to generate addressMap array
#define GENERATE_EEPROM_ADDRESS_MAP() \
    static const stringTriple addressMap[] = { \
        EEPROM_ADDRESS_LIST(EEPROM_ADDRESS_MAP_ENTRY) \
    };

// Macro to generate string array
#define GENERATE_EEPROM_STRING_ARRAY() \
    const char* const eePromAddressStrings[] = { \
        EEPROM_ADDRESS_LIST(EEPROM_STRING_ENTRY) \
    };

// Helper to count addresses (excluding EEPROMRECORDS)
#define EEPROM_COUNT_ENTRY(section, key, name, enumName) +1
#define EEPROM_COUNT (0 EEPROM_ADDRESS_LIST(EEPROM_COUNT_ENTRY))

// Compile-time check: ensure EEPROMRECORDS matches the count
// Note: EEPROMRECORDS is the last enum value, so its numeric value equals EEPROM_COUNT
#define EEPROM_COUNT_CHECK() \
    static_assert(EEPROMRECORDS == EEPROM_COUNT, \
        "EEPROMRECORDS must match the number of addresses in EEPROM_ADDRESS_LIST");

