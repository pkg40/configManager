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

/**
 * @file configManager.cpp
 * @brief Implementation of persistent JSON configuration manager for ESP32/ESP8266
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <map>
#include <utility>
#include <vector>

#include <configManager.hpp>

#include "flashWearCounter.hpp"

namespace
{
    constexpr const char *DEFAULT_WIFI_CONFIG_JSON = R"rawlite(
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

    constexpr const char *DEFAULT_STATE_CONFIG_JSON = R"rawlite(
{
  "sections": {
    "a":"application",
    "b":"input",
    "c":"display",
    "d":"setpoints",
    "e":"flags",
    "f":"testmode",
    "h":"flags",
    "i":"testmode",
    "j":"auto",
    "k":"memory"
  },
  "application" : {
    "name" : "Motor Controller",
    "version" : "5.0",
    "date" : "2025-10-14",
    "role" : "1"
  },
  "input" : {
    "type" : "touch",
    "direction" : "righty"
  },
  "display" : {
    "mode" : "day",
    "brightness" : "5",
    "offtime" : "30",
    "menubuttons" : "tru",
    "fullscreen" : "false",
    "fullscreentoggle" : "false",
    "sleep" : "false"
  },
  "setpoints" : {
    "deviceon" : "1",
    "last" : "0",
    "present" : "0",
    "idle" : "0",
    "max" : "900"
  },
  "flags" : {
    "factoryreset" : "false",
    "role" : "1",
    "ota" : "false"
  },
  "testmode" : {
    "loopback" : "false",
    "stresstest" : "false"
  },
  "autoperiod" : {
    "step1" : "5",
    "step2" : "10",
    "step3" : "15",
    "step4" : "10",
    "step5" : "5",
    "step6" : "20"
  },
  "auto" : {
    "period" : "10",
    "step1" : "120",
    "step2" : "160",
    "step3" : "220",
    "step4" : "240",
    "step5" : "320",
    "step6" : "321",
    "repeats" : "10",
    "steps" : "4"
  },
  "memory1" : {
    "address1" : "100",
    "address2" : "200",
    "address3" : "300",
    "address4" : "400",
    "address5" : "-99",
    "address6" : "-99"
  },
  "memory2" : {
    "address1" : "-99",
    "address2" : "-99",
    "address3" : "-99",
    "address4" : "-99",
    "address5" : "-99",
    "address6" : "-99"
  },
  "memory3" : {
    "address1" : "-99",
    "address2" : "-99",
    "address3" : "-99",
    "address4" : "-99",
    "address5" : "-99",
    "address6" : "-99"
  },
  "memory4" : {
    "address1" : "-99",
    "address2" : "-99",
    "address3" : "-99",
    "address4" : "-99",
    "address5" : "-99",
    "address6" : "-99"
  },
  "memory5" : {
    "address1" : "-99",
    "address2" : "-99",
    "address3" : "-99",
    "address4" : "-99",
    "address5" : "-99",
    "address6" : "-99"
  },
  "memory6" : {
    "address1" : "-99",
    "address2" : "-99",
    "address3" : "-99",
    "address4" : "-99",
    "address5" : "-99",
    "address6" : "-99"
  },
  "calibration" : {
    "offset" : "0",
    "limit" : "900",
    "stepsize" : "5",
    "scalefactor" : "1",
    "period" : "120"
  }
}
)rawlite";
}

configManager::configManager(iFileSystemProvider *fsProvider, const String &configFilePath, size_t maxConfigSize) :
    _fsProvider(fsProvider),
    _maxConfigSize(maxConfigSize),
    _configFilePath(configFilePath),
    _isConfigLoaded(false)
{
}

bool configManager::begin(String filename, bool verbose)
{
    if (filename.isEmpty())
    {
        if (verbose)
        {
            LOG_ERROR(LOG_CAT_CONFIG, "Filename cannot be empty.");
        }
        return false;
    }

    _configFilePath = filename;
    String jsonString;
    _configMap.clear();

    if (!_fsProvider || !_fsProvider->begin())
    {
        if (verbose)
        {
            LOG_ERROR(LOG_CAT_CONFIG, "Filesystem mount failed. Loading defaults...");
        }
        jsonString = loadDefaults();
    }
    else
    {
        if (verbose)
        {
            LOG_INFO(LOG_CAT_CONFIG, "Filesystem mounted");
        }

        if (!loadConfigString(filename.c_str(), &jsonString, verbose))
        {
            if (verbose)
            {
                LOG_WARN(LOG_CAT_CONFIG, "Failed to read config file. Loading defaults...");
            }
            jsonString = loadDefaults();
        }

        _fsProvider->end();
        if (verbose)
        {
            LOG_INFO(LOG_CAT_CONFIG, "Filesystem unmounted");
        }
    }

    if (verbose && !jsonString.isEmpty())
    {
        LOG_INFO(LOG_CAT_CONFIG, "Raw config string:");
        LOG_INFO(LOG_CAT_CONFIG, jsonString.c_str());
    }

    if (!jsonStringToConfig(jsonString, verbose))
    {
        if (verbose)
        {
            LOG_WARN(LOG_CAT_CONFIG, "JSON parsing failed. Loading defaults...");
        }
        jsonString = loadDefaults();
        jsonStringToConfig(jsonString, verbose);
    }

    if (verbose)
    {
        LOG_INFO(LOG_CAT_CONFIG, "Loaded %u config sections", static_cast<unsigned int>(_configMap.size()));
        printConfigToSerial();
    }

    _isConfigLoaded = !_configMap.empty();
    return _isConfigLoaded;
}

String configManager::loadDefaults() const
{
    if (_configFilePath.endsWith("savedState.json") || _configFilePath.endsWith("factoryState.json"))
    {
        return DEFAULT_STATE_CONFIG_JSON;
    }

    return DEFAULT_WIFI_CONFIG_JSON;
}

bool configManager::loadConfigString(const char *filename, String *jsonString, bool verbose)
{
    if (!filename || !jsonString)
    {
        return false;
    }

    fs::File file = _fsProvider->open(filename, "r");
    if (!file)
    {
        if (verbose)
        {
            LOG_ERROR(LOG_CAT_CONFIG, "Failed to open config file. Falling back to defaults.");
        }
        *jsonString = loadDefaults();
        return false;
    }

    *jsonString = file.readString();
    file.close();

    if (jsonString->isEmpty())
    {
        if (verbose)
        {
            LOG_WARN(LOG_CAT_CONFIG, "Config file empty.");
        }
        *jsonString = loadDefaults();
        return false;
    }

    if (verbose)
    {
        LOG_INFO(LOG_CAT_CONFIG, "Config file loaded.");
    }
    return true;
}

bool configManager::jsonStringToConfig(const String &jsonString, bool verbose)
{
    return jsonStringToMap(jsonString, _configMap, verbose);
}

bool configManager::jsonStringToMap(const String &jsonString, std::map<String, std::map<String, String>> &configMap, bool verbose)
{
    configMap.clear();

#ifdef CONFIGMGR_NATIVE
    if (jsonString.empty())
    {
        return false;
    }

    size_t pos = 0;
    while ((pos = jsonString.find('"', pos)) != std::string::npos)
    {
        size_t end = jsonString.find('"', pos + 1);
        if (end == std::string::npos)
        {
            break;
        }
        String section = jsonString.substr(pos + 1, end - pos - 1);
        pos = end + 1;
        if (section.length() && !section.startsWith("_") && section.find('.') == String::npos)
        {
            configMap.emplace(section, std::map<String, String>{});
        }
    }

    if (verbose)
    {
        LOG_INFO(LOG_CAT_CONFIG, "Naive parse produced %u sections", static_cast<unsigned int>(configMap.size()));
    }
    return true;
#else
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    if (error)
    {
        if (verbose)
        {
            LOG_ERROR(LOG_CAT_CONFIG, "deserializeJson() failed: %s", error.c_str());
        }
        return false;
    }

    JsonVariantConst rootVariant = doc.as<JsonVariantConst>();
    if (!rootVariant.is<JsonObjectConst>())
    {
        if (verbose)
        {
            LOG_WARN(LOG_CAT_CONFIG, "Root element is not an object");
        }
        return false;
    }

    JsonObjectConst root = rootVariant.as<JsonObjectConst>();
    if (verbose)
    {
        LOG_INFO(LOG_CAT_CONFIG, "JSON parsed successfully");
        LOG_INFO(LOG_CAT_CONFIG, "Root object has %u keys", static_cast<unsigned int>(root.size()));
    }

    for (JsonPairConst kv : root)
    {
        const String sectionName = kv.key().c_str();
        JsonVariantConst sectionValue = kv.value();
        if (!sectionValue.is<JsonObjectConst>())
        {
            if (verbose)
            {
                LOG_WARN(LOG_CAT_CONFIG, "Section '%s' is not an object", sectionName.c_str());
            }
            continue;
        }

        std::map<String, String> sectionMap;
    JsonObjectConst sectionObj = sectionValue.as<JsonObjectConst>();
        for (JsonPairConst sectionKv : sectionObj)
        {
            const String key = sectionKv.key().c_str();
            const String value = sectionKv.value().as<String>();
            sectionMap.emplace(key, value);
        }
        configMap.emplace(sectionName, std::move(sectionMap));
    }

    if (verbose)
    {
        LOG_INFO(LOG_CAT_CONFIG, "Parsed %u sections", static_cast<unsigned int>(configMap.size()));
    }
    return true;
#endif
}

bool configManager::saveToJson(const String &path, const std::map<String, std::map<String, String>> &configMap) const
{
    String jsonOutput = mapToJsonString(configMap);

    if (!_fsProvider || !_fsProvider->begin())
    {
        LOG_ERROR(LOG_CAT_CONFIG, "Failed to mount filesystem for writing: %s", path.c_str());
        return false;
    }

    fs::File file = _fsProvider->open(path.c_str(), "w");
    if (!file)
    {
        LOG_ERROR(LOG_CAT_CONFIG, "Failed to open file for writing: %s", path.c_str());
        _fsProvider->end();
        return false;
    }

    size_t written = file.print(jsonOutput);
    file.close();
    _fsProvider->end();

    if (written == 0)
    {
        LOG_WARN(LOG_CAT_CONFIG, "Nothing was written to %s", path.c_str());
        return false;
    }

    LOG_INFO(LOG_CAT_CONFIG, "Config saved to %s (%u bytes)", path.c_str(), static_cast<unsigned int>(written));

    // MANDATORY: Always track flash writes for device lifecycle management
    const bool wearResult = updateFlashWearCounter();
    LOG_INFO(LOG_CAT_CONFIG, "updateFlashWearCounter() returned: %s", wearResult ? "true" : "false");

    return true;
}

bool configManager::saveConfigFile(const char *filename)
{
    return filename ? saveToJson(filename, _configMap) : false;
}

String configManager::getValue(const String &section, const String &key) const
{
    auto sectionIt = _configMap.find(section);
    if (sectionIt == _configMap.end())
    {
        return "[NOT FOUND]";
    }

    auto valueIt = sectionIt->second.find(key);
    if (valueIt == sectionIt->second.end())
    {
        return "[NOT FOUND]";
    }

    return valueIt->second;
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
    LOG_INFO(LOG_CAT_CONFIG, "\n===== Configuration Map =====");
    for (const auto &section : _configMap)
    {
        LOG_INFO(LOG_CAT_CONFIG, "[%s]", section.first.c_str());
        for (const auto &field : section.second)
        {
            LOG_INFO(LOG_CAT_CONFIG, "  %s: %s", field.first.c_str(), field.second.c_str());
        }
    }
    LOG_INFO(LOG_CAT_CONFIG, "=============================");
}

String configManager::mapToJsonString(const std::map<String, std::map<String, String>> &configMap) const
{
#ifdef CONFIGMGR_NATIVE
    String out = "{\n";
    bool firstSection = true;
    for (const auto &sectionPair : configMap)
    {
        if (!firstSection)
        {
            out += ",\n";
        }
        firstSection = false;
        out += "  \"" + sectionPair.first + "\": {";

        bool firstKey = true;
        for (const auto &kv : sectionPair.second)
        {
            out += (firstKey ? "" : ",") + String("\n    \"") + kv.first + "\": \"" + kv.second + "\"";
            firstKey = false;
        }

        if (!sectionPair.second.empty())
        {
            out += "\n  ";
        }
        out += "}";
    }
    out += "\n}\n";
    return out;
#else
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    for (const auto &sectionPair : configMap)
    {
        JsonObject sectionObj = root[sectionPair.first].to<JsonObject>();
        for (const auto &kv : sectionPair.second)
        {
            sectionObj[kv.first] = kv.second;
        }
    }

    String output;
    serializeJsonPretty(doc, output);
    return output;
#endif
}

std::map<String, String> &configManager::getSection(const String &sectionName)
{
    return _configMap[sectionName];
}

const std::map<String, String> &configManager::getSection(const String &sectionName) const
{
    static const std::map<String, String> EMPTY_SECTION;
    auto it = _configMap.find(sectionName);
    return it != _configMap.end() ? it->second : EMPTY_SECTION;
}

bool configManager::parseHexStringToBytes(const String &hexInput, uint8_t *outputBuffer, size_t bufferLen) const
{
    if (!outputBuffer || bufferLen == 0)
    {
        return false;
    }

    String hex = hexInput;
    hex.replace(" ", "");
    hex.toUpperCase();

    if (hex.startsWith("0X"))
    {
        hex = hex.substring(2);
    }

    if (hex.length() != bufferLen * 2)
    {
        return false;
    }

    for (size_t i = 0; i < bufferLen; ++i)
    {
        char byteStr[3] = {hex[2 * i], hex[2 * i + 1], '\0'};
        char *end = nullptr;
        outputBuffer[i] = static_cast<uint8_t>(strtol(byteStr, &end, 16));
        if (*end != '\0')
        {
            return false;
        }
    }

    return true;
}

std::vector<String> configManager::getSections() const
{
    std::vector<String> sections;
    sections.reserve(_configMap.size());
    for (const auto &section : _configMap)
    {
        sections.push_back(section.first);
    }
    return sections;
}

std::vector<String> configManager::getFormatSections() const
{
    std::vector<String> formatSections;
    formatSections.reserve(_configMap.size());
    for (const auto &section : _configMap)
    {
        String name = section.first;
        if (name.startsWith("_"))
        {
            name.remove(0, 1);
        }
        formatSections.push_back(name);
    }
    return formatSections;
}

std::vector<String> configManager::getKeys(const String &section) const
{
    std::vector<String> keys;
    auto sectionIt = _configMap.find(section);
    if (sectionIt == _configMap.end())
    {
        return keys;
    }

    keys.reserve(sectionIt->second.size());
    for (const auto &kv : sectionIt->second)
    {
        keys.push_back(kv.first);
    }
    return keys;
}

bool configManager::saveConfig()
{
    return saveConfigFile(_configFilePath.c_str());
}

bool configManager::loadConfig()
{
    return begin(_configFilePath.c_str(), true);
}

void configManager::printHeapStatus() const
{
#if defined(ESP8266) || defined(ESP32)
    LOG_INFO(LOG_CAT_CONFIG, "[configManager] Free heap: %u bytes", ESP.getFreeHeap());
    LOG_INFO(LOG_CAT_CONFIG, "[configManager] Config memory usage: %u bytes", static_cast<unsigned int>(getConfigMemoryUsage()));
#endif
}

size_t configManager::getConfigMemoryUsage() const
{
    size_t total = 0;
    for (const auto &section : _configMap)
    {
        total += section.first.length();
        for (const auto &kv : section.second)
        {
            total += kv.first.length();
            total += kv.second.length();
        }
    }
    return total;
}

bool configManager::clearConfig()
{
    bool removed = _fsProvider ? _fsProvider->remove(_configFilePath.c_str()) : false;
    _configMap.clear();
    return removed;
}