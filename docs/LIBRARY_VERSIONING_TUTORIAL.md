# Library Versioning Tutorial for PlatformIO

This guide explains how to create and manage library versions so that old applications can continue using the old library while new users get access to the latest version.

## Table of Contents
1. [Git-Based Versioning](#git-based-versioning)
2. [PlatformIO Library Versioning](#platformio-library-versioning)
3. [Semantic Versioning](#semantic-versioning)
4. [Branch Management Strategy](#branch-management-strategy)
5. [Registry Publishing](#registry-publishing)
6. [User Consumption](#user-consumption)
7. [Best Practices](#best-practices)

## Git-Based Versioning

### 1. Create Version Tags
Git tags are the foundation of library versioning:

```bash
# Create and push version tags
git tag v1.0.0
git tag v1.1.0
git tag v2.0.0
git push origin --tags

# List all tags
git tag -l

# Show specific tag info
git show v1.0.0
```

### 2. Branch Strategy for Major Versions
Maintain separate branches for major versions:

```bash
# Create maintenance branches for major versions
git checkout -b v1.x v1.0.0
git checkout -b v2.x v2.0.0

# Push branches
git push origin v1.x
git push origin v2.x

# Apply critical fixes to old versions
git checkout v1.x
# Make fixes
git tag v1.0.1
git push origin v1.x --tags
```

## PlatformIO Library Versioning

### 1. library.json Configuration
Update your `library.json` to support versioning:

```json
{
  "name": "ConfigManager",
  "version": "2.0.0",
  "description": "Persistent JSON configuration manager for ESP32/ESP8266",
  "keywords": ["esp32", "esp8266", "json", "config", "filesystem"],
  "repository": {
    "type": "git",
    "url": "https://github.com/your-username/your-repo.git"
  },
  "authors": [
    {
      "name": "Peter K Green",
      "email": "pkg40@yahoo.com",
      "maintainer": true
    }
  ],
  "license": "MIT",
  "homepage": "https://github.com/your-username/your-repo",
  "dependencies": [
    {
      "name": "ArduinoJson",
      "version": "^6.19.0"
    }
  ],
  "frameworks": ["arduino", "espidf"],
  "platforms": ["espressif32", "espressif8266"],
  "build": {
    "includeDir": "src"
  },
  "export": {
    "exclude": [
      "test/*",
      "examples/*",
      ".github/*",
      "docs/*"
    ]
  }
}
```

### 2. Version Compatibility Matrix
Document version compatibility in your README:

| Library Version | Breaking Changes | Compatible Arduino | Compatible PlatformIO |
|----------------|------------------|-------------------|----------------------|
| v1.x.x         | None             | 1.8.0+            | 6.0+                |
| v2.x.x         | Constructor API  | 1.8.0+            | 6.0+                |
| v3.x.x         | Interface Changes| 2.0.0+            | 6.1+                |

## Semantic Versioning

Follow [Semantic Versioning (SemVer)](https://semver.org/) principles:

### Version Format: MAJOR.MINOR.PATCH

```
v2.1.3
│ │ │
│ │ └── PATCH: Bug fixes (backwards compatible)
│ └──── MINOR: New features (backwards compatible)  
└────── MAJOR: Breaking changes (not backwards compatible)
```

### Examples:
- `v1.0.0` → `v1.0.1`: Bug fix (patch)
- `v1.0.1` → `v1.1.0`: New feature (minor)
- `v1.1.0` → `v2.0.0`: Breaking change (major)

### Pre-release Versions:
- `v2.0.0-alpha.1`: Alpha release
- `v2.0.0-beta.1`: Beta release
- `v2.0.0-rc.1`: Release candidate

## Branch Management Strategy

### Main Branches:
- `main`/`master`: Latest stable version
- `develop`: Next version development
- `v1.x`, `v2.x`: Maintenance branches for major versions

### Workflow:
```bash
# Development workflow
git checkout develop
git checkout -b feature/new-feature
# Develop feature
git checkout develop
git merge feature/new-feature

# Release workflow
git checkout main
git merge develop
git tag v2.1.0
git push origin main --tags

# Hotfix for old version
git checkout v1.x
git checkout -b hotfix/critical-bug
# Fix bug
git checkout v1.x  
git merge hotfix/critical-bug
git tag v1.2.1
git push origin v1.x --tags
```

### Visual Branch Strategy:
```
main     ●─────●─────●─────●  (v2.0.0, v2.1.0)
         │     │     │     │
develop  ├─●─●─┴─●─●─┴─●─●─┘
         │
v1.x     ●─────●─────●       (v1.0.0, v1.1.0, v1.1.1)
```

## Registry Publishing

### 1. PlatformIO Registry
Publish to the official PlatformIO Registry:

```bash
# Install PlatformIO CLI
pip install platformio

# Login to PlatformIO account
pio account login

# Publish library
pio package publish --type=library

# Publish specific version
git checkout v1.0.0
pio package publish --type=library
```

### 2. GitHub Releases
Create GitHub releases for each version:

```bash
# Using GitHub CLI
gh release create v2.0.0 \
  --title "ConfigManager v2.0.0 - Filesystem Provider Separation" \
  --notes-file CHANGELOG.md

# Or use GitHub web interface
# Go to: https://github.com/your-username/your-repo/releases/new
```

### 3. Package Metadata
Ensure each version has proper metadata:

```json
{
  "name": "ConfigManager",
  "version": "2.0.0",
  "description": "...",
  "repository": {
    "type": "git",
    "url": "https://github.com/your-username/your-repo.git",
    "branch": "main"
  }
}
```

## User Consumption

### 1. Specify Versions in platformio.ini

```ini
[env:esp32]
platform = espressif32
board = esp32dev
framework = arduino

lib_deps = 
    # Latest version
    your-username/ConfigManager
    
    # Specific version
    your-username/ConfigManager@^2.0.0
    
    # Version range
    your-username/ConfigManager@>=1.0.0,<2.0.0
    
    # Git tag
    https://github.com/your-username/your-repo.git#v1.0.0
    
    # Git branch
    https://github.com/your-username/your-repo.git#v1.x
```

### 2. Version Constraint Examples:

```ini
# Exact version
ConfigManager@2.0.0

# Compatible version (allow patch updates)
ConfigManager@~2.0.0    # Allows 2.0.x

# Caret version (allow minor updates)
ConfigManager@^2.0.0    # Allows 2.x.x

# Range version
ConfigManager@>=2.0.0,<3.0.0

# Latest pre-release
ConfigManager@2.1.0-beta.1
```

### 3. Legacy Support Example:

**For applications using v1.x:**
```ini
[env:legacy]
lib_deps = 
    ConfigManager@^1.0.0  # Stays on v1.x branch
```

**For new applications using v2.x:**
```ini
[env:modern]
lib_deps = 
    ConfigManager@^2.0.0  # Gets latest v2.x
```

## Best Practices

### 1. Maintain Changelog
Keep a detailed `CHANGELOG.md`:

```markdown
# Changelog

## [2.0.0] - 2025-01-XX
### Breaking Changes
- Constructor now requires iFileSystemProvider parameter
- Removed getSectionNames() method (use getSections())

### Added
- platformFileSystemProvider class
- iFileSystemProvider interface
- Improved testability with dependency injection

### Fixed
- Memory leak in config parsing

## [1.1.0] - 2024-12-XX
### Added
- New configuration validation methods
### Fixed
- ESP8266 compatibility issues
```

### 2. Documentation Versioning
Maintain version-specific documentation:

```
docs/
├── v1/
│   ├── README.md
│   ├── API.md
│   └── examples/
├── v2/
│   ├── README.md
│   ├── API.md
│   ├── UPGRADE_GUIDE.md
│   └── examples/
└── latest/  # Symlink to current version
```

### 3. Testing Strategy
Test multiple versions:

```ini
# .github/workflows/test.yml
strategy:
  matrix:
    version: [v1.x, v2.x, main]
    platform: [esp32, esp8266]
```

### 4. Migration Guides
Provide clear migration paths:

```markdown
# Migrating from v1.x to v2.x

## Quick Migration
1. Add filesystem provider
2. Update constructor
3. Replace getSectionNames() with getSections()

## Compatibility Layer
Use the provided legacy wrapper for gradual migration.
```

### 5. Support Policy
Define support policy:

```markdown
# Support Policy

- **Current Major Version (v2.x)**: Full support
- **Previous Major Version (v1.x)**: Security fixes only
- **Older Versions (v0.x)**: No support

## End-of-Life Schedule
- v1.x: Supported until 2026-01-01
- v0.x: End-of-life (no updates)
```

### 6. Release Automation
Automate releases with GitHub Actions:

```yaml
# .github/workflows/release.yml
name: Release
on:
  push:
    tags: ['v*']

jobs:
  release:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Publish to PlatformIO Registry
        env:
          PLATFORMIO_AUTH_TOKEN: ${{ secrets.PLATFORMIO_AUTH_TOKEN }}
        run: |
          pip install platformio
          pio package publish --type=library
```

## Advanced Versioning Scenarios

### 1. Parallel Development
Support multiple major versions simultaneously:

```bash
# v1.x maintenance
git checkout v1.x
git checkout -b feature/v1-security-fix
# Implement fix for v1.x users

# v2.x development  
git checkout main
git checkout -b feature/v2-new-feature
# Implement new features for v2.x
```

### 2. Backporting Features
Selectively backport features to older versions:

```bash
# Cherry-pick specific commits to older versions
git checkout v1.x
git cherry-pick abc123def  # Backport specific fix
git tag v1.2.1
```

### 3. Version Detection at Runtime
Allow libraries to detect their own version:

```cpp
// In your library header
#define CONFIG_MANAGER_VERSION_MAJOR 2
#define CONFIG_MANAGER_VERSION_MINOR 0  
#define CONFIG_MANAGER_VERSION_PATCH 0
#define CONFIG_MANAGER_VERSION "2.0.0"

class configManager {
public:
    static const char* getVersion() { return CONFIG_MANAGER_VERSION; }
    static int getMajorVersion() { return CONFIG_MANAGER_VERSION_MAJOR; }
};
```

## Conclusion

Proper library versioning ensures:
- **Backward Compatibility**: Old applications continue working
- **Forward Progress**: New features don't break existing code
- **Clear Migration Path**: Users can upgrade when ready
- **Professional Quality**: Follows industry standards

Following these practices will make your library more professional, user-friendly, and maintainable over time.
