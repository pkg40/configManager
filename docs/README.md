# 📘 configManager v2.0 — Persistent Config Management for ESP32/ESP8266

A lightweight, modular configuration library for ESP devices, using SPIFFS/LittleFS and ArduinoJson for structured JSON-based settings. Version 2.0 introduces filesystem provider separation for improved testability and flexibility.

---

## 🚀 Features

- 🔁 Persistent config via SPIFFS/LittleFS `.json` file
- 🛡️ Auto fallback to hardcoded defaults
- 🔍 Safe get/set with default value support
- 💾 Serialize config to JSON (pretty or compact)
- 📊 Human-readable output via Serial
- 🧹 Delete/reset config file on demand
- 🔀 **NEW v2.0**: Modular filesystem provider architecture
- 🧪 **NEW v2.0**: Improved testability with dependency injection
- 🎯 **NEW v2.0**: SOLID principles compliance

---

## ⚠️ Breaking Changes in v2.0

**Constructor now requires a filesystem provider:**
```cpp
// v1.x (old)
configManager config("/config.json");

// v2.0 (new)
platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/config.json");
```

See [UPGRADE_GUIDE_V2.md](UPGRADE_GUIDE_V2.md) for complete migration instructions.

---

## 🔧 Installation

### Option 1: PlatformIO Library Registry

Add to your `platformio.ini`:

```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    pkg40/abstractClasses-config@^2.0.0  ; Latest v2.x version
    ; or
    pkg40/abstractClasses-config@2.0.0   ; Exact version
```

### Option 2: Git Repository

```ini
[env:esp32]
lib_deps = 
    https://github.com/pkg40/configManager.git#v2.0.0
```

### Option 3: Local Development

Include in your project:

```cpp
#include "configManager.hpp"
#include "platformFileSystemProvider.hpp"
```

---

## 📖 Library Versioning & Tagging

This library follows [Semantic Versioning (SemVer)](https://semver.org/):

### Available Versions
- **v2.0.0** - "Stable abstract interface full package" ✅ Current
- **v1.x.x** - Legacy version (maintenance only)

### Version Selection in platformio.ini

```ini
# Latest v2.x (recommended for new projects)
lib_deps = pkg40/abstractClasses-config@^2.0.0

# Exact version (for production stability)
lib_deps = pkg40/abstractClasses-config@2.0.0

# Version range
lib_deps = pkg40/abstractClasses-config@>=2.0.0,<3.0.0

# Git tag (development)
lib_deps = https://github.com/pkg40/configManager.git#v2.0.0

# Legacy v1.x (for existing projects)
lib_deps = pkg40/abstractClasses-config@^1.0.0
```

### Migration Between Versions
- **v1.x → v2.0**: See [UPGRADE_GUIDE_V2.md](UPGRADE_GUIDE_V2.md)
- **v2.0 → future**: Breaking changes will increment major version

---

## 🚀 Quick Start

```cpp
#include "configManager.hpp"
#include "platformFileSystemProvider.hpp"

// Create filesystem provider and config manager
platformFileSystemProvider fsProvider;
configManager config(&fsProvider);

void setup() {
    Serial.begin(115200);
    
    // Load configuration from filesystem
    if (config.loadConfig()) {
        Serial.println("✅ Config loaded successfully");
        
        // Read existing values
        String ssid = config.getValue("wifiSTA", "ssid");
        String password = config.getValue("wifiSTA", "password");
        
        // Modify configuration
        config.setValue("wifiSTA", "ssid", "MyNewWiFi");
        config.setValue("wifiSTA", "password", "MyNewPassword");
        
        // Save changes
        config.saveConfig();
    } else {
        Serial.println("⚠️ Config load failed, using defaults");
    }
    
    // Print current configuration
    config.printConfigToSerial();
}

void loop() {
    // Your main code here
}
```

---

## 💡 Advanced Usage Examples

### Polymorphic Interface Usage

```cpp
#include "configManager.hpp"
#include "platformFileSystemProvider.hpp"

void processConfig(iConfigProvider* config) {
    // Works with any implementation of iConfigProvider
    auto sections = config->getSections();
    String user = config->getUser();
    String password = config->getPassword();
    
    // Modify configuration
    config->setValue("system", "mode", "production");
    config->saveConfig();
}

void setup() {
    platformFileSystemProvider fsProvider;
    configManager config(&fsProvider);
    
    // Use polymorphically
    processConfig(&config);
}
```

### Custom Filesystem Provider

```cpp
#include "interface/iFileSystemProvider.hpp"

class CustomFileSystemProvider : public iFileSystemProvider {
public:
    bool begin() override {
        // Your custom filesystem initialization
        return true;
    }
    
    File open(const char* path, const char* mode) override {
        // Your custom file opening logic
        return File(); // Return your custom File object
    }
    
    bool remove(const char* path) override {
        // Your custom file removal logic
        return true;
    }
    
    bool exists(const char* path) override {
        // Your custom file existence check
        return true;
    }
};

void setup() {
    CustomFileSystemProvider customFS;
    configManager config(&customFS, "/custom_config.json");
    
    config.loadConfig();
}
```

### Testing with Mock Provider

```cpp
// For unit testing
class MockFileSystemProvider : public iFileSystemProvider {
private:
    std::map<String, String> mockFiles;
    
public:
    bool begin() override { return true; }
    
    File open(const char* path, const char* mode) override {
        // Return mock file based on mockFiles map
        return File(); // Mock implementation
    }
    
    bool remove(const char* path) override {
        mockFiles.erase(path);
        return true;
    }
    
    bool exists(const char* path) override {
        return mockFiles.count(path) > 0;
    }
    
    // Helper for testing
    void setMockFile(const String& path, const String& content) {
        mockFiles[path] = content;
    }
};
```

### Configuration Validation

```cpp
void validateConfig(configManager& config) {
    // Check required sections exist
    auto sections = config.getSections();
    
    bool hasWifi = std::find(sections.begin(), sections.end(), "wifiSTA") != sections.end();
    bool hasAuth = std::find(sections.begin(), sections.end(), "_auth") != sections.end();
    
    if (!hasWifi) {
        Serial.println("⚠️ Missing WiFi configuration");
        config.setValue("wifiSTA", "ssid", "default_ssid");
        config.setValue("wifiSTA", "password", "default_password");
    }
    
    if (!hasAuth) {
        Serial.println("⚠️ Missing authentication configuration");
        config.setValue("_auth", "_user", "admin");
        config.setValue("_auth", "_password", "admin");
    }
    
    config.saveConfig();
}
```

### Memory Management

```cpp
void monitorMemory(configManager& config) {
    Serial.println("=== Memory Status ===");
    config.printHeapStatus();
    
    size_t configSize = config.getConfigMemoryUsage();
    Serial.printf("Config memory usage: %u bytes\n", configSize);
    
    // Clear config if memory is low
    if (ESP.getFreeHeap() < 10000) {
        Serial.println("⚠️ Low memory, clearing config");
        config.clearConfig();
    }
}
```

---

## 🎯 Build Flags & Optimization

Configure your build in `platformio.ini`:

```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    pkg40/abstractClasses-config@^2.0.0
build_flags =
    -DUSE_LITTLEFS                    ; Use LittleFS instead of SPIFFS
    -DCONFIG_DISABLE_INTERFACE        ; Disable vtable for optimization
    -DTESTBENCH                       ; Enable test functionality
    -Isrc                             ; Include src directory
    -Isrc/interface                   ; Include interface directory
```

### Build Flag Options:
- `USE_LITTLEFS`: Use LittleFS filesystem (recommended for ESP8266)
- `CONFIG_DISABLE_INTERFACE`: Remove interface overhead for performance
- `TESTBENCH`: Enable testing functionality

---

## Extensibility

- The `iConfigProvider` interface allows you to implement your own config backends.
- Add new config sections or fields by updating your JSON and using `setValue`/`getValue`.
- All config data is stored in a nested map: `std::map<String, std::map<String, String>>`.

---

## Memory Usage

- On ESP8266, keep config files under 8KB (`MAX_CONFIG_SIZE`).
- Use `printHeapStatus()` and `getConfigMemoryUsage()` for diagnostics.

---

## License & Author

MIT License. Author: Peter K Green (pkg40@yahoo.com)
