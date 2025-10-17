#include "unifiedDeviceStateInterface.hpp"

// Global configuration variables
bool _useConfigManager = true;
MoCoEEProm* _eepromInstance = nullptr;

/**
 * Get device state value by EEPROM address enum
 */
String deviceStateGet(eePromAddress_t address) {
    Serial.printf("deviceStateGet: address=%d, useConfigManager=%d\n", address, _useConfigManager);
    if (_useConfigManager) {
        return deviceStateGetFromConfig(address);
    } else {
        return deviceStateGetFromEeprom(address);
    }
}

/**
 * Put device state value by EEPROM address enum
 */
bool deviceStatePut(eePromAddress_t address, const String& value) {
    if (_useConfigManager) {
        return deviceStatePutToConfig(address, value);
    } else {
        return deviceStatePutToEeprom(address, value);
    }
}

/**
 * Put device state value by EEPROM address enum (integer overload)
 */
bool deviceStatePut(eePromAddress_t address, int value) {
    return deviceStatePut(address, String(value));
}

/**
 * Get device state value from EEPROM
 */
String deviceStateGetFromEeprom(eePromAddress_t address) {
    if (!_eepromInstance) {
        Serial.printf("deviceStateGetFromEeprom: EEPROM instance not initialized\n");
        return "";
    }
    
    struct eePromRec record;
    int eepromAddress = EESIZE * address;
    
    Serial.printf("deviceStateGetFromEeprom: Reading EEPROM address %d (enum %d)\n", eepromAddress, address);
    _eepromInstance->getUser(eepromAddress, record, false);
    Serial.printf("deviceStateGetFromEeprom: Got value=%d, valid=0x%x\n", record.value, record.valid);
    
    if (record.valid == EEVALID) {
        return String(record.value);
    }
    
    Serial.printf("deviceStateGetFromEeprom: Failed to read address %d (enum %d)\n", eepromAddress, address);
    return "";
}

/**
 * Get device state value from config system
 */
String deviceStateGetFromConfig(eePromAddress_t address) {
    const EepromConfigMapping* mapping = getEepromConfigMappingByEnum(address);
    if (!mapping) {
        Serial.printf("deviceStateGetFromConfig: No mapping for address %d\n", address);
        return "";
    }
    
    Serial.printf("deviceStateGetFromConfig: Looking for %s.%s\n", mapping->section, mapping->key);
    
    // Use DEVICE_STATE_GET macro instead of GETM_ee to avoid macro conflicts
    String value = DEVICE_STATE_GET(mapping->section, mapping->key);
    Serial.printf("deviceStateGetFromConfig: Got value='%s' (length=%d)\n", value.c_str(), value.length());
    
    if (value.length() == 0) {
        Serial.printf("deviceStateGetFromConfig: No value found for %s.%s\n", mapping->section, mapping->key);
    }
    
    return value;
}

/**
 * Put device state value to EEPROM
 */
bool deviceStatePutToEeprom(eePromAddress_t address, const String& value) {
    if (!_eepromInstance) {
        Serial.printf("deviceStatePutToEeprom: EEPROM instance not initialized\n");
        return false;
    }
    
    struct eePromRec record;
    record.value = value.toInt();
    record.valid = EEVALID;
    
    int eepromAddress = EESIZE * address;
    _eepromInstance->putUser(eepromAddress, record, false);
    
    Serial.printf("deviceStatePutToEeprom: Wrote %d to address %d (enum %d)\n", record.value, eepromAddress, address);
    return true; // EEPROM putUser doesn't return a value, assume success
}

/**
 * Put device state value to config system
 */
bool deviceStatePutToConfig(eePromAddress_t address, const String& value) {
    const EepromConfigMapping* mapping = getEepromConfigMappingByEnum(address);
    if (!mapping) {
        Serial.printf("deviceStatePutToConfig: No mapping for address %d\n", address);
        return false;
    }
    
    // Use DEVICE_STATE_SET macro instead of PUTM_ee to avoid macro conflicts
    DEVICE_STATE_SET(mapping->section, mapping->key, value);
    bool success = DEVICE_STATE_SAVE();
    
    if (success) {
        Serial.printf("deviceStatePutToConfig: Wrote %s to %s.%s\n", value.c_str(), mapping->section, mapping->key);
    } else {
        Serial.printf("deviceStatePutToConfig: Failed to write %s to %s.%s\n", value.c_str(), mapping->section, mapping->key);
    }
    
    return success;
}

/**
 * Initialize the unified device state interface
 */
void initUnifiedDeviceStateInterface(bool useConfigManager, MoCoEEProm* eepromInstance) {
    _useConfigManager = useConfigManager;
    _eepromInstance = eepromInstance;
    
    if (useConfigManager) {
        Serial.println("UnifiedDeviceState: Using iConfigManager backend");
        // Ensure config system is initialized
        initDeviceStateConfig();
    } else {
        Serial.println("UnifiedDeviceState: Using EEPROM backend");
        if (!eepromInstance) {
            Serial.println("UnifiedDeviceState: WARNING - EEPROM instance is null!");
        }
    }
}
