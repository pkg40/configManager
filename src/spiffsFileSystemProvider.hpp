#pragma once

#include <Arduino.h>
#include <FS.h>
#include "interface/iFileSystemProvider.hpp"

/**
 * SPIFFS File System Provider
 * 
 * Implementation of iFileSystemProvider for SPIFFS filesystem.
 * Provides file operations for the configManager using SPIFFS.
 */

class spiffsFileSystemProvider : public iFileSystemProvider {
public:
    /**
     * Initialize the SPIFFS filesystem
     * @return true if successful, false otherwise
     */
    bool begin() override {
        return SPIFFS.begin(true); // Format on fail
    }
    
    /**
     * Open a file for reading or writing
     * @param path file path
     * @param mode file mode ("r", "w", "a", etc.)
     * @return File object
     */
    fs::File open(const char *path, const char *mode) override {
        return SPIFFS.open(path, mode);
    }
    
    /**
     * Check if a file exists
     * @param path file path
     * @return true if file exists, false otherwise
     */
    bool exists(const char *path) override {
        return SPIFFS.exists(path);
    }
    
    /**
     * Remove a file
     * @param path file path
     * @return true if successful, false otherwise
     */
    bool remove(const char *path) override {
        return SPIFFS.remove(path);
    }
};
