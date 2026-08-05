#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/odysz/entt_jserv.h"
#include "echoreq.expect.h"

namespace anson {

using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

static AstMap enums;
static map<string, meta_type> types;

static JsonOpt contxt{&enums};

TEST(SAVE, Settings) {
    aninfo(filesystem::current_path().string());

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_peersettings(asts);

    std::string timestamp = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now());
    PeerSettings settings;
    settings.anRequests = {"request-00"};
    settings.ansonBody = timestamp;
    settings.cpp_gen = "testing-target.c++";

    settings.to_file("t14-settings.json");

    PeerSettings sets2;
    Anson::from_file("t14-settings.json", sets2, &contxt);
    ASSERT_EQ(vector<string>{"request-00"}, sets2.anRequests);
    ASSERT_EQ(timestamp, sets2.ansonBody);
    ASSERT_EQ("testing-target.c++", sets2.cpp_gen);
}

}
