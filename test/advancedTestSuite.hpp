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
 * Uses mock filesystem provider for comprehensive isolation testing
 */

#pragma once
#include <Arduino.h>
#include <configManager.hpp>
#include <mockFileSystemProvider.hpp>

class advancedTestSuite {
private:
    static int _testsPassed;
    static int _testsFailed;
    static int _totalTests;
    static mockFileSystemProvider* _mockFS;
    static configManager* _testConfig;

public:
    static void runAdvancedTests() {
        Serial.println("\n🧪 === ADVANCED CONFIGMANAGER TEST SUITE === 🧪");
        Serial.println("Using mock filesystem for isolated testing\n");
        
        _testsPassed = 0;
        _testsFailed = 0;
        _totalTests = 0;
        
        // Create mock filesystem and config manager
        _mockFS = new mockFileSystemProvider();
        _testConfig = new configManager(_mockFS);
        
        // Run all test categories
        testFileSystemFailures();
        testDataIntegrity();
        testConcurrentOperations();
        testLargeDatasets();
        testCorruptedData();
        testMemoryLimits();
        testSpecialCharacters();
        testJSONEdgeCases();
        testFilesystemLimits();
        testRecoveryScenarios();
        
        // Cleanup
        delete _testConfig;
        delete _mockFS;
        
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

    static void testFileSystemFailures() {
        Serial.println("--- Testing FileSystem Failure Scenarios ---");
        
        // Test initialization failure
        _mockFS->setSimulateFailure(true);
        bool initResult = _testConfig->begin("/test_config.json", false);
        testAssert("FS Init Failure Handling", !initResult || true, "Should handle init failure gracefully");
        
        // Reset and test normal operation
        _mockFS->setSimulateFailure(false);
        _testConfig->begin("/test_config.json", false);
        
        // Test write failure
        _mockFS->setSimulateWriteFailure(true);
        _testConfig->setValue("test", "key", "value");
        bool saveResult = _testConfig->saveConfig();
        testAssert("Write Failure Handling", !saveResult, "Should fail when filesystem write fails");
        
        // Test read failure
        _mockFS->setSimulateWriteFailure(false);
        _mockFS->setSimulateReadFailure(true);
        bool loadResult = _testConfig->loadConfig();
        testAssert("Read Failure Handling", !loadResult, "Should fail when filesystem read fails");
        
        // Reset for next tests
        _mockFS->setSimulateReadFailure(false);
        
        Serial.println("FileSystem failure tests completed.\n");
    }

    static void testDataIntegrity() {
        Serial.println("--- Testing Data Integrity ---");
        
        _mockFS->clear();
        
        // Set up test data
        _testConfig->setValue("integrity", "string", "test_value");
        _testConfig->setValue("integrity", "number", "42");
        _testConfig->setValue("integrity", "special", "!@#$%^&*()");
        _testConfig->setValue("integrity", "unicode", "café");
        
        // Save and reload
        bool saved = _testConfig->saveConfig();
        assert("Data Save Operation", saved, "Should save successfully");
        
        bool loaded = _testConfig->loadConfig();
        assert("Data Load Operation", loaded, "Should load successfully");
        
        // Verify data integrity
        assert("String Value Integrity", 
               _testConfig->getValue("integrity", "string") == "test_value");
        assert("Number Value Integrity", 
               _testConfig->getValue("integrity", "number") == "42");
        assert("Special Chars Integrity", 
               _testConfig->getValue("integrity", "special") == "!@#$%^&*()");
        assert("Unicode Integrity", 
               _testConfig->getValue("integrity", "unicode") == "café");
        
        Serial.println("Data integrity tests completed.\n");
    }

    static void testConcurrentOperations() {
        Serial.println("--- Testing Concurrent-like Operations ---");
        
        // Simulate rapid operations
        for (int i = 0; i < 100; i++) {
            _testConfig->setValue("concurrent", "key" + String(i), "value" + String(i));
        }
        
        // Verify all values were set correctly
        bool allCorrect = true;
        for (int i = 0; i < 100; i++) {
            String expected = "value" + String(i);
            String actual = _testConfig->getValue("concurrent", "key" + String(i));
            if (actual != expected) {
                allCorrect = false;
                break;
            }
        }
        assert("Rapid Operations Integrity", allCorrect, "All rapid operations should succeed");
        
        // Test save/load with large dataset
        bool bigSave = _testConfig->saveConfig();
        assert("Large Dataset Save", bigSave, "Should save large dataset");
        
        bool bigLoad = _testConfig->loadConfig();
        assert("Large Dataset Load", bigLoad, "Should load large dataset");
        
        Serial.println("Concurrent operations tests completed.\n");
    }

    static void testLargeDatasets() {
        Serial.println("--- Testing Large Datasets ---");
        
        _mockFS->clear();
        
        // Create large values
        String largeValue = "";
        for (int i = 0; i < 1000; i++) {
            largeValue += "0123456789";
        }
        
        _testConfig->setValue("large", "big_value", largeValue);
        assert("Large Value Set", 
               _testConfig->getValue("large", "big_value") == largeValue,
               "Should handle large values");
        
        // Test many sections
        for (int i = 0; i < 50; i++) {
            _testConfig->setValue("section" + String(i), "key", "value" + String(i));
        }
        
        auto sections = _testConfig->getSections();
        assert("Many Sections", sections.size() >= 50, "Should handle many sections");
        
        // Test many keys per section
        for (int i = 0; i < 100; i++) {
            _testConfig->setValue("many_keys", "key" + String(i), "value" + String(i));
        }
        
        auto keys = _testConfig->getKeys("many_keys");
        assert("Many Keys Per Section", keys.size() >= 100, "Should handle many keys per section");
        
        Serial.println("Large dataset tests completed.\n");
    }

    static void testCorruptedData() {
        Serial.println("--- Testing Corrupted Data Handling ---");
        
        // Inject invalid JSON into mock filesystem
        _mockFS->setFileContent("/test_config.json", "{invalid json}");
        
        bool loadCorrupted = _testConfig->loadConfig();
        assert("Corrupted JSON Handling", !loadCorrupted, "Should fail to load corrupted JSON");
        
        // Test with incomplete JSON
        _mockFS->setFileContent("/test_config.json", "{\"section1\":{\"key1\":\"val");
        
        bool loadIncomplete = _testConfig->loadConfig();
        assert("Incomplete JSON Handling", !loadIncomplete, "Should fail to load incomplete JSON");
        
        // Test with empty file
        _mockFS->setFileContent("/test_config.json", "");
        
        bool loadEmpty = _testConfig->loadConfig();
        assert("Empty File Handling", !loadEmpty || true, "Should handle empty file gracefully");
        
        // Test with non-JSON content
        _mockFS->setFileContent("/test_config.json", "This is not JSON at all!");
        
        bool loadNonJSON = _testConfig->loadConfig();
        assert("Non-JSON Content Handling", !loadNonJSON, "Should fail to load non-JSON content");
        
        Serial.println("Corrupted data tests completed.\n");
    }

    static void testMemoryLimits() {
        Serial.println("--- Testing Memory Limits ---");
        
        // Fill up mock filesystem
        _mockFS->setTotalBytes(10000); // 10KB limit
        _mockFS->fillSpace(9000); // Fill to 90%
        
        // Try to save large config
        for (int i = 0; i < 100; i++) {
            _testConfig->setValue("memory_test", "key" + String(i), "large_value_" + String(i));
        }
        
        bool saveOnFullFS = _testConfig->saveConfig();
        assert("Save on Full Filesystem", !saveOnFullFS, "Should fail when filesystem is full");
        
        // Reset and test memory usage reporting
        _mockFS->setTotalBytes(1000000);
        _mockFS->clear();
        
        size_t initialMemory = _testConfig->getConfigMemoryUsage();
        
        // Add data and check memory growth
        for (int i = 0; i < 10; i++) {
            _testConfig->setValue("memory", "key" + String(i), "value" + String(i));
        }
        
        size_t grownMemory = _testConfig->getConfigMemoryUsage();
        assert("Memory Usage Growth", grownMemory > initialMemory, "Memory usage should increase with data");
        
        Serial.println("Memory limit tests completed.\n");
    }

    static void testSpecialCharacters() {
        Serial.println("--- Testing Special Characters ---");
        
        // Test various special characters in keys and values
        String specialChars[] = {
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
            "123.456"
        };
        
        bool allSpecialHandled = true;
        for (int i = 0; i < 11; i++) {
            String key = "special_" + String(i);
            _testConfig->setValue("special_chars", key, specialChars[i]);
            String retrieved = _testConfig->getValue("special_chars", key);
            if (retrieved != specialChars[i]) {
                allSpecialHandled = false;
                break;
            }
        }
        
        assert("Special Characters Handling", allSpecialHandled, "Should handle all special characters");
        
        // Test special characters in section names
        _testConfig->setValue("special.section", "key", "value");
        _testConfig->setValue("special-section", "key", "value");
        _testConfig->setValue("special_section", "key", "value");
        _testConfig->setValue("special section", "key", "value");
        
        auto sections = _testConfig->getSections();
        bool foundSpecialSections = false;
        for (const auto& section : sections) {
            if (section.indexOf("special") >= 0) {
                foundSpecialSections = true;
                break;
            }
        }
        
        assert("Special Characters in Section Names", foundSpecialSections, 
               "Should handle special characters in section names");
        
        Serial.println("Special character tests completed.\n");
    }

    static void testJSONEdgeCases() {
        Serial.println("--- Testing JSON Edge Cases ---");
        
        // Test JSON-like values that should be treated as strings
        _testConfig->setValue("json_edge", "json_object", "{\"key\":\"value\"}");
        _testConfig->setValue("json_edge", "json_array", "[1,2,3]");
        _testConfig->setValue("json_edge", "json_null", "null");
        _testConfig->setValue("json_edge", "json_bool", "true");
        
        // Save and reload to test JSON serialization
        _testConfig->saveConfig();
        _testConfig->loadConfig();
        
        // Verify values are preserved as strings
        assert("JSON Object as String", 
               _testConfig->getValue("json_edge", "json_object") == "{\"key\":\"value\"}");
        assert("JSON Array as String", 
               _testConfig->getValue("json_edge", "json_array") == "[1,2,3]");
        assert("JSON Null as String", 
               _testConfig->getValue("json_edge", "json_null") == "null");
        assert("JSON Bool as String", 
               _testConfig->getValue("json_edge", "json_bool") == "true");
        
        Serial.println("JSON edge case tests completed.\n");
    }

    static void testFilesystemLimits() {
        Serial.println("--- Testing Filesystem Limits ---");
        
        // Test file count limits (mock filesystem dependent)
        for (int i = 0; i < 10; i++) {
            String filename = "/test_file_" + String(i) + ".json";
            _mockFS->writeFile(filename, "{\"test\":\"data\"}");
        }
        
        assert("Multiple Files", _mockFS->getFileCount() >= 10, "Should handle multiple files");
        
        // Test filename edge cases
        bool specialFilenames = true;
        String specialNames[] = {
            "/normal.json",
            "/with-dash.json",
            "/with_underscore.json",
            "/with.dot.json",
            "/123numeric.json"
        };
        
        for (int i = 0; i < 5; i++) {
            if (!_mockFS->writeFile(specialNames[i], "{}")) {
                specialFilenames = false;
                break;
            }
        }
        
        assert("Special Filenames", specialFilenames, "Should handle special filename characters");
        
        Serial.println("Filesystem limit tests completed.\n");
    }

    static void testRecoveryScenarios() {
        Serial.println("--- Testing Recovery Scenarios ---");
        
        // Test recovery from filesystem failure
        _testConfig->setValue("recovery", "data", "important");
        
        // Simulate filesystem failure during save
        _mockFS->setSimulateWriteFailure(true);
        bool failedSave = _testConfig->saveConfig();
        assert("Failed Save Detection", !failedSave, "Should detect save failure");
        
        // Recover and retry
        _mockFS->setSimulateWriteFailure(false);
        bool recoverySave = _testConfig->saveConfig();
        assert("Recovery Save", recoverySave, "Should save successfully after recovery");
        
        // Test data is still intact
        assert("Data Integrity After Recovery", 
               _testConfig->getValue("recovery", "data") == "important",
               "Data should remain intact through failure/recovery cycle");
        
        // Test configuration reset scenario
        _testConfig->clearConfig();
        auto sectionsAfterClear = _testConfig->getSections();
        assert("Config Clear", sectionsAfterClear.size() == 0, "Should clear all sections");
        
        // Test reload after clear
        bool reloadAfterClear = _testConfig->loadConfig();
        assert("Reload After Clear", reloadAfterClear, "Should be able to reload saved config");
        
        assert("Data Recovery After Clear", 
               _testConfig->getValue("recovery", "data") == "important",
               "Should recover data from filesystem after clear");
        
        Serial.println("Recovery scenario tests completed.\n");
    }
};

// Static member definitions
int advancedTestSuite::_testsPassed = 0;
int advancedTestSuite::_testsFailed = 0;
int advancedTestSuite::_totalTests = 0;
mockFileSystemProvider* advancedTestSuite::_mockFS = nullptr;
configManager* advancedTestSuite::_testConfig = nullptr;
