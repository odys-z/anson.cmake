#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <algorithm>

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
        {false, "//odys-z.github.io%20",                                      false, {80, 1024},   {}, false},
        {false, "//odys-z.github.io%20/notes%20",                             false, {80, 1024},   {"notes%20"}, false},
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

    // Main Test Loop
    for (const JservTestCase& test : urls) {
        HttpParts parts;
        getHttpParts(test.url, parts);

        bool isValid = Regex::isHttps(test.url) == test.https &&
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
