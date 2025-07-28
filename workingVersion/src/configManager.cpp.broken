// Minimal test method for linkage check
int configManager::testLinkage(int x) const {
    return x + 42;
}
// Minimal test method for linkage check
int configManager::testLinkage(int x) const {
    return x + 42;
}
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
 *
 * ESP8266/ESP32 Configuration Manager
 * 
 * Cross-platform JSON configuration manager supporting both ESP32 and ESP8266.
 * Features:
 * - Automatic platform detection and filesystem initialization
 * - Support for SPIFFS (both platforms) and LittleFS (ESP8266 recommended)
 * - Memory-efficient JSON parsing with configurable buffer sizes
 * - Default configuration fallback system
 * 
 * ESP8266 Specific Considerations:
 * - Limited heap memory (~80KB available) 
 * - SPIFFS.begin() on ESP8266 doesn't take parameters (vs ESP32 which takes format flag)
 * - Recommended to use LittleFS on ESP8266 for better performance and reliability
 * - Test thoroughly with actual hardware due to memory constraints
 * 
 * Memory Usage:
 * - JSON parsing uses ArduinoJson which allocates memory for the document
 * - Large configs (>8KB) may cause issues on ESP8266
 * - Consider splitting large configs into multiple files if needed
 */

/**
 * @file configManager.cpp
 * @brief Implementation of persistent JSON configuration manager for ESP32/ESP8266
 * @author Peter K Green (pkg40@yahoo.com)
 */

#include <configManager.hpp>

// Filesystem selection macro
#if defined(USE_LITTLEFS)
  #include <LittleFS.h>
  #define CONFIG_FS LittleFS
#else
  #include <SPIFFS.h>
  #define CONFIG_FS SPIFFS
#endif

bool configManager::begin(const String filename, bool verbose)
{
    String jsonString;
    _configMap.clear(); // Always start clean

    // Initialize filesystem - ESP32 and ESP8266 have different CONFIG_FS.begin() signatures
    // ESP32: CONFIG_FS.begin(format_if_failed) - can auto-format if filesystem is corrupted
    // ESP8266: CONFIG_FS.begin() - no parameters, manual formatting required if corrupted
    #if defined(ESP32)
        if (!CONFIG_FS.begin(true))
    #elif defined(ESP8266)  
        if (!CONFIG_FS.begin())
    #endif
    {
        if (verbose)
            Serial.println("❌ CONFIG_FS Mount Failed. Loading defaults...");
        jsonString = loadDefaults();
    }
    else
    {
        if (verbose)
            Serial.println("✅ CONFIG_FS mounted");

        if (!loadConfigString(filename.c_str(), &jsonString, verbose))
        {
            if (verbose)
                Serial.println("⚠️ Failed to read config file. Loading defaults...");
            jsonString = loadDefaults();
        }

    }

    if (verbose && !jsonString.isEmpty())
    {
        Serial.println("📄 Raw config string:");
        Serial.println(jsonString);
    }

    // Deserialize
    _configMap = jsonStringToMap(jsonString, verbose);

    if (_configMap.empty())
    {
        if (verbose)
            Serial.println("⚠️ JSON parsing failed or produced empty config.");
        jsonString = loadDefaults();
        _configMap = jsonStringToMap(jsonString);
    }

    if (verbose)
    {
        Serial.printf("✅ Loaded %zu config sections\n", _configMap.size());
        printConfigToSerial();
    }

    return !_configMap.empty();
}

String configManager::loadDefaults()
{
    const char *defaultJSON = R"rawlite(
{
  "auth.format": {
    "_user": "string",
    "_password": "string"
  },
  "net.format": {
    "use": "checkbox",
    "channel": "integer",
    "ssid": "string",
    "password": "string"
  },
  "espnow.format": {
    "use": "checkbox",
    "broadcast": "checkbox",
    "user": "string",
    "channel": "integer",
    "devicemac": "macaddress",
    "remotemac": "macaddress",
    "sharedID": "integer"
  },
  "flag.format": {
    "wifistart": "checkbox",
    "webstart": "checkbox",
    "rebootflag": "checkbox",
    "wificonnected": "checkbox"
  },
  "mqtt.format": {
    "use": "checkbox",
    "user": "string",
    "topic": "string",
    "serverIP": "ipaddress",
    "serverPort": "integer",
    "password": "string"
  },
  "wifiAP": {
    "format.use": "net.format",
    "use": "true",
    "channel": "1",
    "ip": "192.168.4.1",
    "port": "80",
    "name": "RudeAP",
    "ssid": "espNowAP",
    "password": "espNowAP"
  },
  "wifiSTA": {
    "format.use": "netformat",
    "use": "true",
    "ip": "",
    "channel": "1",
    "port": "",
    "name": "RudeSTA",
    "ssid": "greenhouse",
    "password": "mytoonis42273211"
  },
  "mqtt": {
    "format.use": "mqtt.format",
    "use": "true",
    "ip": "10.0.0.88",
    "port": "1883",
    "topic": "rudename"
  },
  "_auth": {
    "format.use": "auth.format",
    "_user": "admin",
    "_password": "admin"
  },
  "updates": {
    "topic": "system/online"
  }
}
)rawlite";
    return defaultJSON;
}

bool configManager::loadConfigString(const char *filename, String *jsonString, bool verbose)
{
    File file = CONFIG_FS.open(filename, "r");
    if (!file)
    {
        if (verbose)
            Serial.println("❌ Failed to open config file.");
        *jsonString = loadDefaults();
    }
    else
    {
        *jsonString = file.readString();
        file.close();
    }

    if (jsonString->isEmpty())
    {
        if (verbose)
            Serial.println("⚠️ Config file empty.");
        *jsonString = loadDefaults();
        return false;
    }

    if (verbose)
        Serial.println("✅ Config file loaded.");
    return true;
}

bool configManager::jsonStringToConfig(const String &jsonString, bool verbose)
{
    _configMap = jsonStringToMap(jsonString, verbose);
    return true;
}

std::map<String, std::map<String, String>> configManager::jsonStringToMap(const String &jsonString, bool verbose)
{
    std::map<String, std::map<String, String>> configMap = {};

    JsonDocument doc; // Use JsonDocument instead of DynamicJsonDocument
    Serial.printf("⚠️ JSON string length: %u\n", jsonString.length());
    Serial.println("📄 JSON preview:");
    Serial.println(jsonString);

    DeserializationError error = deserializeJson(doc, jsonString);
    if (!doc.is<JsonObject>()) {
    Serial.println("⚠️ Root element is not an object!");
    return {};
}
    if (error)
    {
        Serial.println("❌ Failed to parse JSON");
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return configMap;
    }

    JsonObjectConst root = doc.as<JsonObjectConst>();
    if (verbose)
    {
        Serial.println("✅ JSON parsed successfully");
        Serial.printf("📊 Root object has %d keys\n", root.size());
    }

    for (JsonPairConst kv : root)
    {
        const String &sectionName = kv.key().c_str();
        JsonVariantConst sectionValue = kv.value();

        if (sectionValue.is<JsonObjectConst>())
        {
            JsonObjectConst sectionObj = sectionValue.as<JsonObjectConst>();
            std::map<String, String> sectionMap;

            for (JsonPairConst sectionKv : sectionObj)
            {
                const String &key = sectionKv.key().c_str();
                const String &value = sectionKv.value().as<String>();
                sectionMap[key] = value;
            }

            configMap[sectionName] = sectionMap;
        }
        else
        {
            Serial.printf("⚠️ Section '%s' is not an object\n", sectionName.c_str());
        }
    }

    if (verbose)
    {
        Serial.printf("✅ Parsed %d sections\n", configMap.size());
    }

    return configMap;
}

bool configManager::saveToJson(const String &path, const std::map<String, std::map<String, String>> &configMap)
{
    String jsonOutput = mapToJsonString(configMap);

    File file = CONFIG_FS.open(path, "w");
    if (!file)
    {
        Serial.println("❌ Failed to open file for writing: " + path);
        return false;
    }

    size_t written = file.print(jsonOutput);
    file.close();

    if (written == 0)
    {
        Serial.println("⚠️ Nothing was written to " + path);
        return false;
    }

    Serial.printf("✅ Config saved to %s (%u bytes)\n", path.c_str(), written);
    return true;
}

bool configManager::saveConfigFile(const char *filename)
{
    return saveToJson(filename, _configMap);
}

String configManager::getValue(const String &section, const String &key) const
{
    if (_configMap.count(section) && _configMap.at(section).count(key))
    {
        return _configMap.at(section).at(key);
    }
    return "[NOT FOUND]";
}

void configManager::setValue(const String &section, const String &key, const String &value)
{
    _configMap[section][key] = value;
}

const std::map<String, std::map<String, String>> &configManager::getConfig() const
{
    return _configMap;
}

String configManager::getUser() const
{
    return getValue("_auth", "user");
}

String configManager::getPassword() const
{
    return getValue("_auth", "password");
}

void configManager::printConfigToSerial() const
{
    Serial.println("\n===== Configuration Map =====");
    for (const auto &section : _configMap)
    {
        Serial.println("[" + section.first + "]");
        for (const auto &field : section.second)
        {
            Serial.printf("  %s: %s\n", field.first.c_str(), field.second.c_str());
        }
    }
    Serial.println("=============================\n");
}

String configManager::mapToJsonString(const std::map<String, std::map<String, String>> &configMap)
{
    JsonDocument doc; // Use JsonDocument instead of DynamicJsonDocument

    for (const auto &sectionPair : configMap)
    {
        const String &sectionName = sectionPair.first;
        const auto &sectionData = sectionPair.second;

        JsonObject sectionObj = doc[sectionName].to<JsonObject>();
        for (const auto &kv : sectionData)
        {
            sectionObj[kv.first] = kv.second;
        }
    }

    String output;
    serializeJsonPretty(doc, output);
    return output;
}


std::map<String, String> &configManager::getSection(const String &sectionName)
{
    return _configMap[sectionName];
}

const std::map<String, String>& configManager::getSection(const String &sectionName) const
{
    static const std::map<String, String> emptyMap;
    auto it = _configMap.find(sectionName);
    if (it != _configMap.end()) {
        return it->second;
    }
    return emptyMap;
}

bool configManager::parseHexStringToBytes(const String &hexInput, uint8_t *out, size_t outLen)
{
    String hex = hexInput;
    hex.replace(" ", "");
    hex.toUpperCase();

    if (hex.startsWith("0X"))
    {
        hex = hex.substring(2);
    }

    if (hex.length() != outLen * 2)
        return false;

    for (size_t i = 0; i < outLen; ++i)
    {
        char byteStr[3] = {hex[2 * i], hex[2 * i + 1], '\0'};
        char *end = nullptr;
        out[i] = static_cast<uint8_t>(strtol(byteStr, &end, 16));
        if (*end != '\0')
            return false;
    }

    return true;
}

// iConfigProvider interface implementation
std::vector<String> configManager::getSections() const
{
    std::vector<String> sections;
    for (const auto& section : _configMap)
    {
        sections.push_back(section.first);
    }
    return sections;
}

std::vector<String> configManager::getFormatSections() const
{
    std::vector<String> formatSections;
    for (const auto& section : _configMap)
    {
        String formatSection = section.first;
        if (formatSection.startsWith("_"))
        {
            formatSection = formatSection.substring(1);
        }
        formatSections.push_back(formatSection);
    }
    return formatSections;
}

std::vector<String> configManager::getKeys(const String& section) const
{
    std::vector<String> keys;
    auto sectionIt = _configMap.find(section);
    if (sectionIt != _configMap.end())
    {
        for (const auto& keyValue : sectionIt->second)
        {
            keys.push_back(keyValue.first);
        }
    }
    return keys;
}

bool configManager::saveConfig()
{
    return saveConfigFile("/config.json");
}

bool configManager::loadConfig()
{
    return begin("/config.json", true);
}

/**
 * @brief Print current heap status to Serial (ESP8266/ESP32 only).
 *
 * Prints free heap and config memory usage for diagnostics.
 */
void configManager::printHeapStatus() const {
#if defined(ESP8266) || defined(ESP32)
    Serial.printf("[configManager] Free heap: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("[configManager] Config memory usage: %u bytes\n", getConfigMemoryUsage());
#else
    // No-op for other platforms
#endif
}

/**
 * @brief Get the memory usage of the loaded config (in bytes).
 *
 * @return size_t Number of bytes used by the config in memory.
 */
size_t configManager::getConfigMemoryUsage() const {
    // Estimate memory usage of config (JSON + STL containers)
    size_t total = 0;
    for (const auto& section : _configMap) {
        total += section.first.length();
        for (const auto& kv : section.second) {
            total += kv.first.length() + kv.second.length();
        }
    }
    return total;
}

/**
 * @brief Erase the config file and reset to defaults.
 *
 * Removes the config file from the filesystem and clears in-memory config.
 * Safe to call at any time.
 * @return true if config was cleared, false otherwise.
 */
bool configManager::clearConfig() {
#if defined(ESP8266) || defined(ESP32)
#if defined(USE_LITTLEFS)
    bool removed = LittleFS.remove("/config.json");
#else
    bool removed = CONFIG_FS.remove("/config.json");
#endif
    _configMap.clear();
    return removed;
#else
    // No-op for other platforms
    return false;
#endif
}