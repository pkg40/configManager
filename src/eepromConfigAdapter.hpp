#pragma once

#include <Arduino.h>
#include "configManager.hpp"
#include "platformFileSystemProvider.hpp"

/**
 * EEPROM Config Adapter
 * 
 * Provides an adapter interface for EEPROM-based configuration
 * that can work with the configManager system. This allows
 * legacy EEPROM code to work alongside the new JSON-based
 * configuration system.
 */

class eepromConfigAdapter {
private:
    configManager* _configManager_p;
    bool _initialized;

public:
    eepromConfigAdapter() : _configManager_p(nullptr), _initialized(false) {}
    
    /**
     * Constructor with existing configManager
     * @param configManager existing configManager instance
     */
    eepromConfigAdapter(configManager* configManager) : _configManager_p(configManager), _initialized(true) {}
    
    /**
     * Initialize the EEPROM config adapter
     * @param configPath path to the config file (e.g., "/eepromConfig.json")
     * @return true if successful, false otherwise
     */
    bool init(const char* configPath = "/eepromConfig.json") {
        if (_initialized) {
            return true;
        }
        
        platformFileSystemProvider* fsProvider = new platformFileSystemProvider();
        _configManager_p = new configManager(fsProvider, configPath, 4096);
        
        if (_configManager_p->loadConfig()) {
            _initialized = true;
            Serial.printf("eepromConfigAdapter: Initialized with config file %s\n", configPath);
            return true;
        } else {
            Serial.printf("eepromConfigAdapter: Failed to load config file %s\n", configPath);
            return false;
        }
    }
    
    /**
     * Get a value from EEPROM config
     * @param address EEPROM address
     * @return String value, empty if not found
     */
    String getValue(int address) {
        if (!_initialized) {
            Serial.printf("eepromConfigAdapter: Not initialized for address %d\n", address);
            return "";
        }
        
        return _configManager_p->getValue("eeprom", String(address));
    }
    
    /**
     * Set a value in EEPROM config
     * @param address EEPROM address
     * @param value value to set
     * @return true if successful, false otherwise
     */
    bool setValue(int address, const String& value) {
        if (!_initialized) {
            Serial.printf("eepromConfigAdapter: Not initialized for address %d\n", address);
            return false;
        }
        
        _configManager_p->setValue("eeprom", String(address), value);
        return _configManager_p->saveConfig();
    }
    
    /**
     * Set a value in EEPROM config (integer overload)
     * @param address EEPROM address
     * @param value integer value to set
     * @return true if successful, false otherwise
     */
    bool setValue(int address, int value) {
        return setValue(address, String(value));
    }
    
    /**
     * Check if the adapter is initialized
     * @return true if initialized, false otherwise
     */
    bool isInitialized() const {
        return _initialized;
    }
    
    /**
     * Save the current configuration
     * @return true if successful, false otherwise
     */
    bool save() {
        if (!_initialized) {
            Serial.println("eepromConfigAdapter: Not initialized, cannot save");
            return false;
        }
        
        return _configManager_p->saveConfig();
    }
    
    /**
     * Print the current configuration to Serial
     */
    void printConfig() {
        if (!_initialized) {
            Serial.println("eepromConfigAdapter: Not initialized, cannot print config");
            return;
        }
        
        _configManager_p->printConfigToSerial();
    }
};

// Global instance for backward compatibility
extern eepromConfigAdapter eepromConfig;
