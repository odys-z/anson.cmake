#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <io/odysz/jprotocol.h>
#include <io/odysz/reflect.h>
#include <nlohmann/json.hpp>

using namespace anson;
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

TEST(LANGEXT, ISENVELOPE) {
    ASSERT_FALSE(LangExt::isenvelope("")) << "false: ''";
    ASSERT_FALSE(LangExt::isenvelope("session openned")) << "false: 'session ....'";
    ASSERT_FALSE(LangExt::isenvelope("{\"x\": 1}")) << "false: ''";
    ASSERT_TRUE(LangExt::isenvelope("{\"type\": \"io.odysz.anson.Anson\"}")) << "false: type: io.odysz.anson.Anson";
    ASSERT_TRUE(LangExt::isenvelope("{\"type\": ")) << "false: type: ''";
}

/**
 * See anson.java/test/io.odysz.anson.AnsonTest.testEscape()
 */
TEST(ANOSTREAM, Escape) {
    JsonOpt opts{nullptr};
    string value = "1\t 2\n 3\" 4\\";
    // 1\\t 2\\n 3\" 4\\
    // Utils.logi(new String(Anson.escape(value)));
    ASSERT_EQ("1\\t 2\\n 3\\\" 4\\\\", Anson::escape(value, opts));

    // nlohmann:
    //           Original Character       JSON Escaped Result
    // Quotation mark  (")                 \"
    // Backslash       (\)                 \\
    // Newline         (\n)                \n
    // Carriage return (\r)                \r
    // Tab             (\t)                \t
    anlog(nlohmann::detail::escape(value));
    ASSERT_EQ("1\t 2\n 3\" 4\\", nlohmann::detail::escape(value));

    value = "1\\t 2\\n3";
    // 1	.2
    // 3
    // TODO ASSERT_EQ("1\t 2\n3", Anson::unescape(value, opts));

    string raw_json = R"({"message": "1\t 2\n 3\" 4\""})";
    nlohmann::json j = nlohmann::json::parse(raw_json);
    ASSERT_EQ(j["message"], R"(1	 2
 3" 4")");

    // value = "1\\t 2\\n 3\\\" 4\\\\";
    // // 1	 2
    // //  3" 4\ //
    // ASSERT_EQ("1\t 2\n 3\" 4\\", Anson::unescape(value));
}
