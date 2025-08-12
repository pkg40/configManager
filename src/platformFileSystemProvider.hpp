#pragma once

#ifndef CONFIGMGR_NATIVE
#include <interface/iFileSystemProvider.hpp>

#if defined(ESP32)
#if defined(USE_LITTLEFS)
#include <LittleFS.h>
#define CONFIG_FS LittleFS
#else
#include <SPIFFS.h>
#define CONFIG_FS SPIFFS
#endif
#elif defined(ESP8266)
#include <LittleFS.h>
#define CONFIG_FS LittleFS
#endif

/**
 * @brief A concrete implementation of iFileSystemProvider for ESP32/ESP8266 platforms.
 *
 * This class uses the standard ESP filesystems (LittleFS or SPIFFS)
 * to perform file operations. The specific filesystem is determined
 * by the platform and build flags.
 */
class platformFileSystemProvider : public iFileSystemProvider
{
public:
    bool begin() override;
    File open(const char *path, const char *mode) override;
    bool remove(const char *path) override;
    bool exists(const char *path) override;
};

#endif // !CONFIGMGR_NATIVE
