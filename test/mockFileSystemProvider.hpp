/*
 * MIT License
 *
 * Copyright (c) 2025 Peter K Green (pkg40)
 * Email: pkg40@yahoo.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * Mock FileSystem Provider for Testing
 * Allows simulation of filesystem conditions for comprehensive testing
 */

#pragma once
#include <iFileSystemProvider.hpp>
#include <map>

class mockFileSystemProvider : public iFileSystemProvider {
private:
    std::map<String, String> _mockFiles;
    bool _simulateFailure;
    bool _simulateFormatFailure;
    bool _simulateReadFailure;
    bool _simulateWriteFailure;
    size_t _totalBytes;
    size_t _usedBytes;

public:
    mockFileSystemProvider() 
        : _simulateFailure(false)
        , _simulateFormatFailure(false)
        , _simulateReadFailure(false)
        , _simulateWriteFailure(false)
        , _totalBytes(1000000)  // 1MB
        , _usedBytes(0) {
    }

    // iFileSystemProvider interface implementation
    bool begin(bool formatOnFail = false) override {
        if (_simulateFailure) {
            return false;
        }
        return true;
    }

    bool format() override {
        if (_simulateFormatFailure) {
            return false;
        }
        _mockFiles.clear();
        _usedBytes = 0;
        return true;
    }

    bool exists(const String& path) override {
        return _mockFiles.find(path) != _mockFiles.end();
    }

    bool remove(const String& path) override {
        auto it = _mockFiles.find(path);
        if (it != _mockFiles.end()) {
            _usedBytes -= it->second.length();
            _mockFiles.erase(it);
            return true;
        }
        return false;
    }

    String readFile(const String& path) override {
        if (_simulateReadFailure) {
            return "";
        }
        
        auto it = _mockFiles.find(path);
        if (it != _mockFiles.end()) {
            return it->second;
        }
        return "";
    }

    bool writeFile(const String& path, const String& content) override {
        if (_simulateWriteFailure) {
            return false;
        }

        // Simulate space limitation
        size_t contentSize = content.length();
        size_t currentFileSize = 0;
        
        auto it = _mockFiles.find(path);
        if (it != _mockFiles.end()) {
            currentFileSize = it->second.length();
        }
        
        if (_usedBytes - currentFileSize + contentSize > _totalBytes) {
            return false; // Simulate out of space
        }

        _usedBytes = _usedBytes - currentFileSize + contentSize;
        _mockFiles[path] = content;
        return true;
    }

    size_t totalBytes() override {
        return _totalBytes;
    }

    size_t usedBytes() override {
        return _usedBytes;
    }

    // Test control methods
    void setSimulateFailure(bool fail) { _simulateFailure = fail; }
    void setSimulateFormatFailure(bool fail) { _simulateFormatFailure = fail; }
    void setSimulateReadFailure(bool fail) { _simulateReadFailure = fail; }
    void setSimulateWriteFailure(bool fail) { _simulateWriteFailure = fail; }
    
    void setTotalBytes(size_t bytes) { _totalBytes = bytes; }
    void fillSpace(size_t bytes) { _usedBytes = std::min(bytes, _totalBytes); }
    
    // Test inspection methods
    size_t getFileCount() const { return _mockFiles.size(); }
    
    void listFiles() const {
        Serial.println("=== Mock FileSystem Contents ===");
        for (const auto& file : _mockFiles) {
            Serial.printf("File: %s (%d bytes)\n", file.first.c_str(), file.second.length());
        }
        Serial.printf("Total files: %d, Used: %d/%d bytes\n", 
                     _mockFiles.size(), _usedBytes, _totalBytes);
        Serial.println("================================");
    }
    
    void setFileContent(const String& path, const String& content) {
        _mockFiles[path] = content;
        // Recalculate used bytes
        _usedBytes = 0;
        for (const auto& file : _mockFiles) {
            _usedBytes += file.second.length();
        }
    }
    
    String getFileContent(const String& path) const {
        auto it = _mockFiles.find(path);
        return (it != _mockFiles.end()) ? it->second : "";
    }
    
    void clear() {
        _mockFiles.clear();
        _usedBytes = 0;
    }
};
