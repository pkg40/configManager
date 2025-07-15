#pragma once
#include "iConfigProvider.hpp"
#include <vector>
#include <Arduino.h>

class configManager2;

/**
 * @brief Adapter that wraps configManager2 to implement iConfigProvider interface
 * 
 * This adapter allows the existing configManager2 to be used with web modules
 * that expect the iConfigProvider interface, following the adapter pattern.
 */
class ConfigAdapter : public iConfigProvider {
    configManager2* _configManager;
    
public:
    ConfigAdapter(configManager2* cm) : _configManager(cm) {}
    
    // iConfigProvider implementation
    String getValue(const String& section, const String& key) const override;
    void setValue(const String& section, const String& key, const String& value) override;
    std::vector<String> getSections() const override;
    std::vector<String> getFormatSections() const override;
    std::vector<String> getKeys(const String& section) const override;
    bool saveConfig() override;
    bool loadConfig() override;
    String getUser() const override;
    String getPassword() const override;
};
