#include <gtest/gtest.h>
#include <string>
#include <optional>
#include <filesystem>
#include <io/odysz/utils.h>

using namespace anson;

namespace {

/** Sets (or, with nullopt, removes) an environment variable; value is UTF-8. */
void set_env(const char* name, const std::optional<std::string>& utf8) {
#ifdef _WIN32
    std::wstring wname(name, name + std::char_traits<char>::length(name));
    std::wstring wval = utf8
        ? std::filesystem::path(std::u8string(utf8->begin(), utf8->end())).wstring()
        : std::wstring();
    _wputenv((wname + L"=" + wval).c_str());    // "NAME=" removes it; _wputenv for MinGW msvcrt
#else
    if (utf8) setenv(name, utf8->c_str(), 1);
    else unsetenv(name);
#endif
}

/** The variable that EnvPath::home() reads first on this platform. */
const char* home_var() {
#ifdef _WIN32
    return "USERPROFILE";
#else
    return "HOME";
#endif
}

/** Saves and restores HOME, USERPROFILE, VOLUME_HOME and EnvPath::volume_home(). */
class T16_EnvPath : public ::testing::Test {
protected:
    std::optional<std::string> home, userprofile, volume;

    void SetUp() override {
        home        = EnvPath::getEnv("HOME");
        userprofile = EnvPath::getEnv("USERPROFILE");
        volume      = EnvPath::getEnv("VOLUME_HOME");

        set_env(home_var(), "/home/ody");
        set_env("VOLUME_HOME", std::nullopt);
        EnvPath::volume_home("");
    }

    void TearDown() override {
        set_env("HOME", home);
        set_env("USERPROFILE", userprofile);
        set_env("VOLUME_HOME", volume);
        EnvPath::volume_home("");
    }
};

} // namespace

TEST_F(T16_EnvPath, Tilde) {
    EXPECT_EQ("/home/ody",       resolveHomePath("~"));
    EXPECT_EQ("/home/ody/a/b",   resolveHomePath("~/a/b"));
    EXPECT_EQ("/home/ody\\a",    resolveHomePath("~\\a"));

    // only a leading ~, and no ~user
    EXPECT_EQ("~user/a",         resolveHomePath("~user/a"));
    EXPECT_EQ("a/~/b",           resolveHomePath("a/~/b"));

    // TEST_F(T16_EnvPath, Home) {
    EXPECT_EQ("/home/ody/x",     resolveHomePath("$HOME/x"));
    EXPECT_EQ("x//home/ody",     resolveHomePath("x/$HOME"));
    EXPECT_EQ("$HOMEX/x",        resolveHomePath("$HOMEX/x"));     // whole name only

    // a trailing separator of home is not doubled for ~
    set_env(home_var(), "/home/ody/");
    EXPECT_EQ("/home/ody/a",     resolveHomePath("~/a"));

    // TEST_F(T16_EnvPath, VolumeHome) {
    // not set: kept as is (and warned), so the name shows up where the path is reported
    EXPECT_EQ("$VOLUME_HOME/syntity.json", resolveHomePath("$VOLUME_HOME/syntity.json"));

    // from the environment; '\' -> '/', and '$' in the value is literal
    set_env("VOLUME_HOME", "D:\\vol$1");
    EXPECT_EQ("D:/vol$1/s.db",   resolveHomePath("$VOLUME_HOME/s.db"));

    // set by the app: takes precedence; a leading ~ in the value is expanded
    EnvPath::volume_home("~/vol");
    EXPECT_EQ("/home/ody/vol/s.db", resolveHomePath("$VOLUME_HOME/s.db"));

    // cleared: back to the environment
    EnvPath::volume_home("");
    EXPECT_EQ("D:/vol$1/s.db",   resolveHomePath("$VOLUME_HOME/s.db"));
}

TEST_F(T16_EnvPath, OtherNamesAreLiteral) {
    set_env("VOLUME_HOME", "/v");
    EXPECT_EQ("/v//home/ody/$JAVA_HOME", resolveHomePath("$VOLUME_HOME/$HOME/$JAVA_HOME"));
    EXPECT_EQ("price$5/a",       resolveHomePath("price$5/a"));
    EXPECT_EQ("a$",              resolveHomePath("a$"));
    EXPECT_EQ("$",               resolveHomePath("$"));
    EXPECT_EQ("",                resolveHomePath(""));

    // TEST_F(T16_EnvPath, Utf8) {
    EXPECT_EQ("/opt/日本語//home/ody", resolveHomePath("/opt/日本語/$HOME"));

    set_env(home_var(), "/home/奥迪");
    EXPECT_EQ("/home/奥迪/vol",  resolveHomePath("~/vol"));
}

#ifdef _WIN32
TEST_F(T16_EnvPath, WindowsHome) {
    // USERPROFILE wins over HOME, and its backslashes become '/'
    set_env("HOME", "C:\\msys\\home\\ody");
    set_env("USERPROFILE", "C:\\Users\\奥迪");
    EXPECT_EQ("C:/Users/奥迪/AppData", resolveHomePath("~/AppData"));
    EXPECT_EQ("C:/Users/奥迪/AppData", resolveHomePath("$HOME/AppData"));

    // HOME is the fallback
    set_env("USERPROFILE", std::nullopt);
    EXPECT_EQ("C:/msys/home/ody/x", resolveHomePath("~/x"));
}
#endif
