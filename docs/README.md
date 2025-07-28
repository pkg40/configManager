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

Include in your project:

```cpp
#include "configManager.hpp"
#include "platformFileSystemProvider.hpp"
```

---

## Quick Start

```cpp
#include "configManager.hpp"
#include "platformFileSystemProvider.hpp"

// Create filesystem provider and config manager
platformFileSystemProvider fsProvider;
configManager config(&fsProvider);

void setup() {
    if (config.loadConfig()) {
        String ssid = config.getValue("wifiSTA", "ssid");
        config.setValue("wifiSTA", "ssid", "newSSID");
        config.saveConfig();
    }
}
```

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
