// EEPROM Address to Config Section/Key Mapping
// Maps EEPROM addresses to JSON config section/key pairs for device state
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Arduino.h>
#include <string.h>
#include "../../motorController/motorControllerEEPROM_Addresses.hpp"

// EEPROM Address to Config Mapping Structure
struct EepromConfigMapping {
    const char* section;
    const char* key;
    const char* description;
};

// Static const mapping objects to avoid temporary object issues
// Mapped to match existing savedState.json structure
namespace EepromMappings {
    // Display Configuration (matches savedState.json "display" section)
    static const EepromConfigMapping DISPLAY_MODE = {"display", "mode", "Display mode"};
    static const EepromConfigMapping DISPLAY_BRIGHTNESS = {"display", "brightness", "Display brightness"};
    static const EepromConfigMapping DISPLAY_SLEEP = {"display", "sleep", "Display sleep timeout"};
    static const EepromConfigMapping DISPLAY_DUMMY = {"display", "dummy", "Display dummy field"};
    
    // Device Configuration (mapped to existing sections)
    static const EepromConfigMapping DEVICE_OTA_MODE = {"testmode", "ota_mode", "OTA mode"};
    static const EepromConfigMapping DEVICE_ROLE = {"application", "role", "Device role (master/slave)"};
    static const EepromConfigMapping DEVICE_BUTTON_MENU = {"display", "menubuttons", "Button menu mode"};
    static const EepromConfigMapping DEVICE_FULLSCREEN = {"display", "fullscreen", "Full screen menu mode"};
    static const EepromConfigMapping DEVICE_MAC_KEY = {"application", "mac_key", "MAC key"};
    static const EepromConfigMapping DEVICE_MAC_ADDR = {"application", "mac_addr", "MAC address modifier"};
    static const EepromConfigMapping DEVICE_INPUT_DIR = {"input", "direction", "Input direction"};
    
    // Motor Control State (matches savedState.json "setpoints" section)
    static const EepromConfigMapping MOTOR_DEVICE_ON = {"setpoints", "deviceon", "Device on/off state"};
    static const EepromConfigMapping MOTOR_IDLE = {"setpoints", "idle", "Idle speed"};
    static const EepromConfigMapping MOTOR_LAST = {"setpoints", "last", "Last speed"};
    static const EepromConfigMapping MOTOR_MAX = {"setpoints", "max", "Maximum speed"};
    
    // Auto Speed Values (matches savedState.json "auto" section)
    static const EepromConfigMapping AUTO1 = {"auto", "step1", "Auto speed 1"};
    static const EepromConfigMapping AUTO2 = {"auto", "step2", "Auto speed 2"};
    static const EepromConfigMapping AUTO3 = {"auto", "step3", "Auto speed 3"};
    static const EepromConfigMapping AUTO4 = {"auto", "step4", "Auto speed 4"};
    static const EepromConfigMapping AUTO5 = {"auto", "step5", "Auto speed 5"};
    static const EepromConfigMapping AUTO6 = {"auto", "step6", "Auto speed 6"};
    
    // Memory Values A (matches savedState.json "memory1" section)
    static const EepromConfigMapping MEM1A = {"memory1", "address1", "Memory value 1A"};
    static const EepromConfigMapping MEM2A = {"memory1", "address2", "Memory value 2A"};
    static const EepromConfigMapping MEM3A = {"memory1", "address3", "Memory value 3A"};
    static const EepromConfigMapping MEM4A = {"memory1", "address4", "Memory value 4A"};
    static const EepromConfigMapping MEM5A = {"memory1", "address5", "Memory value 5A"};
    static const EepromConfigMapping MEM6A = {"memory1", "address6", "Memory value 6A"};
    
    // Memory Values B (matches savedState.json "memory2" section)
    static const EepromConfigMapping MEM1B = {"memory2", "address1", "Memory value 1B"};
    static const EepromConfigMapping MEM2B = {"memory2", "address2", "Memory value 2B"};
    static const EepromConfigMapping MEM3B = {"memory2", "address3", "Memory value 3B"};
    static const EepromConfigMapping MEM4B = {"memory2", "address4", "Memory value 4B"};
    static const EepromConfigMapping MEM5B = {"memory2", "address5", "Memory value 5B"};
    static const EepromConfigMapping MEM6B = {"memory2", "address6", "Memory value 6B"};
    
    // Memory Values C (matches savedState.json "memory3" section)
    static const EepromConfigMapping MEM1C = {"memory3", "address1", "Memory value 1C"};
    static const EepromConfigMapping MEM2C = {"memory3", "address2", "Memory value 2C"};
    static const EepromConfigMapping MEM3C = {"memory3", "address3", "Memory value 3C"};
    static const EepromConfigMapping MEM4C = {"memory3", "address4", "Memory value 4C"};
    static const EepromConfigMapping MEM5C = {"memory3", "address5", "Memory value 5C"};
    static const EepromConfigMapping MEM6C = {"memory3", "address6", "Memory value 6C"};
    
    // Memory Values D (matches savedState.json "memory4" section)
    static const EepromConfigMapping MEM1D = {"memory4", "address1", "Memory value 1D"};
    static const EepromConfigMapping MEM2D = {"memory4", "address2", "Memory value 2D"};
    static const EepromConfigMapping MEM3D = {"memory4", "address3", "Memory value 3D"};
    static const EepromConfigMapping MEM4D = {"memory4", "address4", "Memory value 4D"};
    static const EepromConfigMapping MEM5D = {"memory4", "address5", "Memory value 5D"};
    static const EepromConfigMapping MEM6D = {"memory4", "address6", "Memory value 6D"};
    
    // Memory Values E (matches savedState.json "memory5" section)
    static const EepromConfigMapping MEM1E = {"memory5", "address1", "Memory value 1E"};
    static const EepromConfigMapping MEM2E = {"memory5", "address2", "Memory value 2E"};
    static const EepromConfigMapping MEM3E = {"memory5", "address3", "Memory value 3E"};
    static const EepromConfigMapping MEM4E = {"memory5", "address4", "Memory value 4E"};
    static const EepromConfigMapping MEM5E = {"memory5", "address5", "Memory value 5E"};
    static const EepromConfigMapping MEM6E = {"memory5", "address6", "Memory value 6E"};
    
    // Calibration Parameters (matches savedState.json "calibration" section)
    static const EepromConfigMapping CAL_OFFSET = {"calibration", "offset", "Calibration offset"};
    static const EepromConfigMapping CAL_LIMIT = {"calibration", "limit", "Calibration limit"};
    static const EepromConfigMapping CAL_STEP = {"calibration", "stepsize", "Calibration step"};
    static const EepromConfigMapping CAL_SCALE = {"calibration", "scalefactor", "Calibration scale"};
    static const EepromConfigMapping CAL_DIRECTION = {"calibration", "direction", "Calibration direction"};
    static const EepromConfigMapping CAL_PERIOD = {"calibration", "period", "Calibration period"};
    
    // System State (matches savedState.json "flags" section)
    static const EepromConfigMapping SYSTEM_RESTART = {"flags", "rebootflag", "Restart flag"};
    static const EepromConfigMapping SYSTEM_STRESS_TEST = {"testmode", "stress_test", "Stress test flag"};
}

// Function to get config mapping for EEPROM address enum
inline const EepromConfigMapping* getEepromConfigMappingByEnum(eePromAddress_t eepromAddress) {
    switch (eepromAddress) {
        // Display Configuration
        case eeDISPMODE:    return &EepromMappings::DISPLAY_MODE;
        case eeDISPBRIGHT:  return &EepromMappings::DISPLAY_BRIGHTNESS;
        case eeDISPSLEEP:   return &EepromMappings::DISPLAY_SLEEP;
        case eeDUMMY:       return &EepromMappings::DISPLAY_DUMMY;
        
        // Device Configuration
        case eeOTAMODE:     return &EepromMappings::DEVICE_OTA_MODE;
        case eeROLEIS:      return &EepromMappings::DEVICE_ROLE;
        case eeBUTTONMENU:  return &EepromMappings::DEVICE_BUTTON_MENU;
        case eeFULLSCREEN:  return &EepromMappings::DEVICE_FULLSCREEN;
        case eeMACKEY:      return &EepromMappings::DEVICE_MAC_KEY;
        case eeMACADDR:     return &EepromMappings::DEVICE_MAC_ADDR;
        case eeINPUTDIR:    return &EepromMappings::DEVICE_INPUT_DIR;
        
        // Motor Control State
        case eeDEVICEON:    return &EepromMappings::MOTOR_DEVICE_ON;
        case eeIDLE:        return &EepromMappings::MOTOR_IDLE;
        case eeLAST:        return &EepromMappings::MOTOR_LAST;
        case eeMAX:         return &EepromMappings::MOTOR_MAX;
        
        // Auto Speed Values
        case eeAUTO1:       return &EepromMappings::AUTO1;
        case eeAUTO2:       return &EepromMappings::AUTO2;
        case eeAUTO3:       return &EepromMappings::AUTO3;
        case eeAUTO4:       return &EepromMappings::AUTO4;
        case eeAUTO5:       return &EepromMappings::AUTO5;
        case eeAUTO6:       return &EepromMappings::AUTO6;
        
        // Memory Values A
        case eeMEM1A:       return &EepromMappings::MEM1A;
        case eeMEM2A:       return &EepromMappings::MEM2A;
        case eeMEM3A:       return &EepromMappings::MEM3A;
        case eeMEM4A:       return &EepromMappings::MEM4A;
        case eeMEM5A:       return &EepromMappings::MEM5A;
        case eeMEM6A:       return &EepromMappings::MEM6A;
        
        // Memory Values B
        case eeMEM1B:       return &EepromMappings::MEM1B;
        case eeMEM2B:       return &EepromMappings::MEM2B;
        case eeMEM3B:       return &EepromMappings::MEM3B;
        case eeMEM4B:       return &EepromMappings::MEM4B;
        case eeMEM5B:       return &EepromMappings::MEM5B;
        case eeMEM6B:       return &EepromMappings::MEM6B;
        
        // Memory Values C
        case eeMEM1C:       return &EepromMappings::MEM1C;
        case eeMEM2C:       return &EepromMappings::MEM2C;
        case eeMEM3C:       return &EepromMappings::MEM3C;
        case eeMEM4C:       return &EepromMappings::MEM4C;
        case eeMEM5C:       return &EepromMappings::MEM5C;
        case eeMEM6C:       return &EepromMappings::MEM6C;
        
        // Memory Values D
        case eeMEM1D:       return &EepromMappings::MEM1D;
        case eeMEM2D:       return &EepromMappings::MEM2D;
        case eeMEM3D:       return &EepromMappings::MEM3D;
        case eeMEM4D:       return &EepromMappings::MEM4D;
        case eeMEM5D:       return &EepromMappings::MEM5D;
        case eeMEM6D:       return &EepromMappings::MEM6D;
        
        // Memory Values E
        case eeMEM1E:       return &EepromMappings::MEM1E;
        case eeMEM2E:       return &EepromMappings::MEM2E;
        case eeMEM3E:       return &EepromMappings::MEM3E;
        case eeMEM4E:       return &EepromMappings::MEM4E;
        case eeMEM5E:       return &EepromMappings::MEM5E;
        case eeMEM6E:       return &EepromMappings::MEM6E;
        
        // Calibration Parameters
        case eeCALOFFSET:   return &EepromMappings::CAL_OFFSET;
        case eeCALLIMIT:    return &EepromMappings::CAL_LIMIT;
        case eeCALSTEP:     return &EepromMappings::CAL_STEP;
        case eeCALSCALE:    return &EepromMappings::CAL_SCALE;
        case eeCALDIR:      return &EepromMappings::CAL_DIRECTION;
        case eeCALPERIOD:   return &EepromMappings::CAL_PERIOD;
        
        // System State
        case eeRESTART:     return &EepromMappings::SYSTEM_RESTART;
        case eeSTRESSTEST:  return &EepromMappings::SYSTEM_STRESS_TEST;
        
        default:
            return nullptr;
    }
}

// Function to get config mapping for EEPROM address (integer index)
inline const EepromConfigMapping* getEepromConfigMapping(int eepromAddress) {
    if (eepromAddress >= 0 && eepromAddress < EEPROMRECORDS) {
        return getEepromConfigMappingByEnum(static_cast<eePromAddress_t>(eepromAddress));
    }
    return nullptr;
}

// Function to get section/key for EEPROM address (integer index)
inline bool getEepromConfigKeys(int eepromAddress, const char** section, const char** key) {
    const EepromConfigMapping* mapping = getEepromConfigMapping(eepromAddress);
    if (mapping != nullptr) {
        *section = mapping->section;
        *key = mapping->key;
        return true;
    }
    return false;
}

// Function to get section/key for EEPROM address (enum)
inline bool getEepromConfigKeys(eePromAddress_t eepromAddress, const char** section, const char** key) {
    const EepromConfigMapping* mapping = getEepromConfigMappingByEnum(eepromAddress);
    if (mapping != nullptr) {
        *section = mapping->section;
        *key = mapping->key;
        return true;
    }
    return false;
}
