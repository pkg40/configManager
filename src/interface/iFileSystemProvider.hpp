#pragma once

#include <Arduino.h>
#include <FS.h>

/**
 * @brief Interface for a file system provider.
 *
 * This abstract class defines the contract for file system operations,
 * allowing configManager to be independent of the underlying filesystem
 * implementation (e.g., LittleFS, SPIFFS).
 */
class iFileSystemProvider
{
public:
    virtual ~iFileSystemProvider() = default;

    /**
     * @brief Initializes the filesystem.
     * @return true on success, false on failure.
     */
    virtual bool begin() = 0;

    /**
     * @brief Opens a file.
     * @param path The path to the file.
     * @param mode The mode to open the file in (e.g., "r", "w").
     * @return A File object. The file will be invalid if it could not be opened.
     */
    virtual File open(const char *path, const char *mode) = 0;

    /**
     * @brief Removes a file.
     * @param path The path to the file.
     * @return true on success, false on failure.
     */
    virtual bool remove(const char *path) = 0;

    /**
     * @brief Checks if a file exists.
     * @param path The path to the file.
     * @return true if the file exists, false otherwise.
     */
    virtual bool exists(const char *path) = 0;
};
