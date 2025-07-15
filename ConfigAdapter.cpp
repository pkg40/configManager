#include "ConfigAdapter.hpp"
#include "configManager2.h"

String ConfigAdapter::getValue(const String& section, const String& key) const {
    return _configManager->getValue(section, key);
}

void ConfigAdapter::setValue(const String& section, const String& key, const String& value) {
    _configManager->setValue(section, key, value);
}

std::vector<String> ConfigAdapter::getSections() const {
    std::vector<String> sections;
    const auto& cfg = _configManager->getConfig();
    for (const auto& pair : cfg) {
        sections.push_back(pair.first);
    }
    return sections;
}

std::vector<String> ConfigAdapter::getFormatSections() const {
    std::vector<String> sections;
    const auto& cfg = _configManager->getConfig();
    for (const auto& pair : cfg) {
        if (!pair.first.startsWith("_")) {
            sections.push_back(pair.first);
        }
    }
    return sections;
}

std::vector<String> ConfigAdapter::getKeys(const String& section) const {
    std::vector<String> keys;
    const auto& cfg = _configManager->getConfig();
    auto it = cfg.find(section);
    if (it != cfg.end()) {
        for (const auto& pair : it->second) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

bool ConfigAdapter::saveConfig() {
    return _configManager->saveConfigFile("/config.json");
}

bool ConfigAdapter::loadConfig() {
    return _configManager->begin("/config.json");
}

String ConfigAdapter::getUser() const {
    return _configManager->getUser();
}

String ConfigAdapter::getPassword() const {
    return _configManager->getPassword();
}
