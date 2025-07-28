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

/*
 * Advanced Test Suite for configManager v2.0
 * Provides comprehensive stress testing and edge case coverage
 */

#pragma once
#include <Arduino.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>

class advancedTestSuite {
private:
    static int _testsPassed;
    static int _testsFailed;
    static int _totalTests;

public:
    static void runAdvancedTests() {
        Serial.println("\n🧪 === ADVANCED CONFIGMANAGER TEST SUITE === 🧪");
        Serial.println("Comprehensive stress testing and edge case coverage\n");
        
        _testsPassed = 0;
        _testsFailed = 0;
        _totalTests = 0;
        
        // Create filesystem provider and config manager for testing
        platformFileSystemProvider fsProvider;
        
        // Initialize filesystem provider
        fsProvider.begin();
        
        // Create config manager with filesystem provider
        configManager testConfig(&fsProvider, "/advanced_test.json");
        
        // Run all test categories
        testDataIntegrity(&testConfig);
        testLargeDatasets(&testConfig);
        testSpecialCharacters(&testConfig);
        testConcurrentOperations(&testConfig);
        testErrorHandling(&testConfig);
        testMemoryLimits(&testConfig);
        testPerformanceEdgeCases(&testConfig);
        testJSONCompliance(&testConfig);
        
        // Report results
        Serial.println("\n📊 === ADVANCED TEST RESULTS === 📊");
        Serial.printf("Total Tests: %d\n", _totalTests);
        Serial.printf("Passed: %d (%.1f%%)\n", _testsPassed, 
                     (_totalTests > 0) ? (100.0 * _testsPassed / _totalTests) : 0.0);
        Serial.printf("Failed: %d (%.1f%%)\n", _testsFailed,
                     (_totalTests > 0) ? (100.0 * _testsFailed / _totalTests) : 0.0);
        
        if (_testsFailed == 0) {
            Serial.println("🎉 ALL ADVANCED TESTS PASSED! 🎉");
        } else {
            Serial.printf("⚠️ %d advanced test(s) failed\n", _testsFailed);
        }
        Serial.println("====================================\n");
    }

private:
    static void testAssert(const String& testName, bool condition, const String& message = "") {
        _totalTests++;
        if (condition) {
            Serial.printf("✅ [PASS] %s\n", testName.c_str());
            _testsPassed++;
        } else {
            Serial.printf("❌ [FAIL] %s", testName.c_str());
            if (message.length() > 0) {
                Serial.printf(" - %s", message.c_str());
            }
            Serial.println();
            _testsFailed++;
        }
    }

    static void testDataIntegrity(configManager* config) {
        Serial.println("--- Testing Data Integrity ---");
        
        // Set up test data with various types
        config->setValue("integrity", "string", "test_value");
        config->setValue("integrity", "number", "42");
        config->setValue("integrity", "float", "3.14159");
        config->setValue("integrity", "boolean", "true");
        config->setValue("integrity", "special", "!@#$%^&*()");
        config->setValue("integrity", "unicode", "café");
        config->setValue("integrity", "empty", "");
        
        // Test immediate retrieval
        testAssert("String Value Integrity", 
                  config->getValue("integrity", "string") == "test_value");
        testAssert("Number Value Integrity", 
                  config->getValue("integrity", "number") == "42");
        testAssert("Float Value Integrity", 
                  config->getValue("integrity", "float") == "3.14159");
        testAssert("Boolean Value Integrity", 
                  config->getValue("integrity", "boolean") == "true");
        testAssert("Special Chars Integrity", 
                  config->getValue("integrity", "special") == "!@#$%^&*()");
        testAssert("Unicode Integrity", 
                  config->getValue("integrity", "unicode") == "café");
        testAssert("Empty Value Integrity", 
                  config->getValue("integrity", "empty") == "");
        
        // Test save/load cycle
        bool saved = config->saveConfig();
        testAssert("Data Save Operation", saved, "Should save successfully");
        
        // Clear and reload
        config->clearConfig();
        bool loaded = config->loadConfig();
        testAssert("Data Load Operation", loaded, "Should load successfully");
        
        // Verify data persistence
        if (loaded) {
            testAssert("String Persistence", 
                      config->getValue("integrity", "string") == "test_value");
            testAssert("Number Persistence", 
                      config->getValue("integrity", "number") == "42");
            testAssert("Special Chars Persistence", 
                      config->getValue("integrity", "special") == "!@#$%^&*()");
        }
        
        Serial.println("Data integrity tests completed.\n");
    }

    static void testLargeDatasets(configManager* config) {
        Serial.println("--- Testing Large Datasets ---");
        
        // Test large values
        String largeValue = "";
        for (int i = 0; i < 500; i++) {
            largeValue += "0123456789";
        }
        
        config->setValue("large", "big_value", largeValue);
        testAssert("Large Value Set", 
                  config->getValue("large", "big_value") == largeValue,
                  "Should handle large values (5KB)");
        
        // Test many sections
        for (int i = 0; i < 50; i++) {
            config->setValue("section" + String(i), "key", "value" + String(i));
        }
        
        auto sections = config->getSections();
        testAssert("Many Sections", sections.size() >= 50, "Should handle 50+ sections");
        
        // Test many keys per section
        for (int i = 0; i < 100; i++) {
            config->setValue("many_keys", "key" + String(i), "value" + String(i));
        }
        
        auto keys = config->getKeys("many_keys");
        testAssert("Many Keys Per Section", keys.size() >= 100, "Should handle 100+ keys per section");
        
        // Test memory usage with large dataset
        size_t memoryUsage = config->getConfigMemoryUsage();
        testAssert("Memory Usage Reasonable", memoryUsage > 0 && memoryUsage < 100000, 
                  "Memory usage should be reasonable for large dataset");
        
        Serial.println("Large dataset tests completed.\n");
    }

    static void testSpecialCharacters(configManager* config) {
        Serial.println("--- Testing Special Characters ---");
        
        // Test various special characters in keys and values
        String specialTests[] = {
            "\"quotes\"",
            "\\backslash\\",
            "\ttabs\t",
            "\nnewlines\n",
            "\rcarriage\r",
            "{json}",
            "[array]",
            "null",
            "true", 
            "false",
            "123.456",
            "special chars: àáâãäåæçèéêë",
            "emoji: 🚀🎉✅❌🧪"
        };
        
        bool allSpecialHandled = true;
        for (int i = 0; i < 13; i++) {
            String key = "special_" + String(i);
            config->setValue("special_chars", key, specialTests[i]);
            String retrieved = config->getValue("special_chars", key);
            if (retrieved != specialTests[i]) {
                allSpecialHandled = false;
                Serial.printf("Failed on: %s -> %s\n", specialTests[i].c_str(), retrieved.c_str());
                break;
            }
        }
        
        testAssert("Special Characters Handling", allSpecialHandled, 
                  "Should handle all special characters correctly");
        
        // Test special characters in section names
        config->setValue("special.section", "key", "value1");
        config->setValue("special-section", "key", "value2");
        config->setValue("special_section", "key", "value3");
        config->setValue("special section", "key", "value4");
        
        testAssert("Special Section Names", 
                  config->getValue("special.section", "key") == "value1" &&
                  config->getValue("special-section", "key") == "value2",
                  "Should handle special characters in section names");
        
        Serial.println("Special character tests completed.\n");
    }

    static void testConcurrentOperations(configManager* config) {
        Serial.println("--- Testing High-Volume Operations ---");
        
        // Simulate rapid operations
        unsigned long startTime = micros();
        for (int i = 0; i < 1000; i++) {
            config->setValue("concurrent", "key" + String(i), "value" + String(i));
        }
        unsigned long setTime = micros() - startTime;
        
        // Verify all values were set correctly
        bool allCorrect = true;
        for (int i = 0; i < 1000; i++) {
            String expected = "value" + String(i);
            String actual = config->getValue("concurrent", "key" + String(i));
            if (actual != expected) {
                allCorrect = false;
                break;
            }
        }
        testAssert("Rapid Set Operations", allCorrect, "All 1000 rapid operations should succeed");
        
        // Test retrieval performance
        startTime = micros();
        for (int i = 0; i < 1000; i++) {
            String value = config->getValue("concurrent", "key" + String(i));
        }
        unsigned long getTime = micros() - startTime;
        
        Serial.printf("Performance: 1000 sets in %lu μs, 1000 gets in %lu μs\n", setTime, getTime);
        testAssert("Performance Reasonable", setTime < 1000000 && getTime < 500000, 
                  "Operations should complete in reasonable time");
        
        Serial.println("High-volume operation tests completed.\n");
    }

    static void testErrorHandling(configManager* config) {
        Serial.println("--- Testing Error Handling ---");
        
        // Test with empty section/key names
        config->setValue("", "key", "value");
        testAssert("Empty Section Name", config->getValue("", "key") == "value");
        
        config->setValue("section", "", "value");
        testAssert("Empty Key Name", config->getValue("section", "") == "value");
        
        // Test non-existent keys
        testAssert("Non-existent Key", config->getValue("nonexistent", "key") == "[NOT FOUND]");
        testAssert("Non-existent Section", config->getValue("section", "nonexistent") == "[NOT FOUND]");
        
        // Test overwriting values
        config->setValue("overwrite", "key", "original");
        config->setValue("overwrite", "key", "modified");
        testAssert("Value Overwrite", config->getValue("overwrite", "key") == "modified");
        
        // Test case sensitivity
        config->setValue("CaseSensitive", "Key", "value1");
        config->setValue("casesensitive", "key", "value2");
        testAssert("Case Sensitivity", 
                  config->getValue("CaseSensitive", "Key") != config->getValue("casesensitive", "key"));
        
        Serial.println("Error handling tests completed.\n");
    }

    static void testMemoryLimits(configManager* config) {
        Serial.println("--- Testing Memory Management ---");
        
        // Get initial memory usage
        size_t initialMemory = config->getConfigMemoryUsage();
        testAssert("Initial Memory Usage", initialMemory >= 0, "Should report memory usage");
        
        // Add data and check memory growth
        for (int i = 0; i < 50; i++) {
            config->setValue("memory_test", "key" + String(i), "value" + String(i));
        }
        
        size_t grownMemory = config->getConfigMemoryUsage();
        testAssert("Memory Usage Growth", grownMemory > initialMemory, 
                  "Memory usage should increase with data");
        
        // Test memory usage is reasonable
        testAssert("Memory Usage Reasonable", grownMemory < 50000, 
                  "Memory usage should be reasonable (< 50KB)");
        
        // Test heap status doesn't crash
        config->printHeapStatus();
        testAssert("Heap Status Function", true, "printHeapStatus should not crash");
        
        Serial.println("Memory management tests completed.\n");
    }

    static void testPerformanceEdgeCases(configManager* config) {
        Serial.println("--- Testing Performance Edge Cases ---");
        
        // Test section enumeration with many sections
        unsigned long startTime = micros();
        auto sections = config->getSections();
        unsigned long sectionTime = micros() - startTime;
        
        testAssert("Section Enumeration Performance", sectionTime < 50000, 
                  "Section enumeration should be fast (< 50ms)");
        
        // Test key enumeration with many keys
        startTime = micros();
        if (sections.size() > 0) {
            auto keys = config->getKeys(sections[0]);
        }
        unsigned long keyTime = micros() - startTime;
        
        testAssert("Key Enumeration Performance", keyTime < 20000, 
                  "Key enumeration should be fast (< 20ms)");
        
        // Test save/load performance
        startTime = micros();
        bool saved = config->saveConfig();
        unsigned long saveTime = micros() - startTime;
        
        testAssert("Save Performance", saveTime < 100000 && saved, 
                  "Save should complete quickly (< 100ms)");
        
        Serial.println("Performance edge case tests completed.\n");
    }

    static void testJSONCompliance(configManager* config) {
        Serial.println("--- Testing JSON Compliance ---");
        
        // Test JSON-like values that should be treated as strings
        config->setValue("json_test", "json_object", "{\"key\":\"value\"}");
        config->setValue("json_test", "json_array", "[1,2,3,4,5]");
        config->setValue("json_test", "json_null", "null");
        config->setValue("json_test", "json_bool", "true");
        config->setValue("json_test", "json_number", "123.456");
        
        // Save and reload to test JSON serialization
        config->saveConfig();
        config->clearConfig();
        config->loadConfig();
        
        // Verify values are preserved as strings
        testAssert("JSON Object as String", 
                  config->getValue("json_test", "json_object") == "{\"key\":\"value\"}");
        testAssert("JSON Array as String", 
                  config->getValue("json_test", "json_array") == "[1,2,3,4,5]");
        testAssert("JSON Null as String", 
                  config->getValue("json_test", "json_null") == "null");
        testAssert("JSON Bool as String", 
                  config->getValue("json_test", "json_bool") == "true");
        testAssert("JSON Number as String", 
                  config->getValue("json_test", "json_number") == "123.456");
        
        Serial.println("JSON compliance tests completed.\n");
    }
};

// Static member definitions
int advancedTestSuite::_testsPassed = 0;
int advancedTestSuite::_testsFailed = 0;
int advancedTestSuite::_totalTests = 0;
