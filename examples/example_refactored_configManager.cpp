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
 * @file example_refactored_configManager.cpp
 * @brief Example showing how to use the refactored configManager with platformFileSystemProvider
 * @author Peter K Green (pkg40@yahoo.com)
 */

#include <Arduino.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>

// Global instances
platformFileSystemProvider fsProvider;
configManager config(&fsProvider);

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000); // Wait for Serial on USB boards
    
    Serial.println("\n=== Refactored ConfigManager Example ===");
    
    // Load configuration
    if (config.loadConfig()) {
        Serial.println("✅ Configuration loaded successfully");
    } else {
        Serial.println("⚠️ Using default configuration");
    }
    
    // Print current configuration
    config.printConfigToSerial();
    
    // Read some values
    Serial.println("\n--- Reading Configuration Values ---");
    String wifiSSID = config.getValue("wifiSTA", "ssid");
    String wifiPassword = config.getValue("wifiSTA", "password");
    String mqttIP = config.getValue("mqtt", "ip");
    
    Serial.printf("WiFi SSID: %s\n", wifiSSID.c_str());
    Serial.printf("WiFi Password: %s\n", wifiPassword.c_str());
    Serial.printf("MQTT IP: %s\n", mqttIP.c_str());
    
    // Modify some values
    Serial.println("\n--- Modifying Configuration ---");
    config.setValue("wifiSTA", "ssid", "MyNewNetwork");
    config.setValue("mqtt", "use", "false");
    
    // Save configuration
    if (config.saveConfig()) {
        Serial.println("✅ Configuration saved successfully");
    } else {
        Serial.println("❌ Failed to save configuration");
    }
    
    // Show sections and keys
    Serial.println("\n--- Configuration Structure ---");
    auto sections = config.getSections();
    for (const auto& sectionName : sections) {
        Serial.printf("Section: %s\n", sectionName.c_str());
        auto keys = config.getKeys(sectionName);
        for (const auto& key : keys) {
            String value = config.getValue(sectionName, key);
            Serial.printf("  %s: %s\n", key.c_str(), value.c_str());
        }
    }
    
    // Memory usage information
    Serial.println("\n--- Memory Information ---");
    config.printHeapStatus();
    Serial.printf("Config memory usage: %u bytes\n", config.getConfigMemoryUsage());
}

void loop() {
    // Example of polymorphic usage
    iConfigProvider* provider = &config;
    
    // Every 10 seconds, toggle a flag
    static unsigned long lastToggle = 0;
    if (millis() - lastToggle > 10000) {
        lastToggle = millis();
        
        String currentFlag = provider->getValue("flag", "webstart");
        String newFlag = (currentFlag == "true") ? "false" : "true";
        
        provider->setValue("flag", "webstart", newFlag);
        Serial.printf("Toggled webstart flag to: %s\n", newFlag.c_str());
        
        // Save the change
        provider->saveConfig();
    }
    
    delay(1000);
}
