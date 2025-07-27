# ESP8266 Compatibility Guide - configManager

> **Note:** The `configManager` library fully encapsulates all filesystem and JSON handling. Users do not need to interact with SPIFFS, LittleFS, or ArduinoJson directly. All configuration is managed through the `configManager` API.

## Overview
The configManager library is fully compatible with ESP8266 with some platform-specific considerations.

## Platform Differences

### Platform Flags
- **ESP8266**: It is strongly recommended to use LittleFS for reliability and performance. Enable with the `-DUSE_LITTLEFS` build flag in your `platformio.ini`.

### Memory Constraints
- **ESP8266**: Keep config files under 8KB for best stability. The library will warn or fail gracefully if memory is insufficient.

## Configuration for ESP8266

### platformio.ini Example
```ini
[env:esp8266]
platform = espressif8266
board = nodemcuv2
framework = arduino
build_flags = 
    -DUSE_LITTLEFS          ; Strongly recommended for ESP8266
    -DCONFIG_MAX_SIZE=8192  ; Limit config size for memory safety
lib_deps = 
    abstractClasses
```

### Memory Management Tips
1. **Keep configs small** - Under 8KB recommended for ESP8266.
2. **Monitor configManager memory usage** - Use the new `getConfigMemoryUsage()` and `printHeapStatus()` methods for diagnostics (see below).
3. **Emergency config clear** - Use `configManager::clearConfig()` to erase the config file if needed (see below).
4. **No manual FS or JSON handling required** - All config operations are managed by the library.

### Example Usage
```cpp
#include <configManager.hpp>

configManager config;

void setup() {
    Serial.begin(115200);
    // Print heap status before loading config
    config.printHeapStatus();

    if (config.loadConfig()) {
        Serial.println("Config loaded successfully");
    } else {
        Serial.println("Using default config");
    }

    // Print heap status after loading config
    config.printHeapStatus();
    Serial.printf("Config memory usage: %u bytes\n", config.getConfigMemoryUsage());
}

void loop() {
    // Example: Emergency config clear via serial command
    if (Serial.available() && Serial.readStringUntil('\n') == "CLEAR_CONFIG") {
        config.clearConfig();
        Serial.println("Config cleared. Rebooting...");
        ESP.restart();
    }
}
```

## Known Issues & Workarounds

### 1. Out of Memory Errors
**Symptom**: Random crashes, WDT resets during config loading
**Solution**: 
- Reduce config file size
- Split large configs into multiple files
- Use String references instead of copies where possible

### 1a. Emergency Config Clear
**Symptom**: Device cannot boot due to corrupted or oversized config
**Solution**:
- Use `config.clearConfig()` to erase the config file and reset to defaults. This method is safe and encapsulated; no need to access the filesystem directly.

### 2. SPIFFS Corruption
**Symptom**: SPIFFS.begin() fails, config loading fails
**Solution**: 
- Use LittleFS instead of SPIFFS
- Implement manual SPIFFS.format() recovery

### 3. JSON Parsing Failures
**Symptom**: Config parsing returns empty, JSON errors
**Solution**:
- Validate JSON format with online tools
- Check for memory exhaustion during parsing
- Use `JsonDocument` instead of `DynamicJsonDocument` for fixed sizes

## Testing Recommendations

1. **Test with real hardware** - ESP8266 emulators don't accurately reflect memory constraints
2. **Stress test memory** - Load large configs repeatedly
3. **Test filesystem recovery** - Simulate corrupted filesystems
4. **Monitor heap fragmentation** - Long-running tests with config reloading

## Migration from ESP32

When porting from ESP32 to ESP8266:

1. **Reduce config size** - Aim for <8KB total
2. **Enable LittleFS** - Add `-DUSE_LITTLEFS` build flag
3. **Add heap monitoring** - Track memory usage during development
4. **Test thoroughly** - ESP8266 constraints require careful validation

## Performance Comparison

| Operation | ESP32 (240MHz) | ESP8266 (80MHz) | Notes |
|-----------|----------------|-----------------|-------|
| Config Load (4KB) | ~50ms | ~150ms | Including JSON parsing |
| Config Save (4KB) | ~30ms | ~100ms | Including filesystem write |
| JSON Parse (4KB) | ~20ms | ~80ms | ArduinoJson processing |
| SPIFFS Mount | ~10ms | ~50ms | Filesystem initialization |

## Best Practices

1. **Access all config through configManager** - Do not manipulate the filesystem or JSON directly.
2. **Use provided API for memory and config management** - See `getConfigMemoryUsage()`, `printHeapStatus()`, and `clearConfig()`.
3. **Implement config validation** - Verify critical settings on load using your own logic after `loadConfig()`.
4. **Provide fallback defaults** - Always have working defaults in your application code.
5. **Monitor system health** - Use the configManager API for diagnostics.
