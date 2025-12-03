// single unified Address to Config Section/Key Mapping
// Maps single unified addresses to JSON config section/key pairs for device
// state
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Arduino.h>
#include <string.h>
#include <logger.hpp>
//--------------------------------------------------------------------
// Define User EEPROM Space. Serialize Addresses
//--------------------------------------------------------------------
// Include EEPROM address definitions for X-Macro pattern
#include "eepromAddressDefinitions.h"

// Generate enum from single source of truth
GENERATE_EEPROM_ENUM()

// Compile-time validation that EEPROMRECORDS matches address count
EEPROM_COUNT_CHECK()

struct stringTriple {
  const char *section;
  const char *key;
  const char *name;
};

// Generate addressMap array from single source of truth - automatically stays in sync with enum
GENERATE_EEPROM_ADDRESS_MAP()

//inline void printMapping(const stringTriple *mapping) {
inline void printMapping() {
  LOG_INFO(LOG_CAT_CONFIG, "addressName\tSection\tKey");
  // Loop through all addresses in addressMap (excluding EEPROMRECORDS which is the count)
  for (eePromAddress_t addr = eeDUMMY; addr < EEPROMRECORDS; addr = static_cast<eePromAddress_t>(addr + 1)) {
    LOG_INFO(LOG_CAT_CONFIG, "%-10s\t%-10s\t%-10s\n", 
                  addressMap[addr].name, 
                  addressMap[addr].section, 
                  addressMap[addr].key);
  }
  LOG_INFO(LOG_CAT_CONFIG, "--------------------------------");
}

inline stringTriple getEEPROMAddressTriple(eePromAddress_t eepromAddress) {
  return addressMap[eepromAddress];
}

// Function to get section/key for EEPROM address (enum)
inline bool getEEPROMAddressSection(eePromAddress_t eepromAddress) {
  return addressMap[eepromAddress].section;
}

inline bool getEEPROMAddressKey(eePromAddress_t eepromAddress) {
  return addressMap[eepromAddress].key;
}
