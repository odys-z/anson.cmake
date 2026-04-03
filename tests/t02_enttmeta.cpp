#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"
#include "echoreq.h"

using json = nlohmann::json;
using namespace anson;

/*
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

void register_echo(AstMap &asts) {
    //
    string anclass{EchoReq().anclass};
    hashed_string enttype{anclass.c_str()};
    entt::meta_factory<anson::EchoReq>()
        .type(enttype)
        .base<AnsonBody>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::EchoReq::echo>("echo")
        ;

    AnsonBodyAst *ast = new AnsonBodyAst{anclass};
    ast->enttypeid = enttype;
    ast->base = AnsonBody::_type_;
    ast->dataAnclass = anclass;

    ast->fields = map<string, AnsonField>{
        {"echo", {.fieldname="echo", .dataAnclass="string"}},
    };

    asts[anclass] = unique_ptr<AnsonBodyAst>(ast);
}
*/

TEST(ENTT_META, JSON_REGISTRY) {
    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    // register_echo(asts);
    load_echoAst(asts, "ast/echo.ast.json");

    JsonOpt jsonopts{&asts};
    IJsonable::contxt_ptr = &jsonopts;

    AnsonMsg<EchoReq> msg{Port::echo};

    cout << "Port: " << msg.port << endl;
    EchoReq echobd{"echo msg ..."};
    msg.Body(echobd);

    cout << "Echo: " << msg.Body().echo << endl;

    msg.toBlock(cout, jsonopts);

    msg.toBlock(cout, jsonopts);

    /** to be fixed
    EXPECT_EQ(std::format(R"({{"type": "{}", )", AnsonMsg<EchoReq>::_type_) +
              R"("port": "echo", "body": [{"a": "r/query", "echo": "echo msg ..."}]})",
              msg.toBlock(jsonopts))
        << "static _type_ must be ignored, port name must used as the enum value...";
    */

    entt::hashed_string echo_type_hs{EchoReq::_type_.c_str()};
    auto echo_type = entt::resolve(echo_type_hs);

    auto req_instance = echo_type.construct();
    std::cout << "Actual Type Name: " << req_instance.type().info().name() << std::endl;
    EchoReq* echoreq = req_instance.try_cast<EchoReq>();
    cout << "EchoReq Reflected: " << echoreq->a << endl;

    if (auto data = echo_type.data("echo"_hs)) {
        data.set(req_instance, std::string("Reflection Hello"));
    }

    string msg_anclass = EchoReq()._type_special(AnsonMsg<EchoReq>::_type_);
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
