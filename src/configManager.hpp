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
#include <addressMapping.hpp>
//#include <GlobalDefinitions.h>  // For USE_FLASH_WEAR_COUNTER macro
#include <ArduinoJson.h>
#include "interface/iFileSystemProvider.hpp" // Use the file system provider interface
#include "interface/iConfigProvider.hpp" // Use the config provider interface
#include <logger.hpp>
#include <map>
#include <vector>

// Debug helper defined in commonFunctions.cpp
const char* stringAddressName(eePromAddress_t address);

// Conditional interface inclusion for efficiency
#ifndef CONFIG_DISABLE_INTERFACE
    #define CONFIG_BASE_CLASS : public iConfigProvider
    #define CONFIG_OVERRIDE override
#else
    #define CONFIG_BASE_CLASS
    #define CONFIG_OVERRIDE
#endif

// Usage:
// For ESP8266, define USE_LITTLEFS to use LittleFS instead of SPIFFS.
// Example: Add -DUSE_LITTLEFS to build flags in platformio.ini
//
// Interface Override for Code Efficiency:
// Define CONFIG_DISABLE_INTERFACE to remove virtual interface inheritance
// This eliminates vtable overhead for performance-critical applications
// Example: Add -DCONFIG_DISABLE_INTERFACE to build flags
// Warning: Disabling interface removes polymorphic capabilities
//
// ESP8266 Compatibility Notes:
// - ESP8266 has limited memory (~80KB heap). Large JSON configs may cause issues.
// - Recommended: Keep config files under 8KB for ESP8266
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

//class configManager CONFIG_BASE_CLASS
class configManager : public iConfigProvider
{
    #ifdef TESTBENCH
    friend class testLib;
    #endif
private:
    iFileSystemProvider* _fsProvider;
    std::map<String, std::map<String, String>> _configMap;
    const size_t _maxConfigSize;
    String _configFilePath;

    bool _isConfigLoaded;
    
    // Domain registration for web interface
    String _domainName;
    bool _isDefaultDomain;
    
    // Static registry for all config domains
    static std::map<String, configManager*> _domainRegistry;
    static configManager* _defaultDomain;

    bool begin(String filename, bool verbose = true);
    bool loadConfigString(const char* filename, String* jsonString, bool verbose = true);
    bool jsonStringToMap(const String& jsonString, std::map<String, std::map<String, String>>& configMap, bool verbose = false);
    bool jsonStringToConfig(const String& jsonString, bool verbose = false);
    String mapToJsonString(const std::map<String, std::map<String, String>>& configMap) const;
    bool saveToJson(const String& path, const std::map<String, std::map<String, String>>& configMap) const;
    bool saveConfigFile(const char* filename);
    String loadDefaults() const;
    const std::map<String, std::map<String, String>>& getConfig() const;

public:
    explicit configManager(iFileSystemProvider* fsProvider, const String& configFilePath = "/config.json", size_t maxConfigSize = 8192);
    ~configManager();
    
    // Domain registration for web interface
    void setDomain(const String& domainName, bool isDefault = false);
    String getDomain() const { return _domainName; }
    bool isDefaultDomain() const { return _isDefaultDomain; }
    
    // Static methods to access domain registry
    static std::vector<String> getDomainNames();
    static configManager* getDomain(const String& domainName);
    static configManager* getDefaultDomain();

    String getValue(const String& section, const String& key) const CONFIG_OVERRIDE;
    void setValue(const String& section, const String& key, const String& value) CONFIG_OVERRIDE;

    String getValue(const eePromAddress_t address, bool verbose = false) const {
        String tmpStr = getValue(addressMap[address].section, addressMap[address].key);
        if (verbose) {
            LOG_INFO(LOG_CAT_SYSTEM, "getValue: Retrieving value=%s for address=%s, section=%s, key=%s",
                tmpStr.c_str(), stringAddressName(address),
                addressMap[address].section, addressMap[address].key);
        }
        return tmpStr;
    }

    void setValue(const eePromAddress_t address, const String& value) {
        setValue(addressMap[address].section, addressMap[address].key, value);
    }

    String getSectionName(const eePromAddress_t address) const {
        return addressMap[address].section;
    }

    String getKeyName(const eePromAddress_t address) const {
        return addressMap[address].key;
    }

    String getName(const eePromAddress_t address) const {
        return addressMap[address].name;
    }

    // Alias for interface compatibility
    std::vector<String> getSectionNames() const CONFIG_OVERRIDE { return getSections(); }

    // Optional auth helpers
    String getUser() const CONFIG_OVERRIDE;
    String getPassword() const CONFIG_OVERRIDE;

    void printConfigToSerial() const;
    std::map<String, String>& getSection(const String& sectionName);
    const std::map<String, String>& getSection(const String& sectionName) const CONFIG_OVERRIDE;
    bool parseHexStringToBytes(const String& hexInput, uint8_t* outputBuffer, size_t bufferLen) const;

    // iConfigProvider interface implementation
    std::vector<String> getSections() const CONFIG_OVERRIDE;
    std::vector<String> getFormatSections() const CONFIG_OVERRIDE;
    std::vector<String> getKeys(const String& section) const CONFIG_OVERRIDE;
    bool saveConfig() CONFIG_OVERRIDE;
    bool loadConfig() CONFIG_OVERRIDE;
    void printHeapStatus() const;
    size_t getConfigMemoryUsage() const;
    bool clearConfig();
    
    // Get filesystem provider for use by web server modules
    iFileSystemProvider* getFileSystemProvider() const { return _fsProvider; }
};