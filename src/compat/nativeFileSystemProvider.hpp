#pragma once
#ifdef CONFIGMGR_NATIVE
#include "native_arduino_compat.hpp"
#include "interface/iFileSystemProvider.hpp"
#include <unordered_map>

class nativeFileSystemProvider : public iFileSystemProvider {
public:
    bool begin() override { return true; }
    File open(const char* path, const char* mode) override {
        std::ios::openmode m = std::ios::binary;
        if (mode && mode[0] == 'w') m |= std::ios::out | std::ios::trunc; else m |= std::ios::in;
        return File(path, m);
    }
    bool remove(const char* path) override { return std::remove(path) == 0; }
    bool exists(const char* path) override { std::ifstream f(path); return f.good(); }
};
#endif // CONFIGMGR_NATIVE
