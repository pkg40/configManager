#ifdef RUN_EMBEDDED_TESTS
#include <Arduino.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>
#include "../test/testLib.hpp"
#include "../test/advancedTestSuite_simple.hpp"

/*
 * Embedded Test Runner
 * Runs when built with -DRUN_EMBEDDED_TESTS (see platformio.ini env:seeed_xiao_esp32s3_tests)
 * Executes layered test strategy similar to BasePacket standard.
 */

platformFileSystemProvider _fsProvider;
configManager _config(&_fsProvider, "/embedded_test.json");

static void runLayer1Basics() {
    Serial.println("\n[L1] Basic / Interface / Polymorphic tests");
    testLib::testBasicOperations(&_config);
    testLib::testInterfaceCompliance(&_config);
    testLib::testPolymorphicUsage(&_config);
}

static void runLayer2Core() {
    Serial.println("\n[L2] Persistence / Error / Memory / Edge / Backward");
    testLib::testDataPersistence(&_config);
    testLib::testErrorHandling(&_config);
    testLib::testMemoryManagement(&_config);
    testLib::testEdgeCases(&_config);
    testLib::testBackwardCompatibility(&_config);
}

static void runLayer3Perf() {
    Serial.println("\n[L3] Performance");
    testLib::testPerformance(&_config);
}

static void runLayer4Advanced() {
    Serial.println("\n[L4] Advanced Stress & JSON Compliance");
    advancedTestSuite::runAdvancedTests();
}

void setup() {
    Serial.begin(115200);
    while(!Serial && millis() < 4000) {}
    Serial.println("\n=== configManager Embedded Test Runner ===");

    _fsProvider.begin();
    _config.loadConfig();

    testLib::startTests();
    runLayer1Basics();
    runLayer2Core();
    runLayer3Perf();
    testLib::finishTests();

    // Advanced layer executed separately to keep base stats distinct
    runLayer4Advanced();

    Serial.println("\n=== Embedded Tests Complete ===");
}

void loop() {
    // Idle - could add periodic health checks or watchdog tests here
    delay(2000);
}
#endif // RUN_EMBEDDED_TESTS
