#pragma once

#ifndef CONFIGMGR_NATIVE
#include <interface/iFileSystemProvider.hpp>

#if defined(ESP32) || defined(ESP8266)
#include <LittleFS.h>
#define LITTLEFS_CONFIG_FS LittleFS
#else
#error "LittleFS provider only supports ESP32 and ESP8266 platforms"
#endif

/**
 * @brief A LittleFS-specific implementation of iFileSystemProvider for ESP32/ESP8266 platforms.
 *
 * This class specifically uses LittleFS filesystem for both ESP32 and ESP8266 platforms,
 * providing consistent filesystem behavior across both architectures.
 * 
 * Features:
 * - Uses LittleFS on both ESP32 and ESP8266
 * - Automatic formatting on mount failure
 * - Platform-specific initialization handling
 */
class littleFSProvider : public iFileSystemProvider
{
public:
    bool begin() override;
    bool end() override;
    fs::File open(const char *path, const char *mode) override;
    bool remove(const char *path) override;
    bool exists(const char *path) override;
    
    // Additional LittleFS-specific methods
    size_t totalBytes();
    size_t usedBytes();
    bool format();
};

#endif // !CONFIGMGR_NATIVE