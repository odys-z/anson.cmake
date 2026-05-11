#include <gtest/gtest.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include "io/odysz/common.h"
#include "io/odysz/anson.h"

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

std::string key = "Героям слава!";

// Helper to decode Base64 strings (as OpenSSL's BIO is verbose)
// std::vector<unsigned char> base64_decode(const std::string& base64) {
//     BIO *bio, *b64;
//     std::vector<unsigned char> buffer(base64.length());
//     bio = BIO_new_mem_buf(base64.c_str(), -1);
//     b64 = BIO_new(BIO_f_base64());
//     bio = BIO_push(b64, bio);
//     BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Important: No newlines
//     int length = BIO_read(bio, buffer.data(), base64.length());
//     buffer.resize(length);
//     BIO_free_all(bio);
//     return buffer;
// }

TEST(AESHelper, Decrypt) {
    using namespace anson;

    char buff[FILENAME_MAX];
    _getcwd(buff, FILENAME_MAX);
    anlog("Current working dir: "s + buff);

    std::string input = std::format("encrypt \"{}\" \"Слава Україні!\"", key);
    std::string command = std::format("java -Dfile.encoding=UTF-8 -jar {} {} > {}",
                          AESHelperTest::jarPath, input, AESHelperTest::tempFile);

    anlog(command);
    int exitCode = std::system(command.c_str());
    ASSERT_EQ(exitCode, 0) << "Java process failed to execute.";

    std::string output = AESHelperTest::readOutputFile();

    std::regex expectedPattern("re-decrpyted:\\s+Слава Україні!");

    ASSERT_TRUE(std::regex_search(output, expectedPattern))
        << "Output did not match the regex pattern. Output was: " << endl << output;

    // get cipher and decrypt
    std::regex pattern(R"(cipher:\s*(.*))");
    std::smatch matches;
   
    if (std::regex_search(output, matches, pattern)) {
        std::string b64_cipher = LangExt::trim(matches[1].str());
        anlog("Cipher: " + b64_cipher);

        // Extract IV from Java output
        std::regex iv_pattern(R"(iv:\s*([^\s]+))");
        std::smatch iv_matches;
        ASSERT_TRUE(std::regex_search(output, iv_matches, iv_pattern));
        std::string b64_iv = iv_matches[1].str();

        std::vector<unsigned char> iv = AESHelper2::base64_decode(b64_iv);

        string decrypted = AESHelper2::decrypt(b64_cipher, key, iv);
        if (decrypted.length() == 0)
            FAIL() << "decrypt(): null";

        else
            ASSERT_EQ(decrypted, "Слава Україні!");

    //     EVP_CIPHER_CTX *ctx;
    //     try {
    //         // Setup OpenSSL Decryption
    //         ctx = EVP_CIPHER_CTX_new(); // Performance overhead but not thread safe

    //         // 1. Decode inputs
    //         std::vector<unsigned char> cipherbytes = AESHelper2::base64_decode(b64_cipher);
    //         std::vector<unsigned char> key_bytes = AESHelper2::hash_key_sha256(key);

    //         int len, plain_len;

    //         std::vector<unsigned char> plainbytes(cipherbytes.size());

    //         // Initialize Decryption: AES-128-CBC or AES-256-CBC depending on key size
    //         // Java's "AES/CBC/PKCS5Padding" is equivalent to OpenSSL's default PKCS7 padding
    //         const EVP_CIPHER* cipherType = nullptr;

    //         if (key_bytes.size() == 16) {
    //             cipherType = EVP_aes_128_cbc();
    //         } else if (key_bytes.size() == 32) {
    //             cipherType = EVP_aes_256_cbc();
    //         } else {
    //             throw std::runtime_error(std::format("Unsupported key size: {}", key_bytes.size()));
    //         }

    //         if(EVP_DecryptInit_ex(ctx, cipherType, NULL, key_bytes.data(), iv.data()) != 1)
    //             throw std::runtime_error("Init failed");

    //         if(EVP_DecryptUpdate(ctx, plainbytes.data(), &len, cipherbytes.data(), cipherbytes.size()) != 1)
    //             throw std::runtime_error("Update failed");
    //         plain_len = len;

    //         if(EVP_DecryptFinal_ex(ctx, plainbytes.data() + len, &len) != 1)
    //             throw std::runtime_error("Finalize failed (Check your key/padding)");
    //         plain_len += len;

    //         std::string decrypted_text(reinterpret_cast<char*>(plainbytes.data()), plain_len);
    //         anlog("OpenSSL Decrypted: " + decrypted_text);
            
    //         ASSERT_EQ(decrypted_text, "Слава Україні!");

    //     } catch (const std::exception& e) {
    //         if (ctx) EVP_CIPHER_CTX_free(ctx);
    //         FAIL() << "OpenSSL error: " << e.what();
    //     }
    //     EVP_CIPHER_CTX_free(ctx);
    }
}
