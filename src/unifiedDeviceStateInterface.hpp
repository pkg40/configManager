#pragma once

#include <Arduino.h>
#include <string>
#include "eepromAddressMapping.hpp"
#include "configMigrationWrapper.hpp"
#include "../../motorController/motorControllerEEPROM_Addresses.hpp"

/**
 * Unified Device State Interface
 * 
 * This interface provides a consistent API for accessing device state data
 * regardless of whether the underlying storage is EEPROM or JSON configuration.
 * 
 * Usage:
 *   String value = deviceStateGet(eeIDLE);
 *   bool success = deviceStatePut(eeMAX, "150");
 */

// Forward declarations
class MoCoEEProm;
struct eePromRec;

// Global function declarations
String deviceStateGet(eePromAddress_t address);
bool deviceStatePut(eePromAddress_t address, const String& value);
bool deviceStatePut(eePromAddress_t address, int value);

// Internal helper functions
String deviceStateGetFromEeprom(eePromAddress_t address);
String deviceStateGetFromConfig(eePromAddress_t address);
bool deviceStatePutToEeprom(eePromAddress_t address, const String& value);
bool deviceStatePutToConfig(eePromAddress_t address, const String& value);

// Configuration flags
extern bool _useConfigManager;
extern MoCoEEProm* _eepromInstance;

/**
 * Initialize the unified device state interface
 * @param useConfigManager true to use iConfigManager, false to use EEPROM
 * @param eepromInstance pointer to EEPROM instance (can be null if using config)
 */
void initUnifiedDeviceStateInterface(bool useConfigManager = true, MoCoEEProm* eepromInstance = nullptr);

/**
 * Get device state value by EEPROM address enum
 * @param address EEPROM address enum (e.g., eeIDLE, eeMAX, eeLAST)
 * @return String value, empty if not found or error
 */
String deviceStateGet(eePromAddress_t address);

/**
 * Put device state value by EEPROM address enum
 * @param address EEPROM address enum (e.g., eeIDLE, eeMAX, eeLAST)
 * @param value String value to store
 * @return true if successful, false otherwise
 */
bool deviceStatePut(eePromAddress_t address, const String& value);

/**
 * Put device state value by EEPROM address enum (integer overload)
 * @param address EEPROM address enum (e.g., eeIDLE, eeMAX, eeLAST)
 * @param value Integer value to store
 * @return true if successful, false otherwise
 */
bool deviceStatePut(eePromAddress_t address, int value);

// Internal helper function declarations
String deviceStateGetFromEeprom(eePromAddress_t address);
String deviceStateGetFromConfig(eePromAddress_t address);
bool deviceStatePutToEeprom(eePromAddress_t address, const String& value);
bool deviceStatePutToConfig(eePromAddress_t address, const String& value);
