#include "flashWearCounter.hpp"

// Static variables for flash wear tracking
static MoCoEEProm* _eepromInstance = nullptr;
static uint32_t _maxFlashWrites = DEFAULT_MAX_FLASH_WRITES;
static uint8_t _counterAddress = DEFAULT_FLASH_WEAR_COUNTER_ADDRESS;
static uint32_t _flashWriteCount = 0;
static bool _flashWearInitialized = false;
static unsigned long _lastReportTime = 0;
static unsigned long _reportInterval = DEFAULT_REPORT_INTERVAL_MS;

// Warning thresholds
static float _cautionThreshold = CAUTION_THRESHOLD;
static float _criticalThreshold = CRITICAL_THRESHOLD;
static float _retirementThreshold = RETIREMENT_THRESHOLD;

/**
 * Initialize the flash wear counter system
 */
bool initFlashWearCounter(MoCoEEProm* eepromInstance, 
                         uint32_t maxWrites,
                         uint8_t counterAddress) {
    if (_flashWearInitialized) {
        Serial.println("FlashWearCounter: Already initialized");
        return true;
    }
    
    if (!eepromInstance) {
        Serial.println("FlashWearCounter: ERROR - EEPROM instance is null!");
        return false;
    }
    
    _eepromInstance = eepromInstance;
    _maxFlashWrites = maxWrites;
    _counterAddress = counterAddress;
    
    // Read current write count from EEPROM
    eePromRec counterRecord;
    _eepromInstance->getUser(_counterAddress * EESIZE, counterRecord, false);
    
    if (counterRecord.valid == EEVALID) {
        _flashWriteCount = counterRecord.value;
        Serial.printf("FlashWearCounter: Initialized with %u writes (%.2f%% of limit)\n", 
                     _flashWriteCount, (float)_flashWriteCount / _maxFlashWrites * 100.0);
    } else {
        // First boot or corrupted counter - initialize to 0
        _flashWriteCount = 0;
        updateFlashWearCounter();
        Serial.println("FlashWearCounter: Initialized with 0 writes (0.00% of limit)");
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
        Serial.println("FlashWearCounter: ERROR - Not initialized!");
        return false;
    }
    
    if (!_eepromInstance) {
        Serial.println("FlashWearCounter: ERROR - EEPROM instance is null!");
        return false;
    }
    
    _flashWriteCount++;
    
    // Store updated count in EEPROM (more durable than flash)
    eePromRec counterRecord;
    counterRecord.value = _flashWriteCount;
    counterRecord.valid = EEVALID;
    _eepromInstance->putUser(_counterAddress * EESIZE, counterRecord, false);
    
    // Check if approaching limit
    float percentage = (float)_flashWriteCount / _maxFlashWrites * 100.0;
    if (percentage >= _retirementThreshold) {
        Serial.printf("🚨 CRITICAL: Flash wear at %.2f%% (%u/%u writes) - Device retirement required!\n", 
                     percentage, _flashWriteCount, _maxFlashWrites);
        handleDeviceRetirement();
    } else if (percentage >= _criticalThreshold) {
        Serial.printf("⚠️  WARNING: Flash wear at %.2f%% (%u/%u writes) - Device retirement recommended\n", 
                     percentage, _flashWriteCount, _maxFlashWrites);
    } else if (percentage >= _cautionThreshold) {
        Serial.printf("⚠️  CAUTION: Flash wear at %.2f%% (%u/%u writes)\n", 
                     percentage, _flashWriteCount, _maxFlashWrites);
    }
    
    return true;
}

/**
 * Get the current number of flash writes performed
 */
uint32_t getFlashWriteCount() {
    if (!_flashWearInitialized) {
        Serial.println("FlashWearCounter: ERROR - Not initialized!");
        return 0;
    }
    return _flashWriteCount;
}

/**
 * Get the current flash wear percentage
 */
float getFlashWearPercentage() {
    if (!_flashWearInitialized) {
        Serial.println("FlashWearCounter: ERROR - Not initialized!");
        return 0.0;
    }
    return (float)_flashWriteCount / _maxFlashWrites * 100.0;
}

/**
 * Get the maximum number of writes before retirement
 */
uint32_t getMaxFlashWrites() {
    return _maxFlashWrites;
}

/**
 * Report comprehensive flash wear status
 */
bool reportFlashWearStatus(bool forceReport) {
    if (!_flashWearInitialized) {
        Serial.println("FlashWearCounter: ERROR - Not initialized!");
        return false;
    }
    
    // Check if we should report (either forced or interval elapsed)
    unsigned long currentTime = millis();
    if (!forceReport && (currentTime - _lastReportTime) < _reportInterval) {
        return false; // Too soon to report
    }
    
    float percentage = getFlashWearPercentage();
    Serial.printf("📊 Flash Wear Status: %u/%u writes (%.2f%% used)\n", 
                 _flashWriteCount, _maxFlashWrites, percentage);
    
    // Determine status and issue appropriate warnings
    if (percentage >= _retirementThreshold) {
        Serial.println("🚨 CRITICAL: Flash write limit exceeded - Device should be retired immediately!");
        handleDeviceRetirement();
    } else if (percentage >= _criticalThreshold) {
        Serial.println("⚠️  WARNING: Flash wear critical - Device retirement recommended");
    } else if (percentage >= _cautionThreshold) {
        Serial.println("⚠️  CAUTION: High flash wear detected");
    } else {
        Serial.println("✅ Flash wear within normal limits");
    }
    
    _lastReportTime = currentTime;
    return true;
}

/**
 * Handle device retirement when write limit exceeded
 */
void handleDeviceRetirement() {
    Serial.println("🛑 DEVICE RETIREMENT MODE ACTIVATED");
    Serial.println("   - Flash write limit exceeded");
    Serial.println("   - Device should be replaced immediately");
    Serial.println("   - Continuing operation may cause data loss");
    Serial.println("   - Config writes disabled to prevent further damage");
    
    // You could add additional retirement logic here:
    // - Disable certain features
    // - Enter safe mode
    // - Send alerts to monitoring systems
    // - etc.
}

/**
 * Check if device is in retirement mode
 */
bool isDeviceRetired() {
    if (!_flashWearInitialized) {
        return false;
    }
    return getFlashWearPercentage() >= _retirementThreshold;
}

/**
 * Set custom warning thresholds
 */
void setWarningThresholds(float caution, float critical, float retirement) {
    _cautionThreshold = caution;
    _criticalThreshold = critical;
    _retirementThreshold = retirement;
    
    Serial.printf("FlashWearCounter: Warning thresholds set - Caution: %.1f%%, Critical: %.1f%%, Retirement: %.1f%%\n",
                 caution, critical, retirement);
}

/**
 * Set the periodic report interval
 */
void setReportInterval(unsigned long intervalMs) {
    _reportInterval = intervalMs;
    Serial.printf("FlashWearCounter: Report interval set to %lu ms (%.1f minutes)\n", 
                 intervalMs, intervalMs / 60000.0);
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
    if (!_flashWearInitialized) {
        Serial.println("FlashWearCounter: ERROR - Not initialized!");
        return false;
    }
    
    if (!_eepromInstance) {
        Serial.println("FlashWearCounter: ERROR - EEPROM instance is null!");
        return false;
    }
    
    _flashWriteCount = 0;
    
    // Store reset count in EEPROM
    eePromRec counterRecord;
    counterRecord.value = _flashWriteCount;
    counterRecord.valid = EEVALID;
    _eepromInstance->putUser(_counterAddress * EESIZE, counterRecord, false);
    
    Serial.println("FlashWearCounter: Counter reset to 0");
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
    String status = "📊 Flash Wear: " + String(_flashWriteCount) + "/" + String(_maxFlashWrites) + 
                   " writes (" + String(percentage, 2) + "% used)";
    
    if (percentage >= _retirementThreshold) {
        status += " - 🚨 RETIREMENT REQUIRED";
    } else if (percentage >= _criticalThreshold) {
        status += " - ⚠️ CRITICAL";
    } else if (percentage >= _cautionThreshold) {
        status += " - ⚠️ CAUTION";
    } else {
        status += " - ✅ NORMAL";
    }
    
    return status;
}

/**
 * Check if a warning should be issued based on current wear level
 */
uint8_t getWarningLevel() {
    if (!_flashWearInitialized) {
        return 0;
    }
    
    float percentage = getFlashWearPercentage();
    if (percentage >= _retirementThreshold) {
        return 3; // Retirement
    } else if (percentage >= _criticalThreshold) {
        return 2; // Critical
    } else if (percentage >= _cautionThreshold) {
        return 1; // Caution
    } else {
        return 0; // Normal
    }
}
