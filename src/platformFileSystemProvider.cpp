#include <platformFileSystemProvider.hpp>
#ifdef CONFIGMGR_NATIVE
// Excluded for native build (nativeFileSystemProvider provides implementation)
#warning "platformFileSystemProvider.cpp skipped in native build"
#else

bool platformFileSystemProvider::begin()
{
#if defined(ESP32)
    // For ESP32, the 'true' argument formats the filesystem if it fails to mount.
    return CONFIG_FS.begin(true);
#elif defined(ESP8266)
    // For ESP8266, begin() does not take arguments.
    return CONFIG_FS.begin();
#else
    return false; // Not supported on this platform
#endif
}

fs::File platformFileSystemProvider::open(const char *path, const char *mode)
{
    return CONFIG_FS.open(path, mode);
}

bool platformFileSystemProvider::remove(const char *path)
{
    return CONFIG_FS.remove(path);
}

bool platformFileSystemProvider::exists(const char *path)
{
    return CONFIG_FS.exists(path);
}

#endif // !CONFIGMGR_NATIVE
