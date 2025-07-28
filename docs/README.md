# 📘 configManager — Persistent Config Management for ESP32/ESP8266

A lightweight configuration class for ESP devices, using SPIFFS/LittleFS and ArduinoJson for structured JSON-based settings. It supports default fallbacks, dynamic access, runtime updates, and full serialization.

---

## 🚀 Features

- 🔁 Persistent config via SPIFFS/LittleFS `.json` file
- 🛡️ Auto fallback to hardcoded defaults
- 🔍 Safe get/set with default value support
- 💾 Serialize config to JSON (pretty or compact)
- 📊 Human-readable output via Serial
- 🧹 Delete/reset config file on demand

---

## Coding Standards & Naming Conventions

- All class, variable, and file names use camelCase (no exceptions).
- Private class variables are prefixed with an underscore (e.g., `_configMap`).
- Raw pointers use `_p` postfix, unique pointers use `Uptr`, shared pointers use `Sptr`, weak pointers use `Wptr`.
- Constants are ALL_CAPS (e.g., `MAX_CONFIG_SIZE`, `CONFIG_FILE_PATH`).
- Boolean variables use descriptive prefixes (e.g., `isConfigLoaded`).
- Type definitions use `_t` postfix.
- All code is C++11 compliant.

---

## 🔧 Installation

Include in your project:

```cpp
#include "configManager.hpp"
```

---

## Usage Example

```cpp
#include "configManager.hpp"

configManager config;
if (config.loadConfig()) {
    String ssid = config.getValue("wifiSTA", "ssid");
    config.setValue("wifiSTA", "ssid", "newSSID");
    config.saveConfig();
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
