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
 * Example: How to Run ConfigManager Tests
 * Demonstrates different ways to test the configManager library
 */

#include <Arduino.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>
#include <testLib.hpp>
#include <advancedTestSuite.hpp>

// Create filesystem provider and config manager
platformFileSystemProvider fsProvider;
configManager config(&fsProvider);

void runQuickTests() {
    Serial.println("=== Quick Test Example ===");
    
    // Initialize
    fsProvider.begin(true);
    config.begin("/quick_test.json", true);
    
    // Run basic tests only (fastest)
    testLib::runV2ConfigTests(&config);
    
    Serial.println("Quick tests completed!\n");
}

void runFullTestSuite() {
    Serial.println("=== Full Test Suite Example ===");
    
    // Initialize
    fsProvider.begin(true);
    config.begin("/full_test.json", true);
    
    // Run comprehensive tests (takes longer)
    testLib::runAllTests(&config);
    
    Serial.println("Full test suite completed!\n");
}

void runIsolatedTests() {
    Serial.println("=== Isolated Test Example ===");
    
    // Run advanced tests with mock filesystem (no hardware dependency)
    advancedTestSuite::runAdvancedTests();
    
    Serial.println("Isolated tests completed!\n");
}

void runCustomTest() {
    Serial.println("=== Custom Test Example ===");
    
    // Initialize
    fsProvider.begin(true);
    config.begin("/custom_test.json", true);
    
    // Custom test using the assertion helpers
    testLib::startTests();
    
    // Your custom tests here
    config.setValue("custom", "test1", "value1");
    config.setValue("custom", "test2", "value2");
    
    testLib::assertEqual("Custom test 1", config.getValue("custom", "test1"), "value1");
    testLib::assertEqual("Custom test 2", config.getValue("custom", "test2"), "value2");
    
    // Test save/load
    bool saved = config.saveConfig();
    testLib::assertTrue("Save config", saved);
    
    config.clearConfig();
    bool loaded = config.loadConfig();
    testLib::assertTrue("Load config", loaded);
    
    testLib::assertEqual("Persistence test", config.getValue("custom", "test1"), "value1");
    
    testLib::finishTests();
    
    Serial.println("Custom tests completed!\n");
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("ConfigManager Test Examples");
    Serial.println("==========================\n");
    
    // Choose which test to run (uncomment one):
    
    runQuickTests();        // Fast basic tests
    // runFullTestSuite();     // Comprehensive tests
    // runIsolatedTests();     // Mock filesystem tests
    // runCustomTest();        // Custom test example
    
    Serial.println("All examples completed!");
}

void loop() {
    delay(10000);
}

/*
 * Testing Best Practices:
 * 
 * 1. Quick Tests: Use for rapid development/debugging
 *    - Basic functionality verification
 *    - Fast feedback loop
 * 
 * 2. Full Test Suite: Use before releases
 *    - Comprehensive coverage
 *    - Performance benchmarks
 *    - Real-world scenarios
 * 
 * 3. Isolated Tests: Use for CI/CD
 *    - No hardware dependencies
 *    - Deterministic results
 *    - Edge case coverage
 * 
 * 4. Custom Tests: Use for specific features
 *    - Domain-specific testing
 *    - Integration testing
 *    - Regression testing
 */
