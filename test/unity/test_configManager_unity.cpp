#include <Arduino.h>
#include <unity.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>

platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/unity_test.json");

void setUp(void) {
    fsProvider.begin();
    config.clearConfig();
}

void tearDown(void) {
    // currently unused
}

void test_set_and_get_value() {
    config.setValue("section", "key", "value");
    TEST_ASSERT_EQUAL_STRING("value", config.getValue("section", "key").c_str());
}

void test_overwrite_value() {
    config.setValue("section", "key", "value1");
    config.setValue("section", "key", "value2");
    TEST_ASSERT_EQUAL_STRING("value2", config.getValue("section", "key").c_str());
}

void test_persistence() {
    config.setValue("persist", "k", "v");
    config.saveConfig();
    config.clearConfig();
    config.loadConfig();
    TEST_ASSERT_EQUAL_STRING("v", config.getValue("persist", "k").c_str());
}

// setup() and loop() provided by src/unityRunner.cpp when UNITY_EMBEDDED is defined
