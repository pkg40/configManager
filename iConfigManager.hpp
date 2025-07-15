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
#include <map>

/**
 * @brief Abstract interface for configuration management
 * 
 * Defines the contract that any configuration manager must implement.
 * This allows for different storage backends (SPIFFS, EEPROM, cloud, etc.)
 * while maintaining a consistent API.
 */
class iConfigManager {
public:
    virtual ~iConfigManager() = default;

    // Core configuration operations
    virtual bool begin(const String& filename, bool verbose = true) = 0;
    virtual bool save() = 0;
    virtual bool load() = 0;
    
    // Value access
    virtual String getValue(const String& section, const String& key) const = 0;
    virtual void setValue(const String& section, const String& key, const String& value) = 0;
    virtual bool hasValue(const String& section, const String& key) const = 0;
    
    // Section operations
    virtual std::map<String, String> getSection(const String& sectionName) const = 0;
    virtual bool hasSection(const String& sectionName) const = 0;
    virtual void removeSection(const String& sectionName) = 0;
    
    // Bulk operations
    virtual const std::map<String, std::map<String, String>>& getConfig() const = 0;
    virtual void setConfig(const std::map<String, std::map<String, String>>& config) = 0;
    
    // Serialization
    virtual String toJsonString() const = 0;
    virtual bool fromJsonString(const String& jsonString) = 0;
    
    // Utility methods
    virtual void printToSerial() const = 0;
    virtual bool isValid() const = 0;
    
    // Convenience methods for common config types
    virtual int getInt(const String& section, const String& key, int defaultValue = 0) const = 0;
    virtual float getFloat(const String& section, const String& key, float defaultValue = 0.0f) const = 0;
    virtual bool getBool(const String& section, const String& key, bool defaultValue = false) const = 0;
    
    virtual void setInt(const String& section, const String& key, int value) = 0;
    virtual void setFloat(const String& section, const String& key, float value) = 0;
    virtual void setBool(const String& section, const String& key, bool value) = 0;

    // Authentication helpers (optional - can return empty for non-auth configs)
    virtual String getUser() const { return ""; }
    virtual String getPassword() const { return ""; }
};
