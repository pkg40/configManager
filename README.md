# 📘 configManager v2.0 — Persistent Config Management for ESP32/ESP8266

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/pkg40/library/abstractClasses-config.svg)](https://registry.platformio.org/libraries/pkg40/abstractClasses-config)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-2.0.0-blue.svg)](https://github.com/pkg40/configManager/releases/tag/v2.0.0)

A lightweight, modular configuration library for ESP devices, using SPIFFS/LittleFS and ArduinoJson for structured JSON-based settings. Version 2.0 introduces filesystem provider separation for improved testability and flexibility.

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

## ⚠️ Breaking Changes in v2.0

**Constructor now requires a filesystem provider:**
```cpp
// v1.x (old)
configManager config("/config.json");

// v2.0 (new)
platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/config.json");
```

## 📦 Installation

### PlatformIO (Recommended)

Add to your `platformio.ini`:

```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = 
    pkg40/abstractClasses-config@^2.0.0  ; Latest stable v2.x
```

### Version Selection

```ini
# Latest v2.x (recommended for new projects)
lib_deps = pkg40/abstractClasses-config@^2.0.0

# Exact version (for production stability)
lib_deps = pkg40/abstractClasses-config@2.0.0

# Git repository (development)
lib_deps = https://github.com/pkg40/configManager.git#v2.0.0

# Legacy v1.x (for existing projects)
lib_deps = pkg40/abstractClasses-config@^1.0.0
```

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

## 📖 Library Versioning & Tags

This library follows [Semantic Versioning (SemVer)](https://semver.org/):

### Available Versions
- **v2.0.0** - "Stable abstract interface full package" ✅ Current stable
- **v1.x.x** - Legacy version (security fixes only until 2026-01-01)

### Creating New Versions (For Contributors)

When releasing a new version:

1. **Update version in `library.json`**:
   ```json
   {
     "version": "2.1.0"
   }
   ```

2. **Create and push git tag**:
   ```bash
   git tag -a "v2.1.0" -m "Description of changes"
   git push origin v2.1.0
   ```

3. **Update CHANGELOG.md** with version notes

4. **Publish to PlatformIO Registry** (if applicable):
   ```bash
   pio package publish --type=library
   ```

### Tag Naming Convention
- Use format: `v{MAJOR}.{MINOR}.{PATCH}` (e.g., `v2.0.0`)
- Annotated tags with descriptive messages
- Follow semantic versioning rules

## 🎯 Build Configuration

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

## 📚 Documentation

- **[Complete Usage Guide](docs/README.md)** - Detailed usage examples and API reference
- **[Upgrade Guide](docs/UPGRADE_GUIDE_V2.md)** - Migration from v1.x to v2.0
- **[Library Versioning Tutorial](docs/LIBRARY_VERSIONING_TUTORIAL.md)** - Comprehensive versioning guide
- **[Optimization Guide](docs/OPTIMIZATION_GUIDE.md)** - Performance optimization options
- **[Test Framework](docs/TEST_FRAMEWORK.md)** - Testing and validation
- **[Changelog](CHANGELOG.md)** - Version history and changes

## 💻 Platform Support

| Platform | Board Examples | Filesystem | Status |
|----------|---------------|------------|--------|
| ESP32 | ESP32 Dev, XIAO ESP32S3 | SPIFFS, LittleFS | ✅ Fully Supported |
| ESP8266 | NodeMCU, Wemos D1 | SPIFFS, LittleFS* | ✅ Fully Supported |

*LittleFS recommended for ESP8266

## 🧪 Testing

The library includes comprehensive testing:

```cpp
#include "testLib.hpp"

void setup() {
    platformFileSystemProvider fsProvider;
    configManager config(&fsProvider);
    
    // Run all tests
    testLib::runAllTests(&config);
}
```

### Embedded On-Device Test Runner (BasePacket Standard)

An embedded multi-layer test harness is available when building with the `seeed_xiao_esp32s3_tests` environment (adds `-DRUN_EMBEDDED_TESTS`). It executes:

1. Layer 1: Basic, interface, polymorphic
2. Layer 2: Persistence, error handling, memory, edge, backward compatibility
3. Layer 3: Performance micro-benchmarks
4. Layer 4: Advanced stress & JSON compliance (separate advanced suite)

Run sequence:

```sh
pio run -e seeed_xiao_esp32s3_tests
pio upload -e seeed_xiao_esp32s3_tests
pio device monitor -e seeed_xiao_esp32s3_tests
```

Monitor output for PASS/FAIL, performance timings, and memory summaries.

To run production firmware instead:

```sh
pio run -e seeed_xiao_esp32s3
pio upload -e seeed_xiao_esp32s3
```

ESP8266 and other boards can still use the existing environments for compatibility validation.

### Unity Test Environment
The Unity unit test environment is defined in a chained config (`test/platformio.unity.ini`) referenced from the main `platformio.ini` via:

```
[platformio]
extra_configs = test/platformio.unity.ini
```

Run Unity tests:
```
pio run -e seeed_xiao_esp32s3_unity -t upload
pio device monitor -e seeed_xiao_esp32s3_unity
```

### Native Host Tests
The `native` environment provides fast host-side execution without hardware. A simplified JSON parser is used currently; advanced JSON-centric tests (special characters & JSON compliance) are skipped in native mode until a full host JSON backend is added.

To implement full parity, integrate a desktop JSON library (e.g. nlohmann/json) under `CONFIGMGR_NATIVE`.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes following the coding standards
4. Add tests for new functionality
5. Update documentation
6. Submit a pull request

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

**Author**: Peter K Green (pkg40@yahoo.com)

## 🔗 Links

- [GitHub Repository](https://github.com/pkg40/configManager)
- [PlatformIO Registry](https://registry.platformio.org/libraries/pkg40/abstractClasses-config)
- [Issues & Support](https://github.com/pkg40/configManager/issues)
- [Release Notes](https://github.com/pkg40/configManager/releases)

---

⭐ **Star this repository if you find it useful!**
