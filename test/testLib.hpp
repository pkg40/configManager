

/*
 * Library of configManager test functions
 * Provides reusable test routines for testbenches
 */


#pragma once
#include <Arduino.h>
#include <configManager.hpp>


class testLib {
public:
    static void testLinkage(configManager* config) {
        int result = config->testLinkage(10);
        Serial.printf("testLinkage(10) = %d\n", result);
    }
    static void assertEqual(const String& label, const String& actual, const String& expected) {
        if (actual == expected) {
            Serial.printf("[PASS] %s: %s\n", label.c_str(), actual.c_str());
        } else {
            Serial.printf("[FAIL] %s: got '%s', expected '%s'\n", label.c_str(), actual.c_str(), expected.c_str());
        }
    }

    static void printMap(const std::map<String, std::map<String, String>>& config) {
        Serial.println("--- Config Map ---");
        for (const auto& section : config) {
            Serial.printf("[%s]\n", section.first.c_str());
            for (const auto& kv : section.second) {
                Serial.printf("  %s = %s\n", kv.first.c_str(), kv.second.c_str());
            }
        }
    }

    static void runConfigTests(configManager* config) {
        Serial.println("\n--- Running configManager Tests ---");
        config->setValue("test", "key1", "value1");
        config->setValue("test", "key2", "value2");
        assertEqual("Set/Get key1", config->getValue("test", "key1"), "value1");
        assertEqual("Set/Get key2", config->getValue("test", "key2"), "value2");
        Serial.println("All tests complete.");
    }

    static void runTestbench(configManager* config) {
        Serial.println("\n=== configManager Testbench ===");
        // Test: Initialization (if begin is accessible)
        // bool ok = config->begin("/testbench_config.json", true);
        // Serial.printf("Init: %s\n", ok ? "PASS" : "FAIL");

        // Test: Set/Get/Remove
        config->setValue("network", "ssid", "TestNet");
        config->setValue("network", "password", "Secret");
        // getSection is public, so we can check if the section exists
        auto& section = config->getSection("network");
        assertEqual("Get SSID", section["ssid"], "TestNet");
        // Remove section by clearing map (if removeSection is not available)
        config->getSection("network").clear();
        assertEqual("Remove Section", String(section.empty()), "1");

        // Test: Bulk operations (manually set _configMap)
        std::map<String, std::map<String, String>> bulk = {
            {"wifi", { {"ssid", "BulkSSID"}, {"password", "BulkPass"} }},
            {"system", { {"mode", "test"}, {"version", "1.0"} }}
        };
        config->_configMap = bulk;
        printMap(config->_configMap);

        // Test: Serialization (if mapToJsonString is public)
        String json = config->mapToJsonString(config->_configMap);
        Serial.println("JSON: " + json);

        // Test: Save/Load (if saveConfig/loadConfig are public)
        bool saved = config->saveConfig();
        Serial.println(saved ? "Save: PASS" : "Save: FAIL");
        bool loaded = config->loadConfig();
        Serial.println(loaded ? "Load: PASS" : "Load: FAIL");

        // Test: Print
        config->printConfigToSerial();

        Serial.println("=== Testbench Complete ===");
    }
};
