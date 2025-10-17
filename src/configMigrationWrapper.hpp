#pragma once

#include <Arduino.h>
#include "configManager.hpp"
#include "platformFileSystemProvider.hpp"

/**
 * Configuration Migration Wrapper
 * 
 * Provides backward compatibility macros for migrating from mapSettings
 * to the new configManager system. This allows existing code to work
 * with minimal changes during the migration process.
 */

// Global config manager instances
static configManager* _wifiConfigManager_p = nullptr;
static configManager* _deviceStateManager_p = nullptr;
static bool _wifiConfigInitialized = false;
static bool _deviceStateInitialized = false;

// WiFi Configuration Functions
static void initWifiConfig() {
    if (!_wifiConfigInitialized) {
        Serial.println("initWifiConfig: Starting initialization...");
        platformFileSystemProvider* fsProvider = new platformFileSystemProvider();
        _wifiConfigManager_p = new configManager(fsProvider, "/wifiConfig.json", 4096);
        Serial.println("initWifiConfig: Config manager created");
        bool loadResult = _wifiConfigManager_p->loadConfig();
        Serial.printf("initWifiConfig: Load result = %d\n", loadResult);
        _wifiConfigInitialized = true;
        Serial.printf("initWifiConfig: Initialized, manager = %p\n", _wifiConfigManager_p);
    } else {
        Serial.println("initWifiConfig: Already initialized");
    }
}

static void initDeviceStateConfig() {
    if (!_deviceStateInitialized) {
        Serial.println("initDeviceStateConfig: Starting initialization...");
        platformFileSystemProvider* fsProvider = new platformFileSystemProvider();
        _deviceStateManager_p = new configManager(fsProvider, "/savedState.json", 4096);
        Serial.println("initDeviceStateConfig: Config manager created");
        bool loadResult = _deviceStateManager_p->loadConfig();
        Serial.printf("initDeviceStateConfig: Load result = %d\n", loadResult);
        _deviceStateInitialized = true;
        Serial.printf("initDeviceStateConfig: Initialized, manager = %p\n", _deviceStateManager_p);
    } else {
        Serial.println("initDeviceStateConfig: Already initialized");
    }
}

// WiFi Configuration Macros
#define INIT_WIFI_CONFIG() initWifiConfig()
#define GETM_w(section, key) (_wifiConfigManager_p ? _wifiConfigManager_p->getValue(section, key) : (Serial.printf("GETM_w: WiFi config manager not initialized for %s.%s\n", section, key), String("")))
#define PUTM_w(section, key, value) do { if (_wifiConfigManager_p) _wifiConfigManager_p->setValue(section, key, value); else Serial.printf("PUTM_w: WiFi config manager not initialized for %s.%s = %s\n", section, key, value); } while(0)
#define SAVEM_w() (_wifiConfigManager_p ? _wifiConfigManager_p->saveConfig() : (Serial.println("SAVEM_w: WiFi config manager not initialized"), false))

// Device State Configuration Macros
#define INIT_DEVICE_STATE_CONFIG() initDeviceStateConfig()
#define DEVICE_STATE_GET(section, key) (_deviceStateManager_p ? _deviceStateManager_p->getValue(section, key) : (Serial.printf("DEVICE_STATE_GET: Device state manager not initialized for %s.%s\n", section, key), String("")))
#define DEVICE_STATE_SET(section, key, value) do { if (_deviceStateManager_p) _deviceStateManager_p->setValue(section, key, value); else Serial.printf("DEVICE_STATE_SET: Device state manager not initialized for %s.%s = %s\n", section, key, value); } while(0)
#define DEVICE_STATE_SAVE() (_deviceStateManager_p ? _deviceStateManager_p->saveConfig() : (Serial.println("DEVICE_STATE_SAVE: Device state manager not initialized"), false))

// EEPROM Migration Macros (for backward compatibility)
#define GETM_ee(address) (_deviceStateManager_p ? _deviceStateManager_p->getValue("device", String(address)) : (Serial.printf("GETM_ee: Device state manager not initialized for address %d\n", address), String("")))
#define PUTM_ee(address, value) do { if (_deviceStateManager_p) _deviceStateManager_p->setValue("device", String(address), String(value)); else Serial.printf("PUTM_ee: Device state manager not initialized for address %d = %s\n", address, value); } while(0)
