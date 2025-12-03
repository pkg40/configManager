#include "flashWearCounter.hpp"
#include <logger.hpp>

// Platform-specific EEPROM includes
#if defined(ESP32)
    #include <EEPROM.h>
#elif defined(ESP8266)
    #include <EEPROM.h>
#else
    #error "Flash wear counter only supports ESP32 and ESP8266"
#endif

#include <logger.hpp>

// Static variables for flash wear tracking
static uint32_t _maxFlashWrites = DEFAULT_MAX_FLASH_WRITES;
static uint16_t _counterAddress = DEFAULT_FLASH_WEAR_COUNTER_ADDRESS;
static uint16_t _bootCounterAddress = DEFAULT_BOOT_COUNTER_ADDRESS;
static uint32_t _flashWriteCount = 0;
static uint32_t _bootCount = 0;
static bool _flashWearInitialized = false;
static bool _bootCounterInitialized = false;
static unsigned long _lastReportTime = 0;
static unsigned long _reportInterval = DEFAULT_REPORT_INTERVAL_MS;
static bool _deviceRetired = false;
static size_t _eepromLength = 0;

// Warning thresholds
static float _cautionThreshold = CAUTION_THRESHOLD;
static float _criticalThreshold = CRITICAL_THRESHOLD;
static float _retirementThreshold = RETIREMENT_THRESHOLD;

// Private helper functions
static void _initializeEEPROM();
static bool _readFlashWearRecord(uint16_t address, FlashWearRecord* record);
static bool _writeFlashWearRecord(uint16_t address, const FlashWearRecord* record, const char* label);

/**
 * Initialize the flash wear counter system
 */
bool initFlashWearCounter(uint32_t maxWrites, uint16_t counterAddress, uint16_t bootCounterAddress) {
    if (_flashWearInitialized) {
        LOG_DEBUG(LOG_CAT_SYSTEM,  "FlashWearCounter: Already initialized");
        return true;
    }
    
    _maxFlashWrites = maxWrites;
    _counterAddress = counterAddress;
    _bootCounterAddress = bootCounterAddress;
    
    // Initialize EEPROM
    _initializeEEPROM();
    _eepromLength = EEPROM.length();

    if (_eepromLength < FLASH_WEAR_RECORD_SIZE * 2) {
        LOG_ERROR(LOG_CAT_SYSTEM, "FlashWearCounter: ERROR - EEPROM length too small for wear and boot records");
        return false;
    }

    if (_counterAddress > _eepromLength - FLASH_WEAR_RECORD_SIZE) {
        uint16_t adjustedAddress = static_cast<uint16_t>(_eepromLength - FLASH_WEAR_RECORD_SIZE);
        LOG_WARN(LOG_CAT_SYSTEM, "FlashWearCounter: WARNING - Counter address %u out of range for EEPROM length %u. Adjusting to %u",
                      static_cast<unsigned>(_counterAddress), static_cast<unsigned>(_eepromLength), static_cast<unsigned>(adjustedAddress));
        _counterAddress = adjustedAddress;
    }

    if (_bootCounterAddress > _eepromLength - FLASH_WEAR_RECORD_SIZE) {
        uint16_t fallback = (_counterAddress >= FLASH_WEAR_RECORD_SIZE)
            ? static_cast<uint16_t>(_counterAddress - FLASH_WEAR_RECORD_SIZE)
            : static_cast<uint16_t>(_eepromLength - FLASH_WEAR_RECORD_SIZE);
        LOG_WARN(LOG_CAT_SYSTEM, "BootCounter: WARNING - Boot counter address %u out of range for EEPROM length %u. Adjusting to %u",
                      static_cast<unsigned>(_bootCounterAddress), static_cast<unsigned>(_eepromLength), static_cast<unsigned>(fallback));
        _bootCounterAddress = fallback;
    }

    auto recordsOverlap = [](uint16_t a, uint16_t b) {
        const uint16_t aEnd = static_cast<uint16_t>(a + FLASH_WEAR_RECORD_SIZE);
        const uint16_t bEnd = static_cast<uint16_t>(b + FLASH_WEAR_RECORD_SIZE);
        return (a <= b && aEnd > b) || (b <= a && bEnd > a);
    };

    if (recordsOverlap(_counterAddress, _bootCounterAddress)) {
        uint16_t adjusted = (_counterAddress >= FLASH_WEAR_RECORD_SIZE)
            ? static_cast<uint16_t>(_counterAddress - FLASH_WEAR_RECORD_SIZE)
            : static_cast<uint16_t>(_counterAddress + FLASH_WEAR_RECORD_SIZE);

        if (adjusted > _eepromLength - FLASH_WEAR_RECORD_SIZE) {
            adjusted = 0;
        }

        LOG_WARN(LOG_CAT_SYSTEM, "BootCounter: WARNING - Address conflict between boot (%u) and wear (%u). Adjusting boot to %u\n",
            static_cast<unsigned>(_bootCounterAddress), static_cast<unsigned>(_counterAddress), static_cast<unsigned>(adjusted));
        _bootCounterAddress = adjusted;
    }

    if (_bootCounterAddress > _eepromLength - FLASH_WEAR_RECORD_SIZE) {
        LOG_ERROR(LOG_CAT_SYSTEM, "BootCounter: ERROR - Unable to locate space for boot counter record");
        return false;
    }

    // Initialize boot counter and increment for this boot
    FlashWearRecord bootRecord{};
    (void)_readFlashWearRecord(_bootCounterAddress, &bootRecord);

    LOG_DEBUG(LOG_CAT_SYSTEM, "BootCounter: DEBUG - Reading from EEPROM byte offset %u, valid=%d, value=%u\n",
        static_cast<unsigned>(_bootCounterAddress), bootRecord.valid, bootRecord.value);

    if (bootRecord.valid == FLASH_WEAR_VALID) {
        if (bootRecord.value <= BOOT_COUNTER_SANITY_LIMIT) {
            _bootCount = bootRecord.value;
        } else {
            LOG_WARN(LOG_CAT_SYSTEM, "BootCounter: Detected out-of-range value %u, resetting to 0", static_cast<unsigned>(bootRecord.value));
            _bootCount = 0;
        }
    } else {
        _bootCount = (bootRecord.value != 0 && bootRecord.value != UINT32_MAX) ? bootRecord.value : 0;
        LOG_INFO(LOG_CAT_SYSTEM, "BootCounter: Initializing boot counter to 0");
    }

    uint32_t nextBootCount = (_bootCount == UINT32_MAX) ? UINT32_MAX : _bootCount + 1;
    FlashWearRecord bootWrite{};
    bootWrite.value = nextBootCount;
    bootWrite.valid = FLASH_WEAR_VALID;
    bootWrite.reserved[0] = 0;
    bootWrite.reserved[1] = 0;
    bootWrite.reserved[2] = 0;

    if (!_writeFlashWearRecord(_bootCounterAddress, &bootWrite, "BootCounter")) {
        LOG_ERROR(LOG_CAT_SYSTEM, "BootCounter: ERROR - Failed to persist boot counter");
        return false;
    }

    _bootCount = nextBootCount;
    _bootCounterInitialized = true;

    LOG_INFO(LOG_CAT_SYSTEM, "BootCounter: Boot count incremented to %u\n", 
        static_cast<unsigned>(_bootCount));
    
    // Read current write count from EEPROM
    FlashWearRecord counterRecord{};
    bool readSuccess = _readFlashWearRecord(_counterAddress, &counterRecord);

    LOG_DEBUG(LOG_CAT_SYSTEM, "FlashWearCounter: DEBUG - Reading from EEPROM byte offset %u, valid=%d, value=%u\n", 
              static_cast<unsigned>(_counterAddress), counterRecord.valid, counterRecord.value);

    if (readSuccess && counterRecord.valid == FLASH_WEAR_VALID) {
        _flashWriteCount = counterRecord.value;
        LOG_INFO(LOG_CAT_SYSTEM, "FlashWearCounter: Loaded existing counter: %u writes (%.2f%% of limit)\n", 
                     _flashWriteCount, (float)_flashWriteCount / _maxFlashWrites * 100.0);
    } else {
        // First boot or corrupted counter - this is NORMAL, not an error
        if (counterRecord.value > 0 && counterRecord.value < _maxFlashWrites) {
            // Corrupted but reasonable value - preserve it
            _flashWriteCount = counterRecord.value;
            LOG_ERROR(LOG_CAT_SYSTEM, "FlashWearCounter: Recovered counter from corrupted EEPROM: %u writes (%.2f%% of limit)\n", 
                _flashWriteCount, (float)_flashWriteCount / _maxFlashWrites * 100.0);
        } else {
            // First boot or completely corrupted - initialize to 0
            _flashWriteCount = 0;
            LOG_INFO(LOG_CAT_SYSTEM, "FlashWearCounter: First boot - initializing counter to 0");
        }
        
        // Write the counter back to EEPROM to fix the valid flag without incrementing
        FlashWearRecord resetRecord;
        resetRecord.value = _flashWriteCount;
        resetRecord.valid = FLASH_WEAR_VALID;
        resetRecord.reserved[0] = 0;
        resetRecord.reserved[1] = 0;
        resetRecord.reserved[2] = 0;

        if (!_writeFlashWearRecord(_counterAddress, &resetRecord, "FlashWearCounter")) {
            LOG_ERROR(LOG_CAT_SYSTEM, "FlashWearCounter: ERROR - Failed to initialize counter record");
            return false;
        }
    }
    
    _flashWearInitialized = true;
    _lastReportTime = millis();
    return true;
}

/**
 * Update the flash wear counter after a successful write operation
 */
bool updateFlashWearCounter() {
    if (!_flashWearInitialized) {
        LOG_CRITICAL(LOG_CAT_SYSTEM, "FlashWearCounter: ERROR - Not initialized!");
        return false;
    }
    
    const uint32_t previousCount = _flashWriteCount;
    _flashWriteCount++;
    // TODO-FLASHWEAR-2025-11-07: Changed from LOG_INFO to Serial.printf for always-visible output
    // Flash wear tracking must ALWAYS be visible, not subject to log level filtering
    // OLD: LOG_INFO(LOG_CAT_SYSTEM, "FlashWearCounter: update request %u -> %u\n", ...);
    // Gate for deletion: After confirming counter increments correctly with Serial output
    LOG_INFO(LOG_CAT_SYSTEM,
             "FlashWearCounter: update request %u -> %u",
             static_cast<unsigned>(previousCount),
             static_cast<unsigned>(_flashWriteCount));
    
    // Store updated count in EEPROM
    FlashWearRecord counterRecord;
    counterRecord.value = _flashWriteCount;
    counterRecord.valid = FLASH_WEAR_VALID;
    counterRecord.reserved[0] = 0;
    counterRecord.reserved[1] = 0;
    counterRecord.reserved[2] = 0;
    
    bool writeSuccess = _writeFlashWearRecord(_counterAddress, &counterRecord, "FlashWearCounter");
    
    if (!writeSuccess) {
        LOG_ERROR(LOG_CAT_SYSTEM, "FlashWearCounter: ERROR - Failed to write counter to EEPROM!");
        _flashWriteCount--; // Rollback the increment
        LOG_INFO(LOG_CAT_SYSTEM,
                 "FlashWearCounter: update rollback to %u",
                 static_cast<unsigned>(_flashWriteCount));
        return false;
    }
    
    // Check if approaching limit
    float percentage = (float)_flashWriteCount / _maxFlashWrites * 100.0;
    if (percentage >= _retirementThreshold) {
        LOG_CRITICAL(LOG_CAT_SYSTEM, "Flash wear at %.2f%% (%u/%u writes) - Device retirement required!\n", 
                     percentage, _flashWriteCount, _maxFlashWrites);
        handleDeviceRetirement();
    } else if (percentage >= _criticalThreshold) {
        LOG_WARN(LOG_CAT_SYSTEM, "WARNING: Flash wear at %.2f%% (%u/%u writes) - Device retirement recommended\n", 
                 percentage, _flashWriteCount, _maxFlashWrites);
    } else if (percentage >= _cautionThreshold) {
        LOG_WARN(LOG_CAT_SYSTEM, "CAUTION: Flash wear at %.2f%% (%u/%u writes)\n", 
                 percentage, _flashWriteCount, _maxFlashWrites);
    }
    
    return true;
}

/**
 * Get the current number of flash writes performed
 */
uint32_t getFlashWriteCount() {
    return _flashWriteCount;
}

/**
 * Get the current flash wear percentage
 */
float getFlashWearPercentage() {
    if (_maxFlashWrites == 0) return 0.0;
    return (float)_flashWriteCount / _maxFlashWrites * 100.0;
}

/**
 * Get the maximum number of writes before retirement
 */
uint32_t getMaxFlashWrites() {
    return _maxFlashWrites;
}

uint32_t getFlashWearCount() {
    return getFlashWriteCount();
}

uint32_t getMaxFlashWearCount() {
    return getMaxFlashWrites();
}

/**
 * Report comprehensive flash wear status
 */
bool reportFlashWearStatus(bool forceReport) {
    if (!_flashWearInitialized) {
        LOG_ERROR(LOG_CAT_SYSTEM, "FlashWearCounter: ERROR - Not initialized!");
        return false;
    }
    
    unsigned long currentTime = millis();
    if (!forceReport && (currentTime - _lastReportTime) < _reportInterval) {
        return false; // Too soon to report again
    }
    
    float percentage = getFlashWearPercentage();
    uint8_t warningLevel = getWarningLevel();
    
    LOG_INFO(LOG_CAT_SYSTEM, "Flash Wear Status: %u/%u writes (%.2f%% used)", 
                 _flashWriteCount, _maxFlashWrites, percentage);
    LOG_INFO(LOG_CAT_SYSTEM, "Boot Counter: %u boots", static_cast<unsigned>(_bootCount));
    
    switch (warningLevel) {
        case 0:
            LOG_INFO(LOG_CAT_SYSTEM, "Flash wear within normal limits");
            break;
        case 1:
            LOG_WARN(LOG_CAT_SYSTEM, "CAUTION: Flash wear at %.2f%% - Monitor usage", percentage);
            break;
        case 2:
            LOG_WARN(LOG_CAT_SYSTEM, "WARNING: Flash wear at %.2f%% - Consider device replacement", percentage);
            break;
        case 3:
            LOG_CRITICAL(LOG_CAT_SYSTEM, "CRITICAL: Flash wear at %.2f%% - Device retirement required", percentage);
            break;
    }
    
    _lastReportTime = currentTime;
    return true;
}

/**
 * Handle device retirement when write limit exceeded
 */
void handleDeviceRetirement() {
    _deviceRetired = true;
    LOG_CRITICAL(LOG_CAT_SYSTEM, "DEVICE RETIREMENT: Flash wear limit exceeded!");
    LOG_CRITICAL(LOG_CAT_SYSTEM, "This device should be replaced immediately!");
    LOG_CRITICAL(LOG_CAT_SYSTEM, "Continuing operation may result in data loss!");
}

/**
 * Check if device is in retirement mode
 */
bool isDeviceRetired() {
    return _deviceRetired;
}

/**
 * Set custom warning thresholds
 */
void setWarningThresholds(float caution, float critical, float retirement) {
    _cautionThreshold = caution;
    _criticalThreshold = critical;
    _retirementThreshold = retirement;
}

/**
 * Set the periodic report interval
 */
void setReportInterval(unsigned long intervalMs) {
    _reportInterval = intervalMs;
}

/**
 * Force an immediate status report
 */
bool forceStatusReport() {
    return reportFlashWearStatus(true);
}

/**
 * Reset the flash wear counter (use with caution)
 */
bool resetFlashWearCounter() {
    LOG_CRITICAL(LOG_CAT_SYSTEM, "FlashWearCounter: CRITICAL - Bad Programmer! - you can't reset that!");
    return false;

    if (!_flashWearInitialized) {
        LOG_ERROR(LOG_CAT_SYSTEM, "FlashWearCounter: ERROR - Not initialized!");
        return false;
    }
    
    LOG_WARN(LOG_CAT_SYSTEM, "FlashWearCounter: WARNING - Resetting flash wear counter!");
    _flashWriteCount = 0;
    _deviceRetired = false;

    FlashWearRecord resetRecord{};
    resetRecord.value = _flashWriteCount;
    resetRecord.valid = FLASH_WEAR_VALID;
    resetRecord.reserved[0] = 0;
    resetRecord.reserved[1] = 0;
    resetRecord.reserved[2] = 0;

    if (!_writeFlashWearRecord(_counterAddress, &resetRecord, "FlashWearCounter")) {
        LOG_ERROR(LOG_CAT_SYSTEM, "FlashWearCounter: ERROR - Failed to reset wear counter record");
        return false;
    }

    return true;
}

/**
 * Get flash wear status as a formatted string
 */
String getFlashWearStatusString() {
    if (!_flashWearInitialized) {
        return "FlashWearCounter: Not initialized";
    }
    
    float percentage = getFlashWearPercentage();
    uint8_t warningLevel = getWarningLevel();
    
    String status = "Flash Wear: " + String(_flashWriteCount) + "/" + String(_maxFlashWrites) + 
                   " (" + String(percentage, 2) + "%)";
    status += " | Boots: " + String(_bootCount);
    
    switch (warningLevel) {
        case 1: status += " [CAUTION]"; break;
        case 2: status += " [WARNING]"; break;
        case 3: status += " [CRITICAL]"; break;
    }
    
    return status;
}

/**
 * Check if a warning should be issued based on current wear level
 */
uint8_t getWarningLevel() {
    float percentage = getFlashWearPercentage();
    
    if (percentage >= _retirementThreshold) return 3; // Retirement
    if (percentage >= _criticalThreshold) return 2;   // Critical
    if (percentage >= _cautionThreshold) return 1;    // Caution
    return 0; // Normal
}

uint32_t getBootCount() {
    return _bootCount;
}

bool resetBootCounter() {
    if (!_bootCounterInitialized) {
        LOG_ERROR(LOG_CAT_SYSTEM, "BootCounter: ERROR - Not initialized!");
        return false;
    }

    FlashWearRecord resetRecord{};
    resetRecord.value = 0;
    resetRecord.valid = FLASH_WEAR_VALID;
    resetRecord.reserved[0] = 0;
    resetRecord.reserved[1] = 0;
    resetRecord.reserved[2] = 0;

    if (!_writeFlashWearRecord(_bootCounterAddress, &resetRecord, "BootCounter")) {
        LOG_ERROR(LOG_CAT_SYSTEM, "BootCounter: ERROR - Failed to reset boot counter record");
        return false;
    }

    _bootCount = 0;
    LOG_INFO(LOG_CAT_SYSTEM, "BootCounter: Counter reset to 0");
    return true;
}

String getBootCounterStatusString() {
    if (!_bootCounterInitialized) {
        return "BootCounter: Not initialized";
    }

    return "Boot Count: " + String(_bootCount);
}

// Private helper functions

/**
 * Initialize EEPROM for the platform
 */
static void _initializeEEPROM() {
#if defined(ESP32)
    EEPROM.begin(512); // ESP32 needs explicit size
    LOG_INFO(LOG_CAT_SYSTEM, "FlashWearCounter: EEPROM initialized with 512 bytes for ESP32");
#elif defined(ESP8266)
    EEPROM.begin(512); // ESP8266 needs explicit size
    LOG_INFO(LOG_CAT_SYSTEM, "FlashWearCounter: EEPROM initialized with 512 bytes for ESP8266");
#endif
}

/**
 * Read flash wear record from EEPROM
 */
static bool _readFlashWearRecord(uint16_t address, FlashWearRecord* record) {
    if (!record) return false;

    uint8_t* data = reinterpret_cast<uint8_t*>(record);
    for (uint8_t i = 0; i < FLASH_WEAR_RECORD_SIZE; i++) {
        data[i] = EEPROM.read(address + i);
    }

    return true;
}

static bool _writeFlashWearRecord(uint16_t address, const FlashWearRecord* record, const char* label) {
    if (!record) return false;

    const char* prefix = (label && label[0] != '\0') ? label : "FlashWearCounter";

    LOG_INFO(LOG_CAT_SYSTEM, "%s: Writing to EEPROM byte offset %u, value=%u, valid=0x%02X",
                  prefix, static_cast<unsigned>(address), record->value, record->valid);

    const uint8_t* data = reinterpret_cast<const uint8_t*>(record);
    for (uint8_t i = 0; i < FLASH_WEAR_RECORD_SIZE; i++) {
        EEPROM.write(address + i, data[i]);
    }

#if defined(ESP32) || defined(ESP8266)
    bool success = EEPROM.commit(); // Commit the write operation
    if (!success) {
        LOG_ERROR(LOG_CAT_SYSTEM, "%s: ERROR - EEPROM commit failed!", prefix);
        return false;
    }
    LOG_INFO(LOG_CAT_SYSTEM, "%s: EEPROM write committed successfully", prefix);
#endif

    return true;
}