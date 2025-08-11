// Native compatibility layer for configManager allowing PlatformIO native builds
#pragma once

#ifdef CONFIGMGR_NATIVE

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <cctype>
#include <algorithm>
#include <cstdint>
#include <cstdarg>

class String : public std::string {
public:
    using std::string::string;
    String() : std::string() {}
    String(const char* s) : std::string(s ? s : "") {}
    String(const std::string& s) : std::string(s) {}
    bool isEmpty() const { return empty(); }
    String substring(size_t from, size_t to = std::string::npos) const {
        if (from >= size()) return String("");
        if (to == std::string::npos || to > size()) to = size();
        if (to < from) return String("");
        return std::string::substr(from, to - from);
    }
    bool startsWith(const String& prefix) const {
        if (prefix.size() > size()) return false;
        return std::equal(prefix.begin(), prefix.end(), begin());
    }
    void toUpperCase() {
        std::transform(begin(), end(), begin(), [](unsigned char c){ return (unsigned char)std::toupper(c); });
    }
    void replace(const char* find, const char* repl) {
        if (!find || !*find) return;
        std::string f(find); std::string r(repl ? repl : "");
        size_t pos = 0;
        while ((pos = this->find(f, pos)) != std::string::npos) {
            this->std::string::replace(pos, f.length(), r);
            pos += r.length();
        }
    }
};

inline unsigned long millis() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return (unsigned long) std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}
inline void delay(unsigned long ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

class SerialClass {
public:
    void begin(unsigned long) {}
    template<typename T> void print(const T& v) { std::cout << v; }
    template<typename T> void println(const T& v) { std::cout << v << std::endl; }
    void println() { std::cout << std::endl; }
    int printf(const char* fmt, ...) {
        va_list args; va_start(args, fmt); int r = vprintf(fmt, args); va_end(args); return r; }
    operator bool() const { return true; }
};
static SerialClass Serial;

class File {
    std::fstream _fs;
public:
    File() {}
    File(const std::string& path, std::ios::openmode mode) { _fs.open(path, mode); }
    operator bool() const { return _fs.is_open(); }
    String readString() { std::stringstream buffer; buffer << _fs.rdbuf(); return buffer.str(); }
    size_t print(const String& s) { _fs << s; return s.size(); }
    void close() { if (_fs.is_open()) _fs.close(); }
};

#ifndef ARDUINO
#define ARDUINO 100
#endif
#ifndef F
#define F(x) x
#endif

#endif // CONFIGMGR_NATIVE
