# ConfigManager v2.0 Upgrade Guide

## Overview
Version 2.0 of the ConfigManager library introduces a separation of concerns by splitting file system operations from configuration management. This improves testability, flexibility, and adherence to SOLID principles.

## Breaking Changes

### 1. Constructor Changes
**Before (v1.x):**
```cpp
#include <configManager.hpp>
configManager config("/config.json", 8192);
```

**After (v2.0):**
```cpp
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>

platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/config.json", 8192);
```

### 2. Interface Changes
The `iConfigProvider` interface has been streamlined:
- Removed duplicate `getSectionNames()` method (use `getSections()` instead)
- All other methods remain the same

### 3. Platform-Specific Includes
**Before (v1.x):**
```cpp
// configManager.hpp included platform-specific filesystem headers
#include <configManager.hpp>
// Platform detection was automatic
```

**After (v2.0):**
```cpp
// Platform-specific code moved to platformFileSystemProvider
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>
```

## Migration Steps

### Step 1: Update Includes
Add the filesystem provider include:
```cpp
#include <platformFileSystemProvider.hpp>
```

### Step 2: Create File System Provider
Before creating the configManager:
```cpp
platformFileSystemProvider fsProvider;
```

### Step 3: Update Constructor Call
Pass the filesystem provider to the configManager:
```cpp
// Old way
configManager config("/config.json");

// New way
configManager config(&fsProvider, "/config.json");
```

### Step 4: Update Interface Usage (if applicable)
If you were using `getSectionNames()`, change to `getSections()`:
```cpp
// Old way
auto sections = provider->getSectionNames();

// New way
auto sections = provider->getSections();
```

## Complete Migration Example

### Before (v1.x):
```cpp
#include <Arduino.h>
#include <configManager.hpp>

configManager config("/config.json");

void setup() {
    Serial.begin(115200);
    
    if (config.loadConfig()) {
        Serial.println("Config loaded");
    }
    
    String ssid = config.getValue("wifi", "ssid");
    config.setValue("wifi", "ssid", "NewNetwork");
    config.saveConfig();
}
```

### After (v2.0):
```cpp
#include <Arduino.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>

platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/config.json");

void setup() {
    Serial.begin(115200);
    
    if (config.loadConfig()) {
        Serial.println("Config loaded");
    }
    
    String ssid = config.getValue("wifi", "ssid");
    config.setValue("wifi", "ssid", "NewNetwork");
    config.saveConfig();
}
```

## Benefits of the New Architecture

### 1. Testability
You can now inject a mock filesystem provider for unit testing:
```cpp
class MockFileSystemProvider : public iFileSystemProvider {
    // Implementation for testing
};

MockFileSystemProvider mockFS;
configManager config(&mockFS);
// Now you can test without real filesystem operations
```

### 2. Flexibility
Different filesystem implementations can be swapped in:
```cpp
// Use different filesystem backends
platformFileSystemProvider espFS;        // LittleFS/SPIFFS
sdCardFileSystemProvider sdFS;           // SD card (custom implementation)
networkFileSystemProvider networkFS;     // Network storage (custom implementation)

configManager config1(&espFS);      // Internal flash
configManager config2(&sdFS);       // SD card storage
configManager config3(&networkFS);  // Network storage
```

### 3. Separation of Concerns
- `configManager`: Handles JSON parsing, configuration logic
- `platformFileSystemProvider`: Handles platform-specific file operations
- `iFileSystemProvider`: Defines the contract between them

## Custom File System Providers

You can create custom filesystem providers by implementing `iFileSystemProvider`:

```cpp
class CustomFileSystemProvider : public iFileSystemProvider {
public:
    bool begin() override {
        // Initialize your custom filesystem
        return true;
    }
    
    File open(const char* path, const char* mode) override {
        // Open file with your custom filesystem
    }
    
    bool remove(const char* path) override {
        // Remove file with your custom filesystem
    }
    
    bool exists(const char* path) override {
        // Check if file exists with your custom filesystem
    }
};
```

## Backward Compatibility

### Option 1: Wrapper Class (Recommended for gradual migration)
Create a wrapper that provides the old interface:
```cpp
class legacyConfigManager {
private:
    platformFileSystemProvider fsProvider;
    configManager config;
    
public:
    legacyConfigManager(const String& configFilePath = "/config.json", size_t maxConfigSize = 8192)
        : config(&fsProvider, configFilePath, maxConfigSize) {}
    
    // Delegate all methods to the new configManager
    String getValue(const String& section, const String& key) const {
        return config.getValue(section, key);
    }
    
    void setValue(const String& section, const String& key, const String& value) {
        config.setValue(section, key, value);
    }
    
    bool loadConfig() { return config.loadConfig(); }
    bool saveConfig() { return config.saveConfig(); }
    
    // Add other methods as needed...
};
```

### Option 2: Conditional Compilation
Use preprocessor directives to support both versions:
```cpp
#ifdef CONFIG_MANAGER_V2
    #include <platformFileSystemProvider.hpp>
    platformFileSystemProvider fsProvider;
    configManager config(&fsProvider, "/config.json");
#else
    configManager config("/config.json");  // v1.x style
#endif
```

## Common Issues and Solutions

### Issue 1: Null Pointer Exception
**Problem:** Passing nullptr to configManager constructor
```cpp
configManager config(nullptr);  // ❌ Will cause issues
```

**Solution:** Always create a valid filesystem provider
```cpp
platformFileSystemProvider fsProvider;
configManager config(&fsProvider);  // ✅ Correct
```

### Issue 2: Stack vs Heap Allocation
**Problem:** Filesystem provider goes out of scope
```cpp
void setup() {
    platformFileSystemProvider fsProvider;  // ❌ Local variable
    configManager config(&fsProvider);
    // fsProvider destroyed when setup() ends
}
```

**Solution:** Use global or static storage
```cpp
// Global scope
platformFileSystemProvider fsProvider;
configManager config(&fsProvider);

void setup() {
    // Now safe to use
}
```

### Issue 3: Interface Method Not Found
**Problem:** Using removed interface methods
```cpp
auto sections = provider->getSectionNames();  // ❌ Method removed
```

**Solution:** Use the updated method name
```cpp
auto sections = provider->getSections();  // ✅ Correct
```

## Performance Considerations

The new architecture has minimal performance impact:
- **Memory:** Additional 4-8 bytes for the filesystem provider pointer
- **CPU:** One extra indirection for filesystem calls (negligible)
- **Code Size:** Slightly larger due to interface abstraction

The benefits (testability, flexibility, maintainability) far outweigh these minimal costs.

## Next Steps

1. Update your `#include` statements
2. Create filesystem provider instances
3. Update configManager constructor calls
4. Test your application thoroughly
5. Consider writing unit tests using mock filesystem providers

For questions or issues, please refer to the examples in the `examples/` directory or create an issue in the project repository.
