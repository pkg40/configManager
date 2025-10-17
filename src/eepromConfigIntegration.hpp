// EEPROM Config Integration
// Integration layer to enable EEPROM config overrides
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Arduino.h>
#include "eepromConfigOverrides.hpp"
#include "configMigrationWrapper.hpp"

// Integration function to enable EEPROM config overrides
void enableEepromConfigOverrides() {
    Serial.println("[EEPROM_INTEGRATION] Enabling EEPROM config overrides...");
    
    // Initialize the device state config if not already done
    if (!_deviceStateManager_p) {
        initDeviceStateConfig();
    }
    
    // Initialize the EEPROM overrides
    EepromConfigOverrides::init(_deviceStateManager_p);
    
    if (EepromConfigOverrides::isInitialized()) {
        Serial.println("[EEPROM_INTEGRATION] EEPROM config overrides enabled successfully");
    } else {
        Serial.println("[EEPROM_INTEGRATION] Failed to enable EEPROM config overrides");
    }
}

// Test function to verify EEPROM config integration
void testEepromConfigIntegration() {
    Serial.println("\n=== EEPROM Config Integration Test ===");
    
    if (!EepromConfigOverrides::isInitialized()) {
        Serial.println("❌ EEPROM config overrides not initialized");
        return;
    }
    
    // Test reading and writing a few key values
    struct TestValue {
        int address;
        const char* name;
        int testValue;
    };
    
    TestValue testValues[] = {
        {0, "Display Mode", 1},
        {5, "Device Role", 2},
        {11, "Device On", 1},
        {12, "Idle Speed", 50},
        {15, "Auto Speed 1", 100}
    };
    
    int testCount = sizeof(testValues) / sizeof(testValues[0]);
    int passCount = 0;
    
    for (int i = 0; i < testCount; i++) {
        // Test write
        eePromRec writeRecord;
        writeRecord.valid = 0xAA; // EEVALID
        writeRecord.value = testValues[i].testValue;
        
        bool writeSuccess = EepromConfigOverrides::putUser(testValues[i].address, writeRecord, true);
        
        // Test read
        eePromRec readRecord;
        bool readSuccess = EepromConfigOverrides::getUser(testValues[i].address, readRecord, false);
        
        if (writeSuccess && readSuccess && readRecord.value == testValues[i].testValue) {
            Serial.printf("✅ PASS: %s (address %d) = %d\n", 
                         testValues[i].name, testValues[i].address, readRecord.value);
            passCount++;
        } else {
            Serial.printf("❌ FAIL: %s (address %d) - Write: %s, Read: %s, Value: %d\n", 
                         testValues[i].name, testValues[i].address,
                         writeSuccess ? "OK" : "FAIL",
                         readSuccess ? "OK" : "FAIL",
                         readRecord.value);
        }
    }
    
    Serial.printf("\n=== Integration Test Results ===\n");
    Serial.printf("Passed: %d/%d\n", passCount, testCount);
    
    if (passCount == testCount) {
        Serial.println("✅ All integration tests passed!");
    } else {
        Serial.println("❌ Some integration tests failed.");
    }
}
