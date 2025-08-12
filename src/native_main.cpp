#ifdef CONFIGMGR_NATIVE
#include "compat/native_arduino_compat.hpp"
#include "compat/nativeFileSystemProvider.hpp"
#include "configManager.hpp"
#include "../test/testLib.hpp"
#include "../test/advancedTestSuite_simple.hpp"

int main() {
    Serial.begin(115200);
    Serial.println("=== configManager Native Test Runner ===");

    nativeFileSystemProvider fs;
    configManager cfg(&fs, "native_config.json");
    cfg.loadConfig();

    testLib::runAllTests(&cfg);
    advancedTestSuite::runAdvancedTests();

    Serial.println("All native tests complete.");
    return 0;
}
#endif // CONFIGMGR_NATIVE
