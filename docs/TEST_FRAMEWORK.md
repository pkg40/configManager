# ConfigManager v2.0 Test Framework Documentation

## Overview

The configManager v2.0 library now includes a comprehensive test framework with multiple tiers of testing to ensure robust operation across all supported platforms.

## Test Architecture

### 1. Basic Test Library (`testLib.hpp`)
**Purpose**: Core functionality testing with detailed assertion framework
**Features**:
- ✅ Pass/Fail reporting with statistics
- ✅ 10 comprehensive test categories
- ✅ Performance benchmarking
- ✅ Memory usage monitoring
- ✅ Backward compatibility verification

**Test Categories**:
1. **Basic Operations**: Set/get, overwrite, non-existent keys
2. **FileSystem Provider**: Save/load operations through provider interface  
3. **Interface Compliance**: All iConfigProvider methods, polymorphic usage
4. **Data Persistence**: Save/load cycles, configuration integrity
5. **Error Handling**: Invalid inputs, edge cases, recovery
6. **Memory Management**: Usage tracking, heap monitoring
7. **Edge Cases**: Special characters, unicode, whitespace, case sensitivity
8. **Backward Compatibility**: v1.x API compatibility, method aliases
9. **Performance**: Bulk operations, timing benchmarks
10. **Real-world Scenarios**: IoT device configuration patterns

### 2. Advanced Test Suite (`advancedTestSuite_simple.hpp`)
**Purpose**: Stress testing and comprehensive edge case coverage
**Features**:
- 🧪 8 specialized test categories
- 🧪 Large dataset handling (5KB values, 50+ sections, 100+ keys)
- 🧪 High-volume operations (1000+ rapid operations)
- 🧪 Unicode and emoji support testing
- 🧪 JSON compliance verification
- 🧪 Performance edge case testing

**Test Categories**:
1. **Data Integrity**: Multi-type values, save/load cycles
2. **Large Datasets**: Memory limits, bulk operations
3. **Special Characters**: Unicode, JSON chars, control chars
4. **High-Volume Operations**: Rapid set/get performance
5. **Error Handling**: Empty keys, case sensitivity, overwrites
6. **Memory Management**: Growth tracking, heap monitoring
7. **Performance Edge Cases**: Enumeration speed, save/load timing
8. **JSON Compliance**: String preservation, serialization integrity

### 3. Comprehensive Test Bench (`testBench.cpp`)
**Purpose**: Multi-tier testing execution with real-world scenarios
**Features**:
- 🚀 4-tier test execution
- 🚀 Performance benchmarking with timing
- 🚀 Real-world IoT device simulation
- 🚀 Memory monitoring over time
- 🚀 Comprehensive result reporting

**Test Tiers**:
1. **Basic Functional Tests**: Core library validation
2. **Advanced Isolation Tests**: Edge cases and stress testing  
3. **Performance Benchmark**: Speed and memory optimization
4. **Real-World Scenario**: IoT device configuration lifecycle

## Usage Examples

### Quick Development Testing
```cpp
#include <testLib.hpp>

// Initialize your config manager
platformFileSystemProvider fsProvider;
configManager config(&fsProvider);
fsProvider.begin();

// Run quick tests
testLib::runV2ConfigTests(&config);
```

### Comprehensive Testing
```cpp
#include <testLib.hpp>

// Run full test suite (takes 2-3 minutes)
testLib::runAllTests(&config);
```

### Advanced Stress Testing
```cpp
#include <advancedTestSuite_simple.hpp>

// Run advanced tests (independent filesystem)
advancedTestSuite::runAdvancedTests();
```

### Full Test Bench (Multi-tier)
```cpp
// Use the testBench.cpp as main application
// Runs all test tiers sequentially with reporting
```

## Test Results Interpretation

### Success Indicators
- ✅ **Green PASS markers**: Individual tests passed
- 🎉 **ALL TESTS PASSED**: Complete success
- 📊 **100% success rate**: Perfect execution
- ⚡ **Performance metrics**: Speed within acceptable ranges

### Warning Indicators
- ⚠️ **Yellow warnings**: Non-critical issues (filesystem not available, using defaults)
- 📋 **Info messages**: Operational status updates

### Failure Indicators
- ❌ **Red FAIL markers**: Specific test failures
- 🚨 **Failed test count > 0**: Issues requiring attention
- 💀 **Compilation errors**: Code structure problems

## Performance Benchmarks

### Expected Performance (ESP32S3 @ 240MHz)
- **1000 setValue operations**: < 100ms (10,000+ ops/sec)
- **1000 getValue operations**: < 50ms (20,000+ ops/sec)
- **Save operation**: < 100ms
- **Load operation**: < 50ms
- **Section enumeration**: < 50ms
- **Key enumeration**: < 20ms
- **Memory usage**: < 50KB for large datasets

### Memory Usage Guidelines
- **Empty config**: ~500 bytes
- **Typical IoT config**: 1-3KB
- **Large dataset (50 sections, 100 keys)**: 10-30KB
- **Stress test (1000 keys)**: 40-80KB

## Platform Compatibility

### Tested Platforms
- ✅ **ESP32S3**: LittleFS + SPIFFS
- ✅ **ESP32**: LittleFS + SPIFFS  
- ✅ **ESP8266**: LittleFS + SPIFFS

### Build Validation
- All 7 PlatformIO environments compile successfully
- Cross-platform filesystem provider works correctly
- ArduinoJson 7.4.2 integration validated

## Best Practices

### Development Workflow
1. **Quick Tests**: Use `testLib::runV2ConfigTests()` during development
2. **Feature Testing**: Add custom tests using `testLib` assertion helpers
3. **Pre-release**: Run full `testLib::runAllTests()` 
4. **Stress Testing**: Execute `advancedTestSuite::runAdvancedTests()`
5. **Final Validation**: Complete `testBench.cpp` execution

### Performance Optimization
- Monitor memory usage with `getConfigMemoryUsage()`
- Use `printHeapStatus()` for memory leak detection
- Benchmark critical operations with `micros()` timing
- Test with realistic dataset sizes

### Error Handling
- Always check `saveConfig()` and `loadConfig()` return values
- Handle `[NOT FOUND]` responses gracefully  
- Test filesystem failure scenarios
- Validate configuration data after load operations

## Integration with CI/CD

### Automated Testing
- Use `advancedTestSuite::runAdvancedTests()` for deterministic results
- Parse test output for pass/fail statistics
- Monitor performance regression with benchmark results
- Validate memory usage stays within bounds

### Release Validation
- Run complete test bench on all target platforms
- Verify 100% test pass rate
- Confirm performance benchmarks within expected ranges
- Validate memory usage remains stable

## Extending the Test Framework

### Adding Custom Tests
```cpp
// In your test function
testLib::startTests();

// Your custom tests
config->setValue("custom", "test", "value");
testLib::assertEqual("Custom Test", config->getValue("custom", "test"), "value");

testLib::finishTests();
```

### Performance Testing
```cpp
unsigned long startTime = micros();
// Your operation to test
unsigned long duration = micros() - startTime;

testLib::assertTrue("Performance Test", duration < 10000, "Should complete in < 10ms");
```

### Memory Testing
```cpp
size_t beforeMemory = config->getConfigMemoryUsage();
// Add test data
size_t afterMemory = config->getConfigMemoryUsage();

testLib::assertTrue("Memory Growth", afterMemory > beforeMemory, "Memory should increase");
```

## Troubleshooting

### Common Issues
1. **Filesystem initialization fails**: Check board supports LittleFS/SPIFFS
2. **Tests timeout**: Reduce dataset size for slower platforms
3. **Memory errors**: Monitor heap usage, reduce test scope
4. **Performance regression**: Compare with baseline benchmarks

### Debug Information
- Enable verbose output in test functions
- Use `printConfigToSerial()` to inspect configuration state
- Monitor heap status with `printHeapStatus()`
- Check filesystem space with provider methods

The configManager v2.0 test framework provides comprehensive validation of all library functionality, ensuring reliable operation across all supported platforms and use cases.
