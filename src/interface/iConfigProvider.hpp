
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
 * @file iConfigProvider.hpp
 * @brief Interface for persistent JSON configuration providers
 * @author Peter K Green (pkg40@yahoo.com)
 */

#pragma once
#include <Arduino.h>
#include <map>
#include <vector>

class iConfigProvider {

public:
    virtual ~iConfigProvider() = default;

    // Core data access
    virtual String getValue(const String& section, const String& key) const = 0;
    virtual void setValue(const String& section, const String& key, const String& value) = 0;

    // Metadata access
    virtual std::vector<String> getSections() const = 0;
    virtual std::vector<String> getSectionNames() const = 0;
    virtual std::vector<String> getFormatSections() const = 0;  // User-facing sections (no "_" prefix)
    virtual std::vector<String> getKeys(const String& section) const = 0;

    // Section map access
    virtual const std::map<String, String>& getSection(const String& sectionName) const = 0;

    // Persistence
    virtual bool saveConfig() = 0;
    virtual bool loadConfig() = 0;

    // Common convenience methods that consumers expect
    virtual String getUser() const = 0;
    virtual String getPassword() const = 0;
};
