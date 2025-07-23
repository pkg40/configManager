/*
 * True testbench for iConfigManager
 * Runs comprehensive tests for all config operations and edge cases
 */

#include <Arduino.h>
#include <iConfigManager.hpp>
#include <configManager.hpp>

void printMap(const std::map<String, std::map<String, String>>& config) {
    Serial.println("--- Config Map ---");
    for (const auto& section : config) {
        Serial.printf("[%s]\n", section.first.c_str());
        for (const auto& kv : section.second) {
            Serial.printf("  %s = %s\n", kv.first.c_str(), kv.second.c_str());
        }
    }
}

void runTestbench(iConfigManager* config) {
    Serial.println("\n=== iConfigManager Testbench ===");
    // Test: Initialization
    bool ok = config->begin("/testbench_config.json", true);
    Serial.printf("Init: %s\n", ok ? "PASS" : "FAIL");

    // Test: Set/Get/Has/Remove
    config->setValue("network", "ssid", "TestNet");
    config->setValue("network", "password", "Secret");
    Serial.println(config->hasValue("network", "ssid") ? "Has SSID: PASS" : "Has SSID: FAIL");
    Serial.println(config->getValue("network", "ssid") == "TestNet" ? "Get SSID: PASS" : "Get SSID: FAIL");
    config->removeSection("network");
    Serial.println(config->hasSection("network") ? "Remove Section: FAIL" : "Remove Section: PASS");

    // Test: Bulk operations
    std::map<String, std::map<String, String>> bulk = {
        {"wifi", { {"ssid", "BulkSSID"}, {"password", "BulkPass"} }},
        {"system", { {"mode", "test"}, {"version", "1.0"} }}
    };
    config->setConfig(bulk);
    printMap(config->getConfig());

    // Test: Serialization
    String json = config->toJsonString();
    Serial.println("JSON: " + json);
    bool fromJson = config->fromJsonString(json);
    Serial.println(fromJson ? "fromJsonString: PASS" : "fromJsonString: FAIL");

    // Test: Save/Load
    bool saved = config->save();
    Serial.println(saved ? "Save: PASS" : "Save: FAIL");
    bool loaded = config->load();
    Serial.println(loaded ? "Load: PASS" : "Load: FAIL");

    // Test: Print
    config->printToSerial();

    Serial.println("=== Testbench Complete ===");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    iConfigManager* config = new configManager();
    runTestbench(config);
}

void loop() {
    // Nothing needed for testbench
}
