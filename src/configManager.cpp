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


#include <configManager.hpp>

bool configManager::begin(const String filename, bool verbose)
{
    String jsonString;
    _config.clear(); // Always start clean

    // Initialize filesystem - ESP32 and ESP8266 have different SPIFFS.begin() signatures
    // ESP32: SPIFFS.begin(format_if_failed) - can auto-format if filesystem is corrupted
    // ESP8266: SPIFFS.begin() - no parameters, manual formatting required if corrupted
    #if defined(ESP32)
        if (!SPIFFS.begin(true))
    #elif defined(ESP8266)  
        if (!SPIFFS.begin())
    #endif
    {
        if (verbose)
            Serial.println("❌ SPIFFS Mount Failed. Loading defaults...");
        jsonString = loadDefaults();
    }
    else
    {
        if (verbose)
            Serial.println("✅ SPIFFS mounted");

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
    _config = jsonStringToMap(jsonString, verbose);

    if (_config.empty())
    {
        if (verbose)
            Serial.println("⚠️ JSON parsing failed or produced empty config.");
        jsonString = loadDefaults();
        _config = jsonStringToMap(jsonString);
    }

    if (verbose)
    {
        Serial.printf("✅ Loaded %zu config sections\n", _config.size());
        printConfigToSerial();
    }

    return !_config.empty();
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
    File file = SPIFFS.open(filename, "r");
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
    _config = jsonStringToMap(jsonString, verbose);
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

    File file = SPIFFS.open(path, "w");
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
    return saveToJson(filename, _config);
}

String configManager::getValue(const String &section, const String &key) const
{
    if (_config.count(section) && _config.at(section).count(key))
    {
        return _config.at(section).at(key);
    }
    return "[NOT FOUND]";
}

void configManager::setValue(const String &section, const String &key, const String &value)
{
    _config[section][key] = value;
}

const std::map<String, std::map<String, String>> &configManager::getConfig() const
{
    return _config;
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
    for (const auto &section : _config)
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
    return _config[sectionName];
}

const std::map<String, String>& configManager::getSection(const String &sectionName) const
{
    static const std::map<String, String> emptyMap;
    auto it = _config.find(sectionName);
    if (it != _config.end()) {
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
    for (const auto& section : _config)
    {
        sections.push_back(section.first);
    }
    return sections;
}

std::vector<String> configManager::getFormatSections() const
{
    std::vector<String> formatSections;
    for (const auto& section : _config)
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
    auto sectionIt = _config.find(section);
    if (sectionIt != _config.end())
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