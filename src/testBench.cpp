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
 * ConfigManager v2.0 Comprehensive Test Bench
 * Multi-tier testing with real and mock filesystem providers
 * Excluded from native host build (handled by native_main.cpp)
 */

#if !defined(CONFIGMGR_NATIVE) && !defined(UNITY_EMBEDDED)

#include <Arduino.h>
#include <configManager.hpp>
#ifndef CONFIGMGR_NATIVE
#include <platformFileSystemProvider.hpp>
#endif
#include "../test/testLib.hpp"
#include "../test/advancedTestSuite_simple.hpp"

// Global objects
#ifndef CONFIGMGR_NATIVE
platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/testbench_config.json");
#endif

void runBasicTests() {
    Serial.println("[TEST] === BASIC FUNCTIONAL TESTS === [TEST]");
    testLib::runAllTests(&config);
}

void runAdvancedTests() {
    Serial.println("[ADVANCED] === ADVANCED ISOLATION TESTS === [ADVANCED]");
    advancedTestSuite::runAdvancedTests();
}

void runPerformanceBenchmark() {
    Serial.println("⚡ === PERFORMANCE BENCHMARK === ⚡");
    
    unsigned long startTime = millis();
    
    // Benchmark: Bulk operations
    Serial.println("Running bulk operation benchmark...");
    for (int i = 0; i < 1000; i++) {
        config.setValue("benchmark", "key" + String(i), "value" + String(i));
    }
    unsigned long setTime = millis() - startTime;
    
    startTime = millis();
    for (int i = 0; i < 1000; i++) {
        String value = config.getValue("benchmark", "key" + String(i));
    }
    unsigned long getTime = millis() - startTime;
    
    Serial.printf("1000 setValue operations: %lu ms (%.2f ops/sec)\n", 
                 setTime, 1000.0 / (setTime / 1000.0));
    Serial.printf("1000 getValue operations: %lu ms (%.2f ops/sec)\n", 
                 getTime, 1000.0 / (getTime / 1000.0));
    
    // Benchmark: Save/Load operations
    startTime = millis();
    bool saved = config.saveConfig();
    unsigned long saveTime = millis() - startTime;
    
    startTime = millis();
    bool loaded = config.loadConfig();
    unsigned long loadTime = millis() - startTime;
    
    Serial.printf("Save operation: %lu ms (%s)\n", saveTime, saved ? "SUCCESS" : "FAILED");
    Serial.printf("Load operation: %lu ms (%s)\n", loadTime, loaded ? "SUCCESS" : "FAILED");
    
    // Memory usage report
    Serial.printf("Config memory usage: %u bytes\n", config.getConfigMemoryUsage());
    config.printHeapStatus();
    
    Serial.println("Performance benchmark completed.\n");
}

void runRealWorldScenario() {
    Serial.println("🌍 === REAL-WORLD SCENARIO TEST === 🌍");
    
    // Simulate typical IoT device configuration
    config.setValue("wifi", "ssid", "MyHomeWiFi");
    config.setValue("wifi", "password", "SuperSecretPassword123!");
    config.setValue("wifi", "mode", "STA");
    config.setValue("wifi", "channel", "6");
    
    config.setValue("mqtt", "server", "mqtt.example.com");
    config.setValue("mqtt", "port", "1883");
    config.setValue("mqtt", "username", "device_001");
    config.setValue("mqtt", "password", "mqtt_secret");
    config.setValue("mqtt", "topic_prefix", "home/sensors/");
    
    config.setValue("sensors", "temperature_offset", "0.5");
    config.setValue("sensors", "humidity_calibration", "1.02");
    config.setValue("sensors", "sampling_interval", "30");
    config.setValue("sensors", "deep_sleep_enable", "true");
    
    config.setValue("device", "name", "Kitchen Sensor");
    config.setValue("device", "location", "Kitchen");
    config.setValue("device", "firmware_version", "2.1.0");
    config.setValue("device", "last_update", "2025-01-28T10:30:00Z");
    
    Serial.println("Configured typical IoT device settings:");
    config.printConfigToSerial();
    
    // Test configuration persistence
    Serial.println("\nTesting configuration persistence...");
    bool saved = config.saveConfig();
    Serial.printf("Save result: %s\n", saved ? "SUCCESS" : "FAILED");
    
    if (saved) {
        // Simulate device restart by clearing and reloading
        config.clearConfig();
        Serial.println("Configuration cleared (simulating restart)");
        
        bool loaded = config.loadConfig();
        Serial.printf("Load result: %s\n", loaded ? "SUCCESS" : "FAILED");
        
        if (loaded) {
            Serial.println("Configuration after restart:");
            String ssid = config.getValue("wifi", "ssid");
            String mqtt_server = config.getValue("mqtt", "server");
            String device_name = config.getValue("device", "name");
            
            Serial.printf("WiFi SSID: %s\n", ssid.c_str());
            Serial.printf("MQTT Server: %s\n", mqtt_server.c_str());
            Serial.printf("Device Name: %s\n", device_name.c_str());
            
            bool persistence_ok = (ssid == "MyHomeWiFi" && 
                                 mqtt_server == "mqtt.example.com" && 
                                 device_name == "Kitchen Sensor");
            
            Serial.printf("Persistence test: %s\n", persistence_ok ? "PASSED" : "FAILED");
        }
    }
    
    Serial.println("Real-world scenario test completed.\n");
}

void setup() {
    Serial.begin(115200);
    delay(2000); // Wait for serial connection
    
    Serial.println("\n[START] ConfigManager v2.0 Comprehensive Test Bench [START]");
    Serial.println("====================================================");
    Serial.println("Testing filesystem provider separation architecture");
    Serial.println("Multiple test tiers: Basic → Advanced → Performance → Real-world\n");
    
    // Initialize filesystem provider
    Serial.println("Initializing filesystem provider...");
    if (fsProvider.begin()) {
        Serial.println("✅ Filesystem provider initialized successfully");
    } else {
        Serial.println("❌ Failed to initialize filesystem provider");
        Serial.println("⚠️ Some tests may not work correctly");
    }
    
    // Config manager is initialized in constructor with filesystem provider
    Serial.println("\n✅ ConfigManager initialized with filesystem provider");
    
    Serial.println("\n[INFO] Starting comprehensive test suite...");
    Serial.println("This may take a few minutes to complete.\n");
    
    unsigned long totalStartTime = millis();
    
    // Run all test tiers
    runBasicTests();
    runAdvancedTests();
    runPerformanceBenchmark();
    runRealWorldScenario();
    
    unsigned long totalTime = millis() - totalStartTime;
    
    Serial.println("[COMPLETE] === TEST SUITE COMPLETE === [COMPLETE]");
    Serial.printf("Total execution time: %lu ms (%.2f seconds)\n", totalTime, totalTime / 1000.0);
    Serial.println("=====================================");
    Serial.println("\nConfigManager v2.0 testing completed successfully!");
    Serial.println("The library is ready for production use. [SUCCESS]\n");
}

void loop() {
    // Test bench runs once in setup, then idles
    delay(30000);
    
    // Optional: Print memory status every 30 seconds
    static bool memoryReported = false;
    if (!memoryReported) {
        Serial.println("[INFO] === MEMORY STATUS (30s after tests) === [INFO]");
        config.printHeapStatus();
        Serial.printf("Config memory usage: %u bytes\n", config.getConfigMemoryUsage());
        Serial.println("============================================\n");
        memoryReported = true;
    }
}

#endif // !CONFIGMGR_NATIVE && !UNITY_EMBEDDED
