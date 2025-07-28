/*
 * Example usage of iConfigProvider interface
 * Demonstrates basic config operations for ESP32/ESP8266
 */

#include <Arduino.h>
#include <iConfigProvider.hpp>
#include <configManager.hpp>

// Create a configManager instance (implements iConfigProvider)
iConfigProvider* config = new configManager();

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- iConfigProvider Example ---");

    // Initialize config from file
    if (config->begin("/config.json", true)) {
        Serial.println("Config loaded successfully.");
    } else {
        Serial.println("Config load failed, using defaults.");
    }

    // Set and get values
    config->setValue("wifiSTA", "ssid", "TestSSID");
    config->setValue("wifiSTA", "password", "TestPass");
    Serial.print("WiFi SSID: ");
    Serial.println(config->getValue("wifiSTA", "ssid"));
    Serial.print("WiFi Password: ");
    Serial.println(config->getValue("wifiSTA", "password"));

    // Save config to file
    if (config->save()) {
        Serial.println("Config saved.");
    } else {
        Serial.println("Config save failed.");
    }

    // Print all config to serial
    config->printToSerial();
}

void loop() {
    // Nothing needed for this example
}
