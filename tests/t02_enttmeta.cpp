#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"

using json = nlohmann::json;
using namespace anson;

template<typename T>
void load_json(const std::string& raw_json, T& out_obj) {
    EnTTSaxParser handler(out_obj);

    // Wrap our existing C++ instance so the parser can fill it
    // handler.set_top(entt::forward_as_meta(out_obj));
    handler.set_top(out_obj);

    // Execute SAX parse
    if (nlohmann::json::sax_parse(raw_json, &handler)) {
        // Success: out_obj is now populated
    }
}

TEST(HELLO, ENTT_META) {
    map<string, AnsonAst> enums;
    map<string, meta_type> types;
    register_meta(enums, types);
    JsonOpt jsonopts{&enums, &types};

    AnsonMsg<EchoReq> msg{Port::echo};

    cout << "Port: " << msg.port << endl;
    EchoReq echobd{"echo msg ..."};
    msg.Body(echobd);

    cout << "Echo: " << msg.Body().echo << endl;

    msg.toBlock(cout, jsonopts);

    msg.toBlock(cout, jsonopts);

    EXPECT_EQ(R"({"type": "io.odysz.jprotocol.AnsonMsg", )"
              R"("port": "echo", "body": [{"a": "r/query", "echo": "echo msg ..."}]})",
              msg.toBlock(jsonopts))
        << "static _type_ must be ignored, port name must used as the enum value...";

    // 1. Create EchoReq via reflection
    auto echo_type = entt::resolve("EchoReq"_hs);
    auto req_instance = echo_type.construct();
    std::cout << "Actual Type Name: " << req_instance.type().info().name() << std::endl;
    EchoReq* echoreq = req_instance.try_cast<EchoReq>();
    cout << "EchoReq Reflected: " << echoreq->a << endl;

    // Set the 'echo' field
    if (auto data = echo_type.data("echo"_hs)) {
        data.set(req_instance, std::string("Reflection Hello"));
    }

    // 2. Create AnsonMsg<EchoReq> via reflection
    auto msg_rfl = entt::resolve("AnsonMsgEchoReq"_hs).construct(Port(Port::echo));

    // Use this to check what EnTT actually thinks the type is:
    std::cout << "Actual Type Name: " << msg_rfl.type().info().name() << std::endl;

    // Try to get the reference first, then take the address
    if (auto* msg_rpt = msg_rfl.try_cast<AnsonMsg<EchoReq>>()) {
        string t = msg_rpt->anclass;
        ASSERT_EQ(AnsonMsg<EchoReq>::_type_, t);
    } else {
        // If that fails, msg_rfl might be holding a pointer.
        // Try casting to the pointer type directly:
        auto** ptr_to_ptr = msg_rfl.try_cast<AnsonMsg<EchoReq>*>();
        if (ptr_to_ptr) {
            AnsonMsg<EchoReq>* msg_rpt_actual = *ptr_to_ptr;
            ASSERT_EQ(AnsonMsg<EchoReq>::_type_, msg_rpt_actual->anclass);
        } else {
            FAIL() << "Could not cast meta_any to AnsonMsg<EchoReq>";
        }
    }

    // string t = msg_rpt->type;
    // ASSERT_EQ(AnsonMsg<EchoReq>::_type_, msg_rpt->type);
}
