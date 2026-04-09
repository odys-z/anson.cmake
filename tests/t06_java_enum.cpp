#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <io/odysz/jprotocol.h>
#include "io/odysz/json.h"
// #include "io/odysz/anserializer.h"
#include "io/odysz/json.h"

#include "echoreq.h"
#include "t06_java_enum.h"

namespace {

using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

AstMap enums;
map<string, meta_type> enttypes;

void register_testport(AstMap &enums) {
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
    JsonOpt contxt{&enums};
    IJsonable::contxt_ptr = &contxt;
    register_asts(enums);
    register_port(enums, "ast/port.ast.json");
    register_msg(enums);
    load_msg_specialAst<UserReq>(enums, "ast/usereq.ast.json",
        [](meta_factory<UserReq> enttype){
            enttype.data<&UserReq::data>("data");
    });
    anlog(to_aststring(enums));

    auto p_type = entt::resolve(hashed_string{Port::_type_.c_str()});
    meta_any ptr = p_type.construct();
    Port& port = ptr.cast<anson::Port&>();
    cout << "Port Value: " << port.enm << endl;
    anlog(string_view(format("[1] Port Value: {}", port.enm)));

    ASSERT_EQ("na", port.enm);

    port = Port{Port::update};
    // ASSERT_EQ(Port::update, serialize_jsonable(port, enums));

    std::string json_input = format(R"({{"type": "{}", "port": "{}"}})",
                                    AnsonMsg<EchoReq>::_type_,
                                    Port::echo);
    p_type = entt::resolve(hashed_string{AnsonMsg<UserReq>().anclass.c_str()});
    ptr = p_type.construct();
    AnsonMsg<UserReq> usreq = ptr.cast<anson::AnsonMsg<UserReq>&>();

    // EnTTSaxParser<AnsonMsg<UserReq>>  handler(usreq, contxt);
    EnTTSaxParser handler(usreq, &contxt);
    bool result = nlohmann::json::sax_parse(json_input, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonMsg<UserReq>::_type_ + "<" + UserReq::_type_, usreq.anclass) << "usreq.anclass";
    ASSERT_EQ(Port::echo, usreq.port)  << "expecting msg port: " << Port::echo;
}

// TEST(JAVAENUM, TEST_PORT) {
// }

}
