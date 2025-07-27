/*
 * Test bed for iConfigManager interface
 * Runs basic unit tests for config operations
 */

#include <Arduino.h>
#include <iConfigManager.hpp>
#include <configManager.hpp>

// Test helper
void assertEqual(const String& label, const String& actual, const String& expected) {
    if (actual == expected) {
        Serial.printf("[PASS] %s: %s\n", label.c_str(), actual.c_str());
    } else {
        Serial.printf("[FAIL] %s: got '%s', expected '%s'\n", label.c_str(), actual.c_str(), expected.c_str());
    }
}

void runConfigTests(iConfigManager* config) {
    Serial.println("\n--- Running iConfigManager Tests ---");
    config->setValue("test", "key1", "value1");
    config->setValue("test", "key2", "value2");
    assertEqual("Set/Get key1", config->getValue("test", "key1"), "value1");
    assertEqual("Set/Get key2", config->getValue("test", "key2"), "value2");

    config->setInt("test", "intKey", 42);
    assertEqual("Set/Get intKey", String(config->getInt("test", "intKey")), "42");

    config->setBool("test", "boolKey", true);
    assertEqual("Set/Get boolKey", String(config->getBool("test", "boolKey")), "1");

    config->setFloat("test", "floatKey", 3.14f);
    assertEqual("Set/Get floatKey", String(config->getFloat("test", "floatKey")), "3.14");

    config->removeSection("test");
    assertEqual("Remove section", String(config->hasSection("test")), "0");

    Serial.println("All tests complete.");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    configManager config;
    config.begin("/test_config.json", true);
    runConfigTests(&config);
    config.save();
    config.printToSerial();
}

void loop() {
    // Nothing needed for this test
}
