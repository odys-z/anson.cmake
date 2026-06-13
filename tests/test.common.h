#pragma once

#include <gtest/gtest.h>
#include <string>
#include <io/odysz/anson.h>

using namespace std;
using namespace anson;

struct JservTestCase {
    bool ok;
    std::string url;
    bool https;
    vector<int> portRange;
    vector<string> paths;
    bool ipv6;
    string expect;
};

inline static const std::vector<JservTestCase> urls = {
//  [0] ok? [1] case                                                  [2] https? [3] port-range [4] root-path           [5] ipv6
//  [6] expect jesrv
    {true,  "https://odys-z.github.io:443/notes/index.html?v=1&w=2#rave", true,  {80, 443},    {"notes", "index.html"}, false,
     "https://odys-z.github.io:443/jserv-album"},
    {false, "https://odys-z.github.io/notes/index.html#rave?v=1&w=2",     true,  {1024, -1},   {"notes", "index.html"}, false,
     "https://odys-z.github.io:443/jserv-album"},
    {true,  "//odys-z.github.io/notes/index.html#rave?v=1&w=2",           false, {80, 1024},   {"notes", "index.html"}, false,
     "http://odys-z.github.io:80/jserv-album"},
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

static bool validPaths(const vector<string> &expect, const vector<string> &actual) {
    if (expect.size() != actual.size()) return false;

    for (size_t i = 0; i < expect.size(); ++i) {
        if (expect[i] !=  actual[i])
            return false;
    }
    return true;
}


