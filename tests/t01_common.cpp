#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>

#include "io/odysz/common.h"

using namespace anson;

struct JservTestCase {
    bool ok;
    std::string url;
    bool https;
    vector<int> portRange;
    vector<string> paths;
    bool ipv6;
};

static bool validPaths(const vector<string> &expect, const vector<string> &actual) {
    if (expect.size() != actual.size()) return false;

    for (size_t i = 0; i < expect.size(); ++i) {
        if (expect[i] !=  actual[i])
            return false;
    }
    return true;
}

TEST(RegexTest, TestIsHttps) {
    EXPECT_TRUE(Regex::isHttps("https://odys-z.github.io"));
    EXPECT_TRUE(Regex::isHttp("http://odys-z.github.io"));
    EXPECT_FALSE(Regex::isHttps("http://odys-z.github.io"));

}

TEST(RegexTest, TestUrlIlligalChar) {
    UrlValidator v;
    EXPECT_FALSE(v.isValid("http://odys-z.github.io/notes /"));
    EXPECT_TRUE(v.isValid("http://odys-z.github.io/notes%20/"));

    // In java, this is a invalid authority by Apache UrlValidator;
    // in cpp, authority validation is ignored by boots.url.
    EXPECT_TRUE(v.isValid(Regex::asJserv("//odys-z.github.io%20")));
}

TEST(RegexTest, TestValidJserv) {
    const std::vector<JservTestCase> urls = {
        {true,  "https://odys-z.github.io:443/notes/index.html?v=1&w=2#rave", true,  {80, 443},    {"notes", "index.html"}, false},
        {false, "https://odys-z.github.io/notes/index.html#rave?v=1&w=2",     true,  {1024, -1},   {"notes", "index.html"}, false},
        {true,  "//odys-z.github.io/notes/index.html#rave?v=1&w=2",           false, {80, 1024},   {"notes", "index.html"}, false},
        {true,  "//odys-z.github.io/notes/index.html#rave?v=1&w=2",           false, {80, 1024},   {"notes", "index.html"}, false},
        {true,  "//odys-z.github.io/notes/",                                  false, {80, 1024},   {"notes"}, false},
        {true,  "//odys-z.github.io/notes%20/",                               false, {80, 1024},   {"notes%20"}, false},
        {false, "//odys-z.github.io/notes /",                                 false, {80, 1024},   {"notes "}, false},
        {true,  "//odys-z.github.io/notes%20",                                false, {80, 1024},   {"notes%20"}, false},

        {true,  "//odys-z.github.io/",                                        false, {80, 1024},   {}, false},
        {true,  "//odys-z.github.io",                                         false, {80, 1024},   {}, false},
        {true,  // false in java, see TestUrlIlligalChar
                "//odys-z.github.io%20",                                      false, {80, 1024},   {}, false},
        {true,  // false in java, see TestUrlIlligalChar
                "//odys-z.github.io%20/notes%20",                             false, {80, 1024},   {"notes%20"}, false},
        {true,  "odys-z.github.io/notes/index.html#rave?v=1&w=2",             false, {80, 1024},   {"notes", "index.html"}, false},
        {false, "odys-z.github.io/notes/index.html#rave?v=1&w=2",             false, {81, 1024},   {"notes", "index.html"}, false},
        {true,  "https://odys-z.github.io/notes/index.html",                  true,  {443, 1024},  {"notes", "index.html"}, false},
        {false, "https://odys-z.github.io/notes/index.html",                  true,  {1024, -1},   {"notes", "index.html"}, false},
        {false, "https://127.0.0.1/jserv-album",                              true,  {1024, -1},   {"jserv-album"}, false},
        {true,  "https://127.0.0.1:8964/jserv-album",                         true,  {1024, -1},   {"jserv-album"}, false},
        {false, "//127.0.0.1/jserv-album",                                    true,  {1024, -1},   {"jserv-album"}, false},
        {true,  "127.0.0.1:8964/jserv-album",                                 false, {1024, -1},   {"jserv-album"}, false},
        {false, "https://::1/jserv-album",                                    true,  {1024, -1},   {"jserv-album"}, false},
        {true,  "https://[::3]:8964/jserv-album",                             true,  {1024, -1},   {"jserv-album"}, true},
        {false, "//2604:9cc0:14:b140:5706:4ab0:6cb8:d348/jserv-album",        true,  {80, -1},     {"jserv-album"}, false},
        {true,  "https://[2604:9cc0:14:b140:5706:4ab0:6cb8:d348]/jserv-album",true,  {443, -1},    {"jserv-album"}, true},
        // TODO {true,  "[2604:9cc0:14:b140:5706:4ab0:6cb8:d348]:8964/jserv-album",   false, {1024, -1},   {"jserv-album"}, true}
    };

    // Test isIPv6
    for (const auto& entry : urls) {
        EXPECT_EQ(entry.ipv6, Regex::isIPv6(entry.url)) << "Failed IPv6 test for: " << entry.url;
    }

    UrlValidator urlValidator;

    // Validation Loop
    for (const JservTestCase& test : urls) {
        HttpParts parts;
        string jserv_test = Regex::asJserv(test.url);
        Regex::getHttpParts(jserv_test, parts);

        bool isValid = urlValidator.isValid(jserv_test) &&
                       Regex::isHttps(test.url) == test.https &&
                       validUrlPort(parts.port, test.portRange) &&
                       validPaths(test.paths, parts.paths);

        EXPECT_EQ(test.ok, isValid) << "Failed Jserv validation for: " << test.url;
    }
}

TEST(RegexTest, TestIsEnvelope) {

    EXPECT_TRUE(Regex::startEnvelope("'{\"type\": \"com.examples.test\"}"));
    EXPECT_TRUE(Regex::startEnvelope("'{ \"type\": \"com.examples.test\"}"));
    EXPECT_TRUE(Regex::startEnvelope("'{\t\"type\": \"com.examples.test\"}"));
    EXPECT_TRUE(Regex::startEnvelope("'{\n\"type\": \"com.examples.test\"}"));
    EXPECT_TRUE(Regex::startEnvelope("{\"type\": \"com.examples.test\"}"));
    EXPECT_TRUE(Regex::startEnvelope("{\n\"type\": \"com.examples.test\"}"));

    EXPECT_FALSE(Regex::startEnvelope("{type: \"com.examples.test\"}"));
    EXPECT_FALSE(Regex::startEnvelope("'{type: \"com.examples.test\"}"));
    EXPECT_FALSE(Regex::startEnvelope("{type, \"com.examples.test\"}"));

    EXPECT_TRUE(Regex::startEnvelope("{'type': \"com.examples.test\"}"));
}

TEST(RegexTest, TestPareseTypes) {
    ASSERT_EQ((std::vector<std::string>{"string", "false"}),
              Regex::parse_val_type("string")) << "0.1";

    ASSERT_EQ((std::vector<std::string>{"string", "true"}),
              Regex::parse_val_type("*string")) << "0.2";

    ASSERT_EQ((std::vector<std::string>{"string", "true"}),
              Regex::parse_val_type("shared_ptr<string")) << "0.3";

    ASSERT_EQ((std::vector<std::string>{"map<string", "false"}),
              Regex::parse_val_type("map<string")) << "0.4";

    ASSERT_EQ((std::vector<std::string>{"map<string", "true"}),
              Regex::parse_val_type("shared_ptr<map<string")) << "0.5";

    ASSERT_EQ((std::vector<std::string>{"map<string", "true"}),
              Regex::parse_val_type("*map<string")) << "0.6";

    ASSERT_EQ((std::vector<std::string>{"T_List", "true"}),
              Regex::parse_val_type("shared_ptr<T_List")) << "0.7";

    ASSERT_EQ((std::vector<std::string>{"T_List", "false"}),
              Regex::parseListValtype(R"(list<T_List)")) << "::1";

    ASSERT_EQ((std::vector<std::string>{"T_List", "true"}),
              Regex::parseListValtype(R"(list<shared_ptr<T_List)")) << "::2";

    ASSERT_EQ((std::vector<std::string>{"T_List", "true"}),
              Regex::parseListValtype(R"(list<* T_List)")) << "::3";

    ASSERT_EQ((std::vector<std::string>{"*T_List", "true"}),
              Regex::parseListValtype(R"(list<**T_List)")) << "::4";

    ASSERT_EQ((std::vector<std::string>{"list<string", "false"}),
              Regex::parseListValtype(R"(list<list<string)")) << "::5";


    ASSERT_EQ((std::vector<std::string>{"string", "false"}),
              Regex::parseMapValtype(R"(map<string, string)")) << "::5.1";

    ASSERT_EQ((std::vector<std::string>{"non_list<T_List", "false"}),
              Regex::parseListValtype(R"(non_list<T_List)")) << "::6";

    ASSERT_EQ((std::vector<std::string>{"T_List", "false"}),
              Regex::parseMapValtype(R"(map<string, T_List)")) << "::7";

    ASSERT_EQ((std::vector<std::string>{"T_List", "true"}),
              Regex::parseMapValtype(R"(map<string, shared_ptr<T_List)")) << "::8";

    // Robustness
    ASSERT_EQ((std::vector<std::string>{"map.string, shared_ptr<T_List", "false"}),
              Regex::parseMapValtype(R"(map.string, shared_ptr<T_List)")) << "::-1";
}

class AESHelperTest : public ::testing::Test {
public:
    inline static const std::string jarPath = "../../../tests/res/semantics.transact-1.5.77-SNAPSHOT.jar";
    inline static const std::string tempFile = "test_output.txt";

    // Helper to read the file content
    inline static std::string readOutputFile() {
        std::ifstream file(tempFile);
        std::string content, line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }
        return content;
    }
};

// #include <botan/block_cipher.h>
// #include <botan/cipher_mode.h>
// #include <botan/hex.h>
// #include <iostream>
// #include <vector>

// int gemini_example_main() {
//     try {
//         // 1. Setup Keys and IV (Must match the Java side exactly)
//         // For AES-256, use a 32-byte key and 16-byte IV
//         const std::vector<uint8_t> key = Botan::hex_decode("000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F");
//         const std::vector<uint8_t> iv  = Botan::hex_decode("000102030405060708090A0B0C0D0E0F");

//         std::string plaintext = "Hello from C++ (Botan)!";
//         std::vector<uint8_t> data(plaintext.begin(), plaintext.end());

//         // 2. Create the Cipher object
//         // "AES/CBC/PKCS7" matches Java's "AES/CBC/PKCS5Padding"
//         std::unique_ptr<Botan::Cipher_Mode> enc =
//             Botan::Cipher_Mode::create("AES/CBC/PKCS7", Botan::Cipher_Dir::Encryption);

//         if (!enc) {
//             std::cerr << "Cipher not found!" << std::endl;
//             return 1;
//         }

//         // 3. Initialize with Key and IV
//         enc->set_key(key);
//         enc->start(iv);

//         // 4. Perform Encryption
//         // Botan can process data in-place
//         enc->finish(data);

//         std::cout << "Encrypted (Hex): " << Botan::hex_encode(data) << std::endl;

//         // --- Decryption Example ---
//         std::unique_ptr<Botan::Cipher_Mode> dec =
//             Botan::Cipher_Mode::create("AES/CBC/PKCS7", Botan::Cipher_Dir::Decryption);

//         dec->set_key(key);
//         dec->start(iv);
//         dec->finish(data);

//         std::string decrypted_text(data.begin(), data.end());
//         std::cout << "Decrypted: " << decrypted_text << std::endl;

//     } catch (std::exception& e) {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }

//     return 0;
// }

// inline static std::string key = R"("Героям слава!")";

// TEST(AESHelper, Decrypt) {
//     char buff[FILENAME_MAX];
//     _getcwd(buff, FILENAME_MAX);
//     anlog("Current working dir: "s + buff);

//     std::string input = std::format("encrypt {} \"Слава Україні!\"", key);
//     std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
//                           AESHelperTest::jarPath, input, AESHelperTest::tempFile);

//     anlog(command);
//     int exitCode = std::system(command.c_str());
//     ASSERT_EQ(exitCode, 0) << "Java process failed to execute.";

//     std::string output = AESHelperTest::readOutputFile();

//     std::regex expectedPattern("re-decrpyted:\\s+Слава Україні!");

//     ASSERT_TRUE(std::regex_search(output, expectedPattern))
//         << "Output did not match the regex pattern. Output was: " << endl << output;

//     // get cipher and decrypt
//     std::regex pattern(R"(ciper:\s*([^\s]+))");
//     std::smatch matches;

//     if (std::regex_search(next, matches, pattern)) {
//         // 1. Extract and Base64 decode the IV and Ciphertext
//         // You'll need another regex search for the IV line based on your output text
//         std::regex iv_pattern(R"(iv:\s*([^\s]+))");
//         std::smatch iv_matches;
//         if (!std::regex_search(output, iv_matches, iv_pattern)) {
//             FAIL() << "Could not find IV in output!";
//         }

//         std::string base64_iv = iv_matches[1].str();
//         std::string base64_cipher = matches[1].str(); // From your existing cipher match

//         // Convert Base64 strings to raw bytes
//         std::vector<uint8_t> iv_bytes = Botan::base64_decode(base64_iv);
//         std::vector<uint8_t> cipher_bytes = Botan::base64_decode(base64_cipher);

//         // 2. Prepare the Key
//         // Ensure 'key' is converted to raw bytes.
//         // If it's a raw string: std::vector<uint8_t> key_bytes(key.begin(), key.end());
//         // Note: If your Java side uses a hashed key, ensure you do the same here.
//         std::vector<uint8_t> key_bytes(key.begin(), key.end());

//         try {
//             // 3. Create the Decryptor
//             // "AES/CBC/PKCS7" matches Java's "AES/CBC/PKCS5Padding"
//             std::unique_ptr<Botan::Cipher_Mode> dec =
//                 Botan::Cipher_Mode::create("AES/CBC/PKCS7", Botan::Cipher_Dir::Decryption);

//             if (!dec) {
//                 FAIL() << "Botan could not create the AES/CBC/PKCS7 cipher mode.";
//             }

//             // 4. Decrypt
//             dec->set_key(key_bytes);
//             dec->start(iv_bytes);

//             // Botan decrypts in-place
//             dec->finish(cipher_bytes);

//             // 5. Verify the result
//             std::string decrypted_result(reinterpret_cast<const char*>(cipher_bytes.data()), cipher_bytes.size());

//             anlog("C++ Decrypted Result: " + decrypted_result);
//             ASSERT_EQ(decrypted_result, "Слава Україні!");

//         } catch (std::exception& e) {
//             FAIL() << "Botan Decryption failed: " << e.what();
//         }
//     } else {
//         Fail("Cannot find cipher in output.txt!");
//     }
// }

// TEST(AESHelper, Encrypt) {
//     char buff[FILENAME_MAX];
//     _getcwd(buff, FILENAME_MAX);
//     anlog("Current working dir: "s + buff);

//     std::string iv64    = "9PvT0X2hwBpPRevuu++3eg==";
//     std::string cipher  = "ocUPIn3aFlYGKKF9x3E3Bm2q/9K3WzX6gKc8Mslt76w="; // => "Слава Україні!"

//     std::string input   = std::format("decrypt \"{}\" \"{}\" \"{}\"", key, iv64, cipher);
//     std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
//                           AESHelperTest::jarPath, input, AESHelperTest::tempFile);

//     anlog(command);
//     int exitCode = std::system(command.c_str());
//     ASSERT_EQ(exitCode, 0) << "Java process failed to execute.";

//     std::string output = AESHelperTest::readOutputFile();

//     std::regex expectedPattern("plain:\\s+Слава Україні!");

//     ASSERT_TRUE(std::regex_search(output, expectedPattern))
//         << "Output did not match the regex pattern. Output was: " << endl << output;
// }
