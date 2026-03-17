#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <io/odysz/jprotocol.h>
#include <io/odysz/json.h>
#include <io/odysz/anserializer.h>

#include "t04_java_enum.h"

namespace {

using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

map<string, AnsonAst> enums;

void register_testport(map<string, AnsonAst> &enums) {
    // entt::meta_factory<anson::JavaEnum>()
    //     .type("JavaEnumTestPort"_hs, "JavaEnumTestPort")
    //     .base<IJsonable>()
    //     .data<&anson::JavaEnum::enm>("enm"_hs, "enm")
    //     ;

    entt::meta_factory<anson::TestPort>()
        .type("TestPort"_hs, "TestPort")
        .base<anson::JavaEnum>()
        .ctor<std::string>()
        ;

    // {
    //     TestPort::decode["echo"]     = "echo.test";
    //     TestPort::decode["t_query"]  = "r.test";
    //     TestPort::decode["t_update"] = "u.test";

    //     TestPort::encode["echo.test"]= "echo";
    //     TestPort::encode["r.test"]   = "t_query";
    //     TestPort::encode["u.test"]   = "t_update";
    // }
}

TEST(JAVAENUM, PORT) {
    register_meta(enums);

    register_testport(enums);

    auto p_type = entt::resolve("Port"_hs);
    meta_any ptr = p_type.construct();
    Port& port = ptr.cast<anson::Port&>();
    cout << "Port Value: " << port.enm << endl;
    anlog(format("[1] Port Value: {}", port.enm));

    ASSERT_EQ("na", port.enm);

    port = Port{Port::update};
    ASSERT_EQ(Port::update, serialize_json(port, enums));

    std::string json_input = format(R"({{"type": "{}", "port": "{}"}})",
                                    AnsonMsg<EchoReq>::_type_,
                                    Port::echo);
    p_type = entt::resolve("AnsonMsgUserReq"_hs);
    ptr = p_type.construct();
    AnsonMsg<UserReq> usreq = ptr.cast<anson::AnsonMsg<UserReq>&>();

    EnTTSaxParser<AnsonMsg<UserReq>>  handler(usreq);
    bool result = nlohmann::json::sax_parse(json_input, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonMsg<UserReq>::_type_, usreq.anclass) << "expecting msg {type: " << AnsonMsg<UserReq>::_type_;
    ASSERT_EQ(Port::echo, usreq.port)  << "expecting msg port: " << Port::echo;
}

// TEST(JAVAENUM, TEST_PORT) {
// }

}
