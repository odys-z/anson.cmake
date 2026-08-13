#include <gtest/gtest.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <regex>
#include <vector>
#include <string>
#include <io/odysz/anson.h>
#include "io/odysz/common.h"
#include "io/odysz/entt_jserv.h"
#include "io/odysz/json.h"

class AESHelperTest : public ::testing::Test {
public:
    /** To install the jar package, semantic.transact$ mvn install */
    inline static const std::string jarPath     = "../../../tests/res/semantics.transact-1.5.77-SNAPSHOT.jar";
    inline static const std::string ivbytes     = "iv-bytes.bin";
    inline static const std::string ivbase64    = "ivbase64.txt";
    inline static const std::string encrypted   = "encrypt_output.txt";
    inline static const std::string decrypting  = "decrypt_output.txt";

    inline static const std::string login_reply = "login_reply.json";
    inline static const std::string sessn_token = "session_token.txt";
    inline static const std::string token_verify= "token_verify.txt";
};

inline static std::string key = "Героям слава!";
inline static std::string slava_ukraini = "Слава Україні!";

using namespace anson;

TEST(T01_AESHelper, Base64) {
    aninfo("Copy semantic.transact/target/semantic.transct-#.#.#-jar-with-dependency.jar as *.jar here!");

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

TEST(T01_AESHelper, Decrypt) {
    using namespace anson;

    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    anwarn("This test can work on Windows only with system setting of Beta Utf-8.");
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
    else {
        FAIL() << "Result is not valid." << endl;
    }
}

TEST(T01_AESHelper, Encrypt) {
    vector<unsigned char> iv64 = AESHelper2::getRandom();
    string cipher = AESHelper2::encrypt(slava_ukraini, key, iv64);

    std::string input   = std::format("decrypt \"{}\" \"{}\" \"{}\"", key, AESHelper2::encode64(iv64), cipher);
    std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
                          AESHelperTest::jarPath, input, AESHelperTest::decrypting);

    anlog(command);
    int exitCode = std::system(command.c_str());
    ASSERT_EQ(exitCode, 0) << "Java process failed to execute.";

    std::string output = Utils::readOutputFile(AESHelperTest::decrypting);

    std::regex expectedPattern("plain:\\s+" + slava_ukraini);

    ASSERT_TRUE(std::regex_search(output, expectedPattern))
        << "Output did not match the regex pattern. Output was: " << endl << output;
}

TEST(T01_AESHelper, Verify_Tokon) {
    AstMap asts;
    JsonOpt ctx{&asts};
    register_asts(asts);
    register_jserv(&ctx);

    std::string uid = "ody", pswd = "123456";
    std::string login   = std::format("login \"{}\" \"{}\"", uid, pswd);
    std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
                                AESHelperTest::jarPath, login, AESHelperTest::login_reply);

    anlog(command);
    int exitCode = std::system(command.c_str());
    ASSERT_EQ(exitCode, 0) << "Java login failed!";

    std::string reply   = Utils::readOutputFile(AESHelperTest::login_reply);
    // vector<string_view> ssid_token = LangExt::split(reply, '\n');

    SessionInf ssinf;
    Anson::from_file(AESHelperTest::login_reply, ssinf, &ctx);

    vector<string_view> ss = LangExt::split(ssinf.ssToken, ':');
    string knows = AESHelper2::decrypt(string{ss[0]}, pswd, AESHelper2::decode64(string{ss[1]}));

    ssinf.ssToken = AESHelper2::repackSessionToken(ssinf.ssToken, pswd, uid);

    {
        // save for js tests
        std::ofstream ofstream(AESHelperTest::sessn_token);
        ofstream << ssinf.ssid << endl;
        ofstream << knows << endl;
        ofstream << ssinf.ssToken << endl;
    }

    std::string verify  = std::format("verify {} {} {} {}", ssinf.uid, pswd, ssinf.ssToken, knows);
    std::string vericmd = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
                                AESHelperTest::jarPath, verify, AESHelperTest::token_verify);
    aninfo(vericmd);
    exitCode = std::system(vericmd.c_str());
    ASSERT_EQ(exitCode, 0) << "Java verification failed!";

    std::string result = Utils::readOutputFile(AESHelperTest::token_verify);
    ASSERT_EQ("OK", result);
}
