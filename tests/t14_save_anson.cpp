#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include "io/odysz/entt_jserv.h"
#include "expect/t14_app_settings.hpp"

namespace anson {

using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

static AstMap enums;
static map<string, meta_type> types;

static JsonOpt contxt{&enums};

TEST(SAVE, PeerSettings) {
    aninfo(filesystem::current_path().string());

    AstMap asts;
    JsonOpt contxt{&asts};
    // IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_peersettings(&contxt);

    std::string timestamp = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now());
    PeerSettings settings;
    settings.anRequests = {"request-00"};
    settings.ansonBody = timestamp;
    settings.cpp_gen = "testing-target.c++";

    settings.to_file("t14-settings.json", &contxt);

    PeerSettings sets2;
    Anson::from_file("t14-settings.json", sets2, &contxt);
    ASSERT_EQ(vector<string>{"request-00"}, sets2.anRequests);
    ASSERT_EQ(timestamp, sets2.ansonBody);
    ASSERT_EQ("testing-target.c++", sets2.cpp_gen);
}


/**
 * FIXME
 * This test cannot cover the error case.
 *
 * If the serializer's context switched, the auto-generated code is not
 * switching the context.
 *
 * Solution:
 * The right signature of the callback:
 *
 * get_field_instance = [ast](const IJsonable& ans, const string& fieldname, const JsonOpt* contxt) -> meta_any
 */
TEST(SAVE, DesktopSettings) {
    AstMap asts;
    JsonOpt contxt{&asts};
    // IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_anclient_cmake(&contxt, "ast");
    register_desktopsettingsAst(&contxt);

    DesktopSettings ds;
    Anson::from_file("settings/app-settings-t14.json", ds, &contxt);

    ASSERT_EQ("my", ds.market);
    ds.to_file("output-t14.json", &contxt);

    DesktopSettings ds2;
    Anson::from_file("output-t14.json", ds2, &contxt);

    string sysuri;
    string synuri;
    string jserv;
    string org;
    string domain;
    string device;
    string admin;
    string domain_token;
    string regiserv;
    string centralPswd;
    string temp_dir;

    ASSERT_FALSE(ds2.sysuri.empty());
    ASSERT_FALSE(ds2.synuri.empty());
    ASSERT_FALSE(ds2.jserv.empty());
    ASSERT_FALSE(ds2.org.empty());
    ASSERT_FALSE(ds2.domain.empty());
    ASSERT_FALSE(ds2.device.empty());
    ASSERT_FALSE(ds2.admin.empty());
    ASSERT_FALSE(ds2.domain_token.empty());
    ASSERT_FALSE(ds2.regiserv.empty());
    ASSERT_FALSE(ds2.centralPswd.empty());
    ASSERT_FALSE(ds2.temp_dir.empty());

    // anerror("FIXME\nFIXME\nFIXME\nFIXME\nFIXME\nFIXME\nFIXME\nFIXME");
    // FAIL() << "fix get_field_instance = [ast](const IJsonable& ans, const string& fieldname, const JsonOpt* contxt) -> meta_any";
}
}
