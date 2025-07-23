# ESP8266 Compatibility Guide - configManager

## Overview
The configManager library is fully compatible with ESP8266 with some platform-specific considerations.

## Platform Differences

### Filesystem Initialization
- **ESP32**: `SPIFFS.begin(true)` - auto-formats if corrupted
- **ESP8266**: `SPIFFS.begin()` - no auto-format, manual intervention required

### Memory Constraints
| Platform | Heap Memory | Recommended Config Size | Max Tested |
|----------|-------------|------------------------|------------|
| ESP32    | ~300KB      | 32KB+                  | 64KB+      |
| ESP8266  | ~80KB       | 8KB                    | 16KB       |

### Filesystem Options
- **SPIFFS**: Available on both platforms
- **LittleFS**: ESP8266 only, more efficient and reliable
  - Enable with: `-DUSE_LITTLEFS` build flag

## Configuration for ESP8266

### platformio.ini Example
```ini
[env:esp8266]
platform = espressif8266
board = nodemcuv2
framework = arduino
build_flags = 
    -DUSE_LITTLEFS          ; Use LittleFS instead of SPIFFS
    -DCONFIG_MAX_SIZE=8192  ; Limit config size for memory safety
lib_deps = 
    bblanchon/ArduinoJson@^7.0.0
    abstractClasses
```

### Memory Management Tips
1. **Keep configs small** - Under 8KB recommended
2. **Use selective loading** - Only load needed sections
3. **Monitor heap usage** - Use `ESP.getFreeHeap()` for debugging
4. **Avoid deep nesting** - Flat JSON structures use less memory

### Example Usage
```cpp
#include <configManager.hpp>

configManager config;

void setup() {
    Serial.begin(115200);
    
    // Monitor free heap before config loading
    Serial.printf("Free heap before: %u\n", ESP.getFreeHeap());
    
    if (config.loadConfig()) {
        Serial.println("Config loaded successfully");
    } else {
        Serial.println("Using default config");
    }
    
    // Monitor free heap after config loading
    Serial.printf("Free heap after: %u\n", ESP.getFreeHeap());
}
```

## Known Issues & Workarounds

### 1. Out of Memory Errors
**Symptom**: Random crashes, WDT resets during config loading
**Solution**: 
- Reduce config file size
- Split large configs into multiple files
- Use String references instead of copies where possible

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

1. **Cache frequently used values** - Avoid repeated JSON lookups
2. **Use const references** - Minimize String copying
3. **Implement config validation** - Verify critical settings on load
4. **Provide fallback defaults** - Always have working defaults
5. **Monitor system health** - Track heap, filesystem usage
