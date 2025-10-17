#pragma once

#include <Arduino.h>
#include <myEEProm.hpp> // For eePromRec and EESIZE
#include <moCoEEprom.hpp> // For MoCoEEProm class

/**
 * Flash Wear Counter Library
 * 
 * Provides comprehensive flash wear tracking and device lifecycle management
 * for any device using the config libraries. Tracks flash write operations
 * and manages device retirement based on configurable limits.
 */

// Configuration constants
const uint32_t DEFAULT_MAX_FLASH_WRITES = 12000000; // 12 million writes
const uint8_t DEFAULT_FLASH_WEAR_COUNTER_ADDRESS = 255; // Last EEPROM address
const unsigned long DEFAULT_REPORT_INTERVAL_MS = 300000; // 5 minutes

// Warning thresholds (percentages)
const float CAUTION_THRESHOLD = 75.0;
const float CRITICAL_THRESHOLD = 90.0;
const float RETIREMENT_THRESHOLD = 100.0;

/**
 * Initialize the flash wear counter system
 * @param eepromInstance pointer to EEPROM instance for counter storage
 * @param maxWrites maximum number of writes before retirement (default: 12M)
 * @param counterAddress EEPROM address for counter storage (default: 255)
 * @return true if initialization successful, false otherwise
 */
bool initFlashWearCounter(MoCoEEProm* eepromInstance, 
                         uint32_t maxWrites = DEFAULT_MAX_FLASH_WRITES,
                         uint8_t counterAddress = DEFAULT_FLASH_WEAR_COUNTER_ADDRESS);

/**
 * Update the flash wear counter after a successful write operation
 * @return true if counter updated successfully, false otherwise
 */
bool updateFlashWearCounter();

/**
 * Get the current number of flash writes performed
 * @return current write count
 */
uint32_t getFlashWriteCount();

/**
 * Get the current flash wear percentage
 * @return percentage of limit used (0.0-100.0)
 */
float getFlashWearPercentage();

/**
 * Get the maximum number of writes before retirement
 * @return maximum write limit
 */
uint32_t getMaxFlashWrites();

/**
 * Report comprehensive flash wear status
 * @param forceReport if true, report even if recently reported
 * @return true if report generated, false otherwise
 */
bool reportFlashWearStatus(bool forceReport = false);

/**
 * Handle device retirement when write limit exceeded
 * This function should be called when the device reaches retirement threshold
 */
void handleDeviceRetirement();

/**
 * Check if device is in retirement mode
 * @return true if device should be retired, false otherwise
 */
bool isDeviceRetired();

/**
 * Set custom warning thresholds
 * @param caution threshold for caution warnings (default: 75.0)
 * @param critical threshold for critical warnings (default: 90.0)
 * @param retirement threshold for retirement mode (default: 100.0)
 */
void setWarningThresholds(float caution = CAUTION_THRESHOLD,
                         float critical = CRITICAL_THRESHOLD,
                         float retirement = RETIREMENT_THRESHOLD);

/**
 * Set the periodic report interval
 * @param intervalMs interval in milliseconds (default: 300000 = 5 minutes)
 */
void setReportInterval(unsigned long intervalMs);

/**
 * Force an immediate status report
 * @return true if report generated successfully
 */
bool forceStatusReport();

/**
 * Reset the flash wear counter (use with caution)
 * This should only be used for testing or after device replacement
 * @return true if reset successful, false otherwise
 */
bool resetFlashWearCounter();

/**
 * Get flash wear status as a formatted string
 * @return formatted status string
 */
String getFlashWearStatusString();

/**
 * Check if a warning should be issued based on current wear level
 * @return warning level: 0=none, 1=caution, 2=critical, 3=retirement
 */
uint8_t getWarningLevel();
