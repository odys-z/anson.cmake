#include <gtest/gtest.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <regex>
#include <vector>
#include <string>
#include "io/odysz/common.h"

class AESHelperTest : public ::testing::Test {
public:
    inline static const std::string jarPath = "../../../tests/res/semantics.transact-1.5.77-SNAPSHOT.jar";
    inline static const std::string ivbytes = "iv-bytes.bin";
    inline static const std::string ivbase64= "ivbase64.txt";
    inline static const std::string encrypted = "encrypt_output.txt";
    inline static const std::string decrypting= "decrypt_output.txt";
};

inline static std::string key = "Героям слава!";
inline static std::string slava_ukraini = "Слава Україні!";

TEST(AESHelper, Base64) {
    vector<unsigned char> iv = anson::AESHelper2::getRandom();
    anson::Utils::writeBinaryFile(AESHelperTest::ivbytes, iv);

    std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} encode64 {} > {}",
                          AESHelperTest::jarPath, AESHelperTest::ivbytes, AESHelperTest::ivbase64);
    anlog(command);

    int exitCode = std::system(command.c_str());
    ASSERT_EQ(exitCode, 0) << "command encode.";

    std::string output = anson::Utils::readOutputFile(AESHelperTest::ivbase64);
    ASSERT_EQ(anson::AESHelper2::encode64(iv), output);

    //
    command = std::format("java -Dfile.encoding=UTF-8 -jar {} decode64 {} {}",
                          AESHelperTest::jarPath, output, AESHelperTest::ivbytes);
    anlog(command);
    exitCode = std::system(command.c_str());
    ASSERT_EQ(exitCode, 0) << "command decode.";

    std::vector<unsigned char> decoded_iv = anson::Utils::readBinaryFile(AESHelperTest::ivbytes);

    ASSERT_EQ(iv.size(), decoded_iv.size());
    ASSERT_EQ(iv, decoded_iv) << "Binary mismatch after Java decode64 cycle";
}

TEST(AESHelper, Decrypt) {
    using namespace anson;

    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    anlog("Current working dir: "s + buff);

    std::string input = std::format("encrypt \"{}\" \"{}\"", key, slava_ukraini);
    std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
                          AESHelperTest::jarPath, input, AESHelperTest::encrypted);

    anlog(command);
    int exitCode = std::system(command.c_str());
    ASSERT_EQ(exitCode, 0) << "Java process failed to execute.";

    std::string output = anson::Utils::readOutputFile(AESHelperTest::encrypted);

    std::regex expectedPattern("re-decrpyted:\\s+" + slava_ukraini);

    ASSERT_TRUE(std::regex_search(output, expectedPattern))
        << "Output did not match the regex pattern. Output was: " << endl << output;

    // get cipher and decrypt
    std::regex pattern(R"(cipher:\s*(.*))");
    std::smatch matches;
   
    if (std::regex_search(output, matches, pattern)) {
        std::string b64_cipher = LangExt::trim(matches[1].str());
        anlog("Cipher: " + b64_cipher);

        std::regex iv_pattern(R"(iv:\s*(.+))");
        std::smatch iv_matches;
        ASSERT_TRUE(std::regex_search(output, iv_matches, iv_pattern));
        std::string b64_iv = iv_matches[1].str();

        std::vector<unsigned char> iv = AESHelper2::decode64(b64_iv);

        string decrypted = AESHelper2::decrypt(b64_cipher, key, iv);
        if (decrypted.length() == 0)
            FAIL() << "decrypt(): null";

        else
            ASSERT_EQ(decrypted, slava_ukraini);
    }
}

TEST(AESHelper, Encrypt) {

    using namespace anson;

    vector<unsigned char> iv64 = AESHelper2::getRandom();
    string cipher = AESHelper2::encrypt(slava_ukraini, key, iv64);

    std::string input   = std::format("decrypt \"{}\" \"{}\" \"{}\"", key, AESHelper2::encode64(iv64), cipher);
    std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
                          AESHelperTest::jarPath, input, AESHelperTest::decrypting);

    anlog(command);
    int exitCode = std::system(command.c_str());
    ASSERT_EQ(exitCode, 0) << "Java process failed to execute.";

    std::string output = anson::Utils::readOutputFile(AESHelperTest::decrypting);

    std::regex expectedPattern("plain:\\s+" + slava_ukraini);

    ASSERT_TRUE(std::regex_search(output, expectedPattern))
        << "Output did not match the regex pattern. Output was: " << endl << output;
}
