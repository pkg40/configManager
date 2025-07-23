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
