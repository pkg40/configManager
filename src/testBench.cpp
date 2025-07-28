/*
 * Testbench for configManager library
 * Runs all test modules and reports results
 */

#include <Arduino.h>
#include <configManager.hpp>
#include "../test/testLib.hpp"

void setup() {
    Serial.begin(115200);
    delay(1000);
    configManager config;
    testLib::runConfigTests(&config);
    testLib::runTestbench(&config);
    testLib::testLinkage(&config);
}

void loop() {
    // Nothing needed for testbench
}
