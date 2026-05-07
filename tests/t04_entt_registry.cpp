#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"
#include "echoreq.expect.h"

using json = nlohmann::json;
using namespace anson;

TEST(ENTT_META, JSON_REGISTRY) {
    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    load_echoAst_test(asts, "ast/echo.ast.json");

    JsonOpt jsonopts{&asts};
    IJsonable::contxt_ptr = &jsonopts;

    AnsonMsg<EchoReq> msg{Port::echo};

    cout << "Port: " << msg.port << endl;
    EchoReq echobd{"echo msg ..."};
    msg.Body(echobd);

    cout << "Echo: " << msg.Body().echo << endl;

    msg.toBlock(cout, jsonopts);

    msg.toBlock(cout, jsonopts);

    entt::hashed_string echo_type_hs{EchoReq::_type_.c_str()};
    auto echo_type = entt::resolve(echo_type_hs);

    auto req_instance = echo_type.construct();
    std::cout << "Actual Type Name: " << req_instance.type().info().name() << std::endl;
    EchoReq* echoreq = req_instance.try_cast<EchoReq>();
    cout << "EchoReq Reflected: " << echoreq->a << endl;

    if (auto data = echo_type.data("echo"_hs)) {
        data.set(req_instance, std::string("Reflection Hello"));
    }

    // string msg_anclass = EchoReq()._type_special(AnsonMsg<EchoReq>::_type_);
    string msg_anclass = AnsonMsg<EchoReq>().anclass;
    auto msg_rfl = entt::resolve(hashed_string{msg_anclass.c_str()}).construct(Port(Port::echo));

    std::cout << "Actual Type Name: " << msg_rfl.type().info().name() << std::endl;

    if (auto* msg_rpt = msg_rfl.try_cast<AnsonMsg<EchoReq>>()) {
        string t = msg_rpt->anclass;
        ASSERT_EQ(msg_anclass, t);
    } else {
        auto** ptr_to_ptr = msg_rfl.try_cast<AnsonMsg<EchoReq>*>();
        if (ptr_to_ptr) {
            AnsonMsg<EchoReq>* msg_rpt_actual = *ptr_to_ptr;
            ASSERT_EQ(AnsonMsg<EchoReq>::_type_, msg_rpt_actual->anclass);
        } else {
            FAIL() << "Could not cast meta_any to AnsonMsg<EchoReq>";
        }
    }
}
