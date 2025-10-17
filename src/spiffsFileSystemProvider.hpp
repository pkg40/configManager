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
     * End the SPIFFS filesystem
     */
    void end() override {
        SPIFFS.end();
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
    
    /**
     * Rename a file
     * @param pathFrom source path
     * @param pathTo destination path
     * @return true if successful, false otherwise
     */
    bool rename(const char *pathFrom, const char *pathTo) override {
        return SPIFFS.rename(pathFrom, pathTo);
    }
    
    /**
     * Get total size of SPIFFS
     * @return total size in bytes
     */
    size_t totalBytes() override {
        return SPIFFS.totalBytes();
    }
    
    /**
     * Get used size of SPIFFS
     * @return used size in bytes
     */
    size_t usedBytes() override {
        return SPIFFS.usedBytes();
    }
};
