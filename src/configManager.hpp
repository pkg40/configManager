/*
 * MIT License
 *
 * Copyright (c) 2025 Peter K Green (pkg40)
 * Email: pkg40@yahoo.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */



#pragma once
#include <Arduino.h>
#include <WString.h>
#include <map>
#include <vector>
#include <ArduinoJson.h>
#include "interface/iConfigProvider.hpp"


// Platform-specific filesystem includes
#if defined(ESP32S3) || defined(ESP32C3) || defined(ESP8266) || defined(USE_LITTLEFS)
#include <LittleFS.h> // Use LittleFS for ESP32S3, ESP32C3, ESP8266, or if forced
#elif defined(ESP32)
#include <SPIFFS.h>   // Classic ESP32 can use SPIFFS
#else
#error "Unsupported platform: Only ESP32, ESP32S3, ESP32C3, and ESP8266 are supported."
#endif

// Usage:
// For ESP8266, define USE_LITTLEFS to use LittleFS instead of SPIFFS.
// Example: Add -DUSE_LITTLEFS to build flags in platformio.ini
//
// ESP8266 Compatibility Notes:
// - ESP8266 has limited memory (~80KB heap). Large JSON configs may cause issues.
// - Recommended: Keep config files under 8KB for ESP8266
// - SPIFFS is supported on both ESP32 and ESP8266
// - LittleFS is more efficient and recommended for ESP8266 (requires USE_LITTLEFS flag)
// - Flash filesystem must be initialized before use
//
// Memory Usage Guidelines:
// ESP32: Can handle large configs (32KB+) 

/**
 * @file configManager.hpp
 * @brief Persistent JSON configuration manager for ESP32/ESP8266
 * @author Peter K Green (pkg40@yahoo.com)
 */
// ESP8266: Recommended max config size 8KB, test with actual hardware
// Consider using config sections to load only needed parts on ESP8266

class configManager : public iConfigProvider
{
private:
    std::map<String, std::map<String, String>> _config; // section → field → value
    bool begin( String filename, bool = true);
    bool loadConfigString(const char *filename, String *jsonString, bool verbose = true);
    std::map<String, std::map<String, String>> jsonStringToMap(const String &jsonString, bool = false);
    bool jsonStringToConfig(const String &jsonString, bool = false);
    String mapToJsonString(const std::map<String, std::map<String, String>> &configMap);
    bool saveToJson(const String &path, const std::map<String, std::map<String, String>> &configMap);
    bool saveConfigFile(const char *filename);
    String loadDefaults();
    const std::map<String, std::map<String, String>> &getConfig() const;

public:
    configManager() = default;
    ~configManager() = default;


    String getValue(const String &section, const String &key) const override;
    void setValue(const String &section, const String &key, const String &value) override;

    // Alias for interface compatibility
    std::vector<String> getSectionNames() const override { return getSections(); }


    // Optional auth helpers
    String getUser() const override;
    String getPassword() const override;

    void printConfigToSerial() const;
    std::map<String, String> &getSection(const String &sectionName);
    const std::map<String, String>& getSection(const String &sectionName) const override;
    bool parseHexStringToBytes(const String &hexInput, uint8_t *out, size_t outLen) ;

    // iConfigProvider interface implementation
    std::vector<String> getSections() const override;
    std::vector<String> getFormatSections() const override;
    std::vector<String> getKeys(const String& section) const override;
    bool saveConfig() override;
    bool loadConfig() override;
    void printHeapStatus() const;
    size_t getConfigMemoryUsage() const;
    bool clearConfig();
};