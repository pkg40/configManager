// EEPROM Config Overrides
// Provides EEPROM-compatible interface that routes to JSON config system
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Arduino.h>
#include "eepromAddressMapping.hpp"
#include "configMigrationWrapper.hpp"
#include <myEEProm.hpp>  // For eePromRec, eePromLabel, MoCoEEProm

// EEPROM Override Class
class EepromConfigOverrides {
private:
    static bool _initialized;
    static configManager* _deviceStateManager_p;
    
public:
    // Initialize the EEPROM config overrides
    static void init(configManager* deviceStateManager) {
        _deviceStateManager_p = deviceStateManager;
        _initialized = true;
    }
    
    // Check if overrides are initialized
    static bool isInitialized() {
        return _initialized && _deviceStateManager_p != nullptr;
    }
    
    // EEPROM-compatible read function
    static bool getUser(int address, eePromRec& record, bool verbose = false) {
        if (!isInitialized()) {
            if (verbose) {
                Serial.printf("[EEPROM_OVERRIDE] Device state manager not initialized for address %d\n", address);
            }
            record.valid = 0;
            record.value = 0;
            return false;
        }
        
        const char* section;
        const char* key;
        if (!getEepromConfigKeys(address, &section, &key)) {
            if (verbose) {
                Serial.printf("[EEPROM_OVERRIDE] No mapping found for address %d\n", address);
            }
            record.valid = 0;
            record.value = 0;
            return false;
        }
        
        String value = _deviceStateManager_p->getValue(section, key);
        if (value.length() > 0) {
            record.valid = 0xAA; // EEVALID equivalent
            record.value = value.toInt();
            if (verbose) {
                Serial.printf("[EEPROM_OVERRIDE] Read address %d -> %s.%s = %d\n", address, section, key, record.value);
            }
            return true;
        } else {
            record.valid = 0;
            record.value = 0;
            if (verbose) {
                Serial.printf("[EEPROM_OVERRIDE] No value found for address %d -> %s.%s\n", address, section, key);
            }
            return false;
        }
    }
    
    // EEPROM-compatible write function
    static bool putUser(int address, const eePromRec& record, bool commit = false) {
        if (!isInitialized()) {
            Serial.printf("[EEPROM_OVERRIDE] Device state manager not initialized for address %d\n", address);
            return false;
        }
        
        const char* section;
        const char* key;
        if (!getEepromConfigKeys(address, &section, &key)) {
            Serial.printf("[EEPROM_OVERRIDE] No mapping found for address %d\n", address);
            return false;
        }
        
        if (record.valid == 0xAA) { // EEVALID equivalent
            _deviceStateManager_p->setValue(section, key, String(record.value));
            if (commit) {
                _deviceStateManager_p->saveConfig();
            }
            Serial.printf("[EEPROM_OVERRIDE] Write address %d -> %s.%s = %d\n", address, section, key, record.value);
            return true;
        } else {
            Serial.printf("[EEPROM_OVERRIDE] Invalid record for address %d (valid=0x%02X)\n", address, record.valid);
            return false;
        }
    }
    
    // EEPROM-compatible reserve read function
    static bool getReserve(int address, eePromRec& record, bool verbose = false) {
        // For now, treat reserve as regular user data
        return getUser(address, record, verbose);
    }
    
    // EEPROM-compatible reserve write function
    static bool putReserve(int address, const eePromRec& record, bool commit = false) {
        // For now, treat reserve as regular user data
        return putUser(address, record, commit);
    }
    
    // EEPROM-compatible label read function
    static bool getLabel(int address, eePromLabel& label) {
        if (!isInitialized()) {
            Serial.printf("[EEPROM_OVERRIDE] Device state manager not initialized for label address %d\n", address);
            return false;
        }
        
        // Labels are typically stored in a special section
        String value = _deviceStateManager_p->getValue("labels", String(address));
        if (value.length() > 0) {
            label.valid = 0xAA; // EEVALID equivalent
            strncpy(label.text, value.c_str(), sizeof(label.text) - 1);
            label.text[sizeof(label.text) - 1] = '\0';
            Serial.printf("[EEPROM_OVERRIDE] Read label address %d = %s\n", address, label.text);
            return true;
        } else {
            label.valid = 0;
            label.text[0] = '\0';
            return false;
        }
    }
    
    // EEPROM-compatible label write function
    static bool putLabel(int address, const eePromLabel& label) {
        if (!isInitialized()) {
            Serial.printf("[EEPROM_OVERRIDE] Device state manager not initialized for label address %d\n", address);
            return false;
        }
        
        if (label.valid == 0xAA) { // EEVALID equivalent
            _deviceStateManager_p->setValue("labels", String(address), String(label.text));
            _deviceStateManager_p->saveConfig();
            Serial.printf("[EEPROM_OVERRIDE] Write label address %d = %s\n", address, label.text);
            return true;
        } else {
            Serial.printf("[EEPROM_OVERRIDE] Invalid label for address %d (valid=0x%02X)\n", address, label.valid);
            return false;
        }
    }
    
    // EEPROM-compatible commit function
    static bool Commit() {
        if (!isInitialized()) {
            Serial.printf("[EEPROM_OVERRIDE] Device state manager not initialized for commit\n");
            return false;
        }
        
        bool result = _deviceStateManager_p->saveConfig();
        if (result) {
            Serial.printf("[EEPROM_OVERRIDE] Config committed successfully\n");
        } else {
            Serial.printf("[EEPROM_OVERRIDE] Config commit failed\n");
        }
        return result;
    }
    
    // Additional EEPROM-compatible functions that might be needed
    static void setupEEPromInit(const int* eeValues, int recordCount) {
        if (!isInitialized()) {
            Serial.printf("[EEPROM_OVERRIDE] Device state manager not initialized for setup\n");
            return;
        }
        
        Serial.printf("[EEPROM_OVERRIDE] Setting up %d EEPROM records with default values\n", recordCount);
        
        // Initialize all addresses with default values
        for (int i = 0; i < recordCount; i++) {
            const char* section;
            const char* key;
            if (getEepromConfigKeys(i, &section, &key)) {
                String currentValue = _deviceStateManager_p->getValue(section, key);
                if (currentValue.length() == 0) {
                    // No existing value, set default
                    _deviceStateManager_p->setValue(section, key, String(eeValues[i]));
                    Serial.printf("[EEPROM_OVERRIDE] Set default %s.%s = %d\n", section, key, eeValues[i]);
                }
            }
        }
        
        // Save the configuration
        _deviceStateManager_p->saveConfig();
    }
    
    static void setupEEPromReadCommon() {
        Serial.printf("[EEPROM_OVERRIDE] EEPROM read common setup completed\n");
    }
    
    static void setupEEPromRead(void* device) {
        Serial.printf("[EEPROM_OVERRIDE] EEPROM read device setup completed\n");
    }
    
    // Boot count and try count functions (if needed)
    static int getCommitCount(bool verbose = false) {
        if (!isInitialized()) return 0;
        
        String value = _deviceStateManager_p->getValue("system", "commit_count");
        int count = value.length() > 0 ? value.toInt() : 0;
        if (verbose) {
            Serial.printf("[EEPROM_OVERRIDE] Commit count = %d\n", count);
        }
        return count;
    }
    
    static int getTryCount(bool verbose = false) {
        if (!isInitialized()) return 0;
        
        String value = _deviceStateManager_p->getValue("system", "try_count");
        int count = value.length() > 0 ? value.toInt() : 0;
        if (verbose) {
            Serial.printf("[EEPROM_OVERRIDE] Try count = %d\n", count);
        }
        return count;
    }
    
    static void putTryCount(int count) {
        if (!isInitialized()) return;
        
        _deviceStateManager_p->setValue("system", "try_count", String(count));
        Serial.printf("[EEPROM_OVERRIDE] Set try count = %d\n", count);
    }
    
    static void resetTryCount() {
        putTryCount(0);
    }
    
    static void resetBootCount() {
        if (!isInitialized()) return;
        
        _deviceStateManager_p->setValue("system", "boot_count", "0");
        Serial.printf("[EEPROM_OVERRIDE] Reset boot count\n");
    }
    
    static void incrementBootCount(bool verbose = false) {
        if (!isInitialized()) return;
        
        String value = _deviceStateManager_p->getValue("system", "boot_count");
        int count = value.length() > 0 ? value.toInt() : 0;
        count++;
        _deviceStateManager_p->setValue("system", "boot_count", String(count));
        if (verbose) {
            Serial.printf("[EEPROM_OVERRIDE] Incremented boot count to %d\n", count);
        }
    }
};

// Static member definitions
bool EepromConfigOverrides::_initialized = false;
configManager* EepromConfigOverrides::_deviceStateManager_p = nullptr;
