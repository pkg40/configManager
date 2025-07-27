# 📘 configManager — Persistent Config Management for ESP32/ESP8266

A lightweight configuration class for ESP devices, using SPIFFS and ArduinoJson for structured JSON-based settings. It supports default fallbacks, dynamic access, runtime updates, and full serialization.

---

## 🚀 Features

- 🔁 Persistent config via SPIFFS `.json` file
- 🛡️ Auto fallback to hardcoded defaults
- 🔍 Safe get/set with default value support
- 💾 Serialize config to JSON (pretty or compact)
- 📊 Human-readable output via Serial
- 🧹 Delete/reset config file on demand

---

## 🔧 Installation

Include in your project:

```cpp
#include "configManager.h"