#pragma once
// Stub Arduino.h for native build
#include "native_arduino_compat.hpp"
// Minimal Arduino.h stub for native build
#pragma once
#ifdef CONFIGMGR_NATIVE
#include "native_arduino_compat.hpp"

// PROGMEM / Flash string helpers (no-op on native)
#ifndef PROGMEM
#define PROGMEM
#endif
using __FlashStringHelper = char;
#define F(x) x
inline char pgm_read_byte(const void* p){ return *reinterpret_cast<const char*>(p); }
inline const void* pgm_read_ptr(const void* p){ return p; }

// Basic Print / Stream emulation for ArduinoJson compatibility
class Print {
public:
	virtual size_t write(uint8_t c){ std::cout << (char)c; return 1; }
	size_t print(const String& s){ std::cout << s; return s.length(); }
	size_t println(const String& s){ std::cout << s << std::endl; return s.length()+1; }
};
class Stream : public Print {
public:
	int available(){ return 0; }
	int read(){ return -1; }
};

#endif // CONFIGMGR_NATIVE
