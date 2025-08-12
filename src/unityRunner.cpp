#ifdef UNITY_EMBEDDED
#include <Arduino.h>
#include <unity.h>
#include <configManager.hpp>
#include <platformFileSystemProvider.hpp>

platformFileSystemProvider fsProvider;
configManager config(&fsProvider, "/unity_test.json");

// Persisted summary for periodic reprint
static volatile bool g_testsDone = false;
static int g_failures = -1;
static const int g_totalTests = 12; // keep in sync with RUN_TEST calls below

static void test_set_and_get_value() {
    config.clearConfig();
    config.setValue("section", "key", "value");
    TEST_ASSERT_EQUAL_STRING("value", config.getValue("section", "key").c_str());
}

static void test_overwrite_value() {
    config.clearConfig();
    config.setValue("section", "key", "value1");
    config.setValue("section", "key", "value2");
    TEST_ASSERT_EQUAL_STRING("value2", config.getValue("section", "key").c_str());
}

static void test_persistence() {
    config.clearConfig();
    config.setValue("persist", "k", "v");
    TEST_ASSERT_TRUE(config.saveConfig());
    config.clearConfig();
    TEST_ASSERT_TRUE(config.loadConfig());
    TEST_ASSERT_EQUAL_STRING("v", config.getValue("persist", "k").c_str());
}

static void test_load_before_use_then_set() {
    config.clearConfig();
    // load should succeed even if file may not exist yet; ensure save then load works
    (void)config.loadConfig();
    config.setValue("pre", "a", "1");
    TEST_ASSERT_TRUE(config.saveConfig());
    config.clearConfig();
    TEST_ASSERT_TRUE(config.loadConfig());
    TEST_ASSERT_EQUAL_STRING("1", config.getValue("pre", "a").c_str());
}

static void test_sections_and_keys_listing() {
    config.clearConfig();
    config.setValue("s1", "k1", "v1");
    config.setValue("s1", "k2", "v2");
    config.setValue("s2", "k1", "v3");
    auto sections = config.getSections();
    TEST_ASSERT_TRUE(sections.size() >= 2);
    auto keys_s1 = config.getKeys("s1");
    TEST_ASSERT_TRUE(keys_s1.size() >= 2);
    TEST_ASSERT_EQUAL_STRING("v2", config.getValue("s1", "k2").c_str());
}

static void test_hex_parser_valid_and_invalid() {
    config.clearConfig();
    uint8_t buf[4] = {0};
    // Valid: 8 hex chars -> 4 bytes
    TEST_ASSERT_TRUE(config.parseHexStringToBytes("0A0B0C0D", buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT8(0x0A, buf[0]);
    TEST_ASSERT_EQUAL_UINT8(0x0B, buf[1]);
    TEST_ASSERT_EQUAL_UINT8(0x0C, buf[2]);
    TEST_ASSERT_EQUAL_UINT8(0x0D, buf[3]);
    // Invalid length: odd chars
    TEST_ASSERT_FALSE(config.parseHexStringToBytes("ABC", buf, sizeof(buf)));
    // Invalid chars: non-hex
    TEST_ASSERT_FALSE(config.parseHexStringToBytes("ZZZZ", buf, sizeof(buf)));
}

static void test_clear_config_resets_values() {
    config.clearConfig();
    config.setValue("x", "y", "z");
    TEST_ASSERT_EQUAL_STRING("z", config.getValue("x", "y").c_str());
    config.clearConfig();
    TEST_ASSERT_EQUAL_STRING("[NOT FOUND]", config.getValue("x", "y").c_str());
}

static void test_auth_helpers_defaults() {
    config.clearConfig();
    // Ensure default indicates not found if nothing stored
    TEST_ASSERT_EQUAL_STRING("[NOT FOUND]", config.getUser().c_str());
    TEST_ASSERT_EQUAL_STRING("[NOT FOUND]", config.getPassword().c_str());
    // After set (write to the section the helpers read from)
    config.setValue("_auth", "user", "u1");
    config.setValue("_auth", "password", "p1");
    TEST_ASSERT_EQUAL_STRING("u1", config.getUser().c_str());
    TEST_ASSERT_EQUAL_STRING("p1", config.getPassword().c_str());
}

static void test_malformed_json_fallback() {
    // Write malformed JSON to the managed path, then ensure load falls back to defaults
    config.clearConfig();
    {
        File f = fsProvider.open("/unity_test.json", "w");
        TEST_ASSERT_TRUE(f);
        f.print("{ this is not valid json");
        f.close();
    }
    TEST_ASSERT_TRUE(config.loadConfig());
    // Defaults include section "updates" with key "topic" = "system/online"
    TEST_ASSERT_EQUAL_STRING("system/online", config.getValue("updates", "topic").c_str());
}

// Fake filesystem provider to simulate begin() failure
class FakeFailFS : public iFileSystemProvider {
public:
    bool begin() override { return false; }
    File open(const char*, const char*) override { return File(); }
    bool remove(const char*) override { return false; }
    bool exists(const char*) override { return false; }
};

static void test_filesystem_begin_failure_uses_defaults() {
    FakeFailFS badfs;
    configManager cm(&badfs, "/unity_fail.json");
    TEST_ASSERT_TRUE(cm.loadConfig());
    auto sections = cm.getSections();
    TEST_ASSERT_TRUE(sections.size() > 0);
}

static void test_bulk_large_config_persistence() {
    config.clearConfig();
    const int N = 200; // produce a few KB of JSON
    for (int i = 0; i < N; ++i) {
        String k = String("k") + i;
        String v = String("v") + i;
        config.setValue("bulk", k, v);
    }
    TEST_ASSERT_TRUE(config.saveConfig());
    config.clearConfig();
    TEST_ASSERT_TRUE(config.loadConfig());
    auto keys = config.getKeys("bulk");
    TEST_ASSERT_TRUE(keys.size() >= (size_t)N);
}

static void test_remove_file_then_load_defaults() {
    // Remove underlying file and ensure loadConfig still produces a non-empty config via defaults
    (void)fsProvider.remove("/unity_test.json");
    config.clearConfig();
    TEST_ASSERT_TRUE(config.loadConfig());
    TEST_ASSERT_TRUE(config.getSections().size() > 0);
}

void setup() {
    Serial.begin(115200);

    // --- Serial Conditioning -------------------------------------------------
    // Allow USB CDC enumeration (ESP32S3 native USB sometimes needs a window)
    delay(100);
    unsigned long _start = millis();
    // Wait up to 5s for monitor to attach; emit a dot every 250ms even if not connected
    while(!Serial && (millis() - _start) < 5000) {
        Serial.write('.');
        Serial.flush();
        delay(250);
    }
    // Short grace so first lines aren't truncated right after connection
    delay(100);
    while(Serial.available()) { Serial.read(); }
    Serial.println(); Serial.flush();
    Serial.println(F("[unity] configManager Unity test firmware booting")); Serial.flush();
    Serial.printf("[unity] Boot time: %lu ms\n", millis()); Serial.flush();
    Serial.println(F("[unity] Starting filesystem provider init...")); Serial.flush();
    fsProvider.begin();
    Serial.println(F("[unity] Filesystem provider init done")); Serial.flush();
    Serial.println(F("[unity] Beginning Unity test session")); Serial.flush();
    UNITY_BEGIN();
    RUN_TEST(test_set_and_get_value);
    RUN_TEST(test_overwrite_value);
    RUN_TEST(test_persistence);
    RUN_TEST(test_load_before_use_then_set);
    RUN_TEST(test_sections_and_keys_listing);
    RUN_TEST(test_hex_parser_valid_and_invalid);
    RUN_TEST(test_clear_config_resets_values);
    RUN_TEST(test_auth_helpers_defaults);
    RUN_TEST(test_malformed_json_fallback);
    RUN_TEST(test_filesystem_begin_failure_uses_defaults);
    RUN_TEST(test_bulk_large_config_persistence);
    RUN_TEST(test_remove_file_then_load_defaults);
    const int unityResult = UNITY_END();
    g_failures = unityResult;
    g_testsDone = true;
    Serial.printf("[unity] Test session complete, result code=%d\n", unityResult); Serial.flush();
    Serial.printf("[unity] Summary: tests=%d, failures=%d, status=%s\n",
                  g_totalTests, g_failures, (g_failures == 0 ? "PASS" : "FAIL"));
    Serial.flush();
    Serial.println(F("[unity] (If you missed initial output, summary will repeat periodically)")); Serial.flush();
}

void loop() {
    // Heartbeat and periodic summary repeat
    static unsigned long lastDot = 0;
    static unsigned long lastSummary = 0;
    unsigned long now = millis();
    if (now - lastDot > 2000) {
        lastDot = now;
        Serial.print('.');
        Serial.flush();
    }
    if (now - lastSummary > 15000) { // every 15s repeat tail summary
        lastSummary = now;
        Serial.println();
        if (g_testsDone) {
            Serial.printf("[unity] Summary: tests=%d, failures=%d, status=%s\n",
                          g_totalTests, g_failures, (g_failures == 0 ? "PASS" : "FAIL"));
        } else {
            Serial.println(F("[unity] (heartbeat) Waiting for tests..."));
        }
        Serial.println(F("[unity] Reset the board to rerun tests."));
        Serial.flush();
    }
    delay(25);
}
#endif // UNITY_EMBEDDED
