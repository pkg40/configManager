# Native Mode Advanced Test Notes

## Why Advanced Tests Fail in Native Mode
- The current native (host) build uses a naive JSON parser that only recognizes section names, not key-value pairs.
- As a result, all advanced tests that require persistence, value storage, or JSON compliance fail in native mode.

## How to Fix
- Integrate a real C++ JSON library (e.g., nlohmann/json) for native builds.
- Use conditional compilation in `configManager.cpp` to select ArduinoJson for embedded and nlohmann/json for native.
- Map between `String` and `std::string` as needed.
- This will allow all advanced and persistence tests to pass natively, matching embedded behavior.

## Summary
- Core and interface tests pass in native mode.
- Advanced tests require a real JSON parser for full native support.
- Embedded builds are unaffected and use ArduinoJson as before.
