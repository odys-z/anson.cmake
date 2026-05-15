#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <io/odysz/jprotocol.h>
#include "io/odysz/entt_jserv.h"

#include "echoreq.expect.h"

namespace {

using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

AstMap enums;
map<string, meta_type> enttypes;

TEST(JAVAENUM, PORT) {
    JsonOpt contxt{&enums};
    register_jserv(enums, contxt);
    load_usereqAst_test(enums, "ast/usereq.ast.json");

    anlog(to_aststring(enums));

    auto p_type = entt::resolve(hashed_string{Port::_type_.c_str()});
    meta_any ptr = p_type.construct();
    Port& port = ptr.cast<anson::Port&>();
    cout << "Port Value: " << port.enm << endl;
    anlog(string_view(format("[1] Port Value: {}", port.enm)));

    ASSERT_EQ("na", port.enm);

    port = Port{Port::update};

    std::string json_input = format(R"({{"type": "{}", "port": "{}"}})",
                                    AnsonMsg<EchoReq>::_type_,
                                    Port::echo);
    p_type = entt::resolve(hashed_string{AnsonMsg<UserReq>().anclass.c_str()});
    ptr = p_type.construct();
    AnsonMsg<UserReq> usreq = ptr.cast<anson::AnsonMsg<UserReq>&>();

    EnTTSaxParser handler(usreq, &contxt);
    bool result = nlohmann::json::sax_parse(json_input, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonMsg<UserReq>().anclass, usreq.anclass) << "usreq.anclass";
    ASSERT_EQ("echo", usreq.port)  << "usreq.echo [1]";
    ASSERT_EQ(Port{Port::echo}, usreq.port)  << "usreq.echo [2]";
}
}
