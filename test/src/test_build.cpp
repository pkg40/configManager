/*
 * Test build for configManager v2.0
 * This file tests that the refactored library compiles correctly
 */

#include <Arduino.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>

// Global instances
platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/test_config.json", 4096);

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 5000);
    
    Serial.println("=== ConfigManager v2.0 Test Build ===");
    
    // Test basic functionality
    if (config.loadConfig()) {
        Serial.println("✅ Config loaded successfully");
    } else {
        Serial.println("⚠️ Using default configuration");
    }
    
    // Test all major API methods
    String testValue = config.getValue("test", "key");
    config.setValue("test", "key", "test_value");
    
    // Test interface methods
    auto sections = config.getSections();
    auto sectionNames = config.getSectionNames(); // Test backward compatibility
    auto keys = config.getKeys("test");
    
    // Test section access
    const auto& section = config.getSection("test");
    
    // Test convenience methods
    String user = config.getUser();
    String password = config.getPassword();
    
    // Test polymorphic usage
    iConfigProvider* provider = &config;
    String polyValue = provider->getValue("test", "key");
    
    // Test filesystem provider
    iFileSystemProvider* fs = &fsProvider;
    bool fsReady = fs->begin();
    
    Serial.println("✅ All API methods compiled successfully");
    Serial.println("✅ Test build completed");
}

void loop() {
    // Test periodic operations
    static unsigned long lastTest = 0;
    if (millis() - lastTest > 5000) {
        lastTest = millis();
        
        // Test save/load cycle
        config.setValue("runtime", "timestamp", String(millis()));
        if (config.saveConfig()) {
            Serial.println("✅ Config saved");
        }
        
        // Test memory monitoring
        config.printHeapStatus();
        Serial.printf("Config memory: %u bytes\n", config.getConfigMemoryUsage());
    }
    
    delay(100);
}
