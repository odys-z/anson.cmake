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
    EXPECT_EQ("/home/ody",       EnvPath::replaceEnv("~"));
    EXPECT_EQ("/home/ody/a/b",   EnvPath::replaceEnv("~/a/b"));
    EXPECT_EQ("/home/ody\\a",    EnvPath::replaceEnv("~\\a"));

    // only a leading ~, and no ~user
    EXPECT_EQ("~user/a",         EnvPath::replaceEnv("~user/a"));
    EXPECT_EQ("a/~/b",           EnvPath::replaceEnv("a/~/b"));

    // TEST_F(T16_EnvPath, Home) {
    EXPECT_EQ("/home/ody/x",     EnvPath::replaceEnv("$HOME/x"));
    EXPECT_EQ("x//home/ody",     EnvPath::replaceEnv("x/$HOME"));
    EXPECT_EQ("$HOMEX/x",        EnvPath::replaceEnv("$HOMEX/x"));     // whole name only

    // a trailing separator of home is not doubled for ~
    set_env(home_var(), "/home/ody/");
    EXPECT_EQ("/home/ody/a",     EnvPath::replaceEnv("~/a"));

    // TEST_F(T16_EnvPath, VolumeHome) {
    // not set: kept as is (and warned), so the name shows up where the path is reported
    EXPECT_EQ("$VOLUME_HOME/syntity.json", EnvPath::replaceEnv("$VOLUME_HOME/syntity.json"));

    // from the environment; '\' -> '/', and '$' in the value is literal
    set_env("VOLUME_HOME", "D:\\vol$1");
    EXPECT_EQ("D:/vol$1/s.db",   EnvPath::replaceEnv("$VOLUME_HOME/s.db"));

    // set by the app: takes precedence; a leading ~ in the value is expanded
    EnvPath::volume_home("~/vol");
    EXPECT_EQ("/home/ody/vol/s.db", EnvPath::replaceEnv("$VOLUME_HOME/s.db"));

    // cleared: back to the environment
    EnvPath::volume_home("");
    EXPECT_EQ("D:/vol$1/s.db",   EnvPath::replaceEnv("$VOLUME_HOME/s.db"));
}

TEST_F(T16_EnvPath, OtherNamesAreLiteral) {
    set_env("VOLUME_HOME", "/v");
    EXPECT_EQ("/v//home/ody/$JAVA_HOME", EnvPath::replaceEnv("$VOLUME_HOME/$HOME/$JAVA_HOME"));
    EXPECT_EQ("price$5/a",       EnvPath::replaceEnv("price$5/a"));
    EXPECT_EQ("a$",              EnvPath::replaceEnv("a$"));
    EXPECT_EQ("$",               EnvPath::replaceEnv("$"));
    EXPECT_EQ("",                EnvPath::replaceEnv(""));

    // TEST_F(T16_EnvPath, Utf8) {
    EXPECT_EQ("/opt/日本語//home/ody", EnvPath::replaceEnv("/opt/日本語/$HOME"));

    set_env(home_var(), "/home/奥迪");
    EXPECT_EQ("/home/奥迪/vol",  EnvPath::replaceEnv("~/vol"));
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
