# Changelog

All notable changes to the ConfigManager library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2025-01-28

### Breaking Changes
- **Constructor API Change**: `configManager` constructor now requires an `iFileSystemProvider*` parameter
  ```cpp
  // Before (v1.x)
  configManager config("/config.json");
  
  // After (v2.0)
  platformFileSystemProvider fsProvider;
  configManager config(&fsProvider, "/config.json");
  ```
- **Interface Cleanup**: Removed duplicate `getSectionNames()` method from `iConfigProvider` (use `getSections()` instead)
- **Platform Dependencies**: Removed platform-specific filesystem includes from `configManager.hpp`

### Added
- **Filesystem Provider Separation**: New `iFileSystemProvider` interface for filesystem operations
- **Platform Implementation**: `platformFileSystemProvider` class for ESP32/ESP8266 filesystem support
- **Dependency Injection**: Improved testability through filesystem provider injection
- **SOLID Compliance**: Better separation of concerns between configuration and filesystem logic
- **Custom Providers**: Ability to create custom filesystem providers (SD card, network, mock, etc.)
- **Enhanced Documentation**: 
  - `UPGRADE_GUIDE_V2.md` for migration instructions
  - `LIBRARY_VERSIONING_TUTORIAL.md` for version management
  - Updated README with v2.0 information
- **New Examples**: 
  - `example_refactored_configManager.cpp` showing v2.0 usage
  - Polymorphic usage examples with `iConfigProvider`

### Improved
- **Testability**: Configuration logic can now be tested independently of filesystem
- **Flexibility**: Different filesystem backends can be easily swapped
- **Error Handling**: Better null pointer checks and error reporting
- **Code Organization**: Cleaner separation between interface and implementation
- **Memory Management**: Simplified lifecycle management

### Technical Details
- **File Changes**:
  - `configManager.hpp`: Updated constructor, removed platform includes, added filesystem provider member
  - `configManager.cpp`: Updated all filesystem calls to use provider interface
  - `iConfigProvider.hpp`: Removed duplicate `getSectionNames()` method
  - `platformFileSystemProvider.hpp/cpp`: New concrete filesystem provider
  - `iFileSystemProvider.hpp`: New filesystem provider interface

### Migration Path
1. Include `platformFileSystemProvider.hpp`
2. Create filesystem provider instance: `platformFileSystemProvider fsProvider;`
3. Update constructor call: `configManager config(&fsProvider, "/config.json");`
4. Replace `getSectionNames()` calls with `getSections()`

### Backward Compatibility
- **API**: Most methods unchanged except constructor
- **Functionality**: All existing features preserved
- **Performance**: Minimal overhead (one pointer indirection)
- **Memory**: Additional 4-8 bytes per configManager instance

## [1.0.1] - 2024-12-XX

### Fixed
- ESP8266 compatibility improvements
- Memory leak fixes in JSON parsing
- Platform detection edge cases

### Added
- Better error messages for config loading failures
- Heap status monitoring methods

## [1.0.0] - 2024-11-XX

### Initial Release
- JSON-based configuration management
- ESP32/ESP8266 support
- SPIFFS/LittleFS filesystem support
- Default configuration fallback
- Section-based configuration organization
- Serial output debugging
- Configuration validation and parsing

### Features
- Persistent storage of configuration in JSON format
- Automatic platform detection (ESP32/ESP8266)
- Filesystem abstraction (SPIFFS/LittleFS)
- Type-safe configuration access
- Built-in default configuration
- Memory usage monitoring
- Configuration file management (save/load/clear)

---

## Version Support Policy

- **v2.x**: Active development and full support
- **v1.x**: Security fixes and critical bug fixes until 2026-01-01
- **v0.x**: End-of-life (no support)

## Migration Assistance

For help migrating between versions:
- See `docs/UPGRADE_GUIDE_V2.md` for detailed migration instructions
- Check `examples/` directory for version-specific usage examples
- Review `docs/LIBRARY_VERSIONING_TUTORIAL.md` for version management strategies

## Contributing

When contributing to this project:
1. Follow semantic versioning for changes
2. Update this changelog with your modifications
3. Add appropriate tests for new features
4. Update documentation as needed
