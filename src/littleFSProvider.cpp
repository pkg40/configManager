#include <littleFSProvider.hpp>

#ifdef CONFIGMGR_NATIVE
// Excluded for native build
#warning "littleFSProvider.cpp skipped in native build"
#else

bool littleFSProvider::begin()
{
#if defined(ESP32)
    // For ESP32, the 'true' argument formats the filesystem if it fails to mount.
    // LittleFS.begin(true) will attempt to mount and format if needed
    bool result = LITTLEFS_CONFIG_FS.begin(true);
    if (!result) {
        // If begin with format fails, try explicit format then begin again
        if (LITTLEFS_CONFIG_FS.format()) {
            result = LITTLEFS_CONFIG_FS.begin(false);
        }
    }
    return result;
    
#elif defined(ESP8266)
    // For ESP8266, begin() does not take arguments by default
    // Try to begin, if it fails, format and try again
    bool result = LITTLEFS_CONFIG_FS.begin();
    if (!result) {
        // Format and try again
        if (LITTLEFS_CONFIG_FS.format()) {
            result = LITTLEFS_CONFIG_FS.begin();
        }
    }
    return result;
    
#else
    return false; // Not supported on this platform
#endif
}

bool littleFSProvider::end()
{
#if defined(ESP32) || defined(ESP8266)
    LITTLEFS_CONFIG_FS.end();
    return true;
#else
    return false; // Not supported on this platform
#endif
}

fs::File littleFSProvider::open(const char *path, const char *mode)
{
    return LITTLEFS_CONFIG_FS.open(path, mode);
}

bool littleFSProvider::remove(const char *path)
{
    return LITTLEFS_CONFIG_FS.remove(path);
}

bool littleFSProvider::exists(const char *path)
{
    return LITTLEFS_CONFIG_FS.exists(path);
}

size_t littleFSProvider::totalBytes()
{
#if defined(ESP32) || defined(ESP8266)
    return LITTLEFS_CONFIG_FS.totalBytes();
#else
    return 0;
#endif
}

size_t littleFSProvider::usedBytes()
{
#if defined(ESP32) || defined(ESP8266)
    return LITTLEFS_CONFIG_FS.usedBytes();
#else
    return 0;
#endif
}

bool littleFSProvider::format()
{
#if defined(ESP32) || defined(ESP8266)
    return LITTLEFS_CONFIG_FS.format();
#else
    return false;
#endif
}

#endif // !CONFIGMGR_NATIVE