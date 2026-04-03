#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"
#include "echoreq.h"

namespace anson {

using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

AstMap enums;
map<string, meta_type> types;

JsonOpt contxt{&enums};

/*
TEST(Anson, Base) {
    register_asts(enums);
    IJsonable::contxt_ptr = &contxt;

    auto an_type = entt::resolve(hashed_string{Anson::_type_.c_str()});
    meta_any anptr = an_type.construct();
    Anson &anobj = anptr.cast<anson::Anson&>();
    cout << "Actual Type Name: " << anobj.anclass << endl;
    ASSERT_EQ(Anson::_type_, anobj.anclass);

    anptr = an_type.construct(Anson::_type_);
    Anson& anobj2 = anptr.cast<anson::Anson&>();
    ASSERT_EQ(Anson::_type_, anobj2.anclass) << "1. anobj2.type";

    std::string json_input = R"({"type": "input"})";
    EnTTSaxParser handler(anobj2);

    cout << "[2] " << json_input << endl;
    bool result = nlohmann::json::sax_parse(json_input, &handler);

    ASSERT_TRUE(result);
    ASSERT_EQ("input", anobj2.type) << "anobj2.type: {type: input}.";
}

TEST(Anson, AnsonBody) {
    register_msg(enums);
    IJsonable::contxt_ptr = &contxt;

    AnsonBody testbody{"test"}; // instantatiate abstract class?

    auto b = entt::resolve(hashed_string{AnsonBody::_type_.c_str()});
    auto v = b.construct(std::string("r/ds"));
    AnsonBody* anb = v.try_cast<AnsonBody>();
    cout << "[1] anb.type: " << anb->anclass << endl;
    ASSERT_EQ(AnsonBody::_type_, anb->anclass);
    ASSERT_EQ("r/ds", anb->a) << "[1.0]";

    std::string json_input = R"({"type": "input", "a": "r/query"})";
    EnTTSaxParser handler(*anb, &contxt);

    cout << "[2] " << json_input << endl;
    bool result = nlohmann::json::sax_parse(json_input, &handler);
    cout << "[3] ok: " << result << ", type: " << anb->anclass << ", a: " << anb->a << endl;
    ASSERT_TRUE(result);
    ASSERT_EQ("input", anb->type) << "[3.1]";
    ASSERT_EQ("r/query", anb->a) << "[3.2]";

    AnsonBody anc;
    EnTTSaxParser handler2(anc, &contxt);
    result = nlohmann::json::sax_parse(json_input, &handler2);
    ASSERT_TRUE(result);
    ASSERT_EQ("input", anc.type) << "[3.4]";

    json_input = R"({"type": "input-2", "a": "d/del"})";
    result = Anson::from_json<AnsonBody>(json_input, anc, &contxt);
    ASSERT_TRUE(result);
    ASSERT_EQ("input-2", anc.type) << "[3.5]";
    ASSERT_EQ("d/del", anc.a) << "[3.6]";
}

TEST(Anson, PORT) {
    register_port(enums, "ast/port.ast.json");
    string portclass = Port::_type_;
    AnsonJavaEnumAst* portAst = dynamic_cast<AnsonJavaEnumAst*>(enums.at(portclass).get());
    hashed_string portype{portclass.c_str()};

    ASSERT_TRUE(portAst->encode.size() > 2);
    ASSERT_TRUE(portAst->encode.contains(Port::echo));
    ASSERT_TRUE(portAst->decode.size() > 2);
    ASSERT_TRUE(portAst->decode.contains("echo"));
    ASSERT_EQ(portAst->enttypeid, portype);
}
*/

/**
 * @brief specialize_req
 *
 * Register AnsonMsg template (example for EchoReq)
 *
 * @param asts
 * @param body_ast
template<typename T>
void specialize_req(AstMap &asts, const AnsonBodyAst *body_ast) {
    AnsonMsg<T> msg_echoreq;
    string anclass = msg_echoreq.anclass;
    hashed_string enttype{anclass.c_str()};

    entt::meta_factory<anson::AnsonMsg<T>>()
        .type(enttype)
        .template ctor<>()
        .template ctor<anson::Port>()
        .template base<anson::Anson>()
        .template data<&anson::AnsonMsg<T>::port>("port")
        .template data<&anson::AnsonMsg<T>::body>("body");

    AnsonMsgAst *ast = new AnsonMsgAst(anclass);
    ast->dataBaseAst = AnsonAst::_type_;
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;

    ast->fields = map<string, AnsonField>{
        {"port", {.fieldname = "port", .dataAnclass=Port::_type_}},
        {"body", {.fieldname = "body", .dataAnclass="list<"s + T::_type_}}
    };

    // asts[anclass] = unique_ptr<AnsonMsgAst>(ast);
    asts.insert(make_pair(anclass, ast));
}

void load_echoAst(AstMap &asts, string ast_path) {
    hashed_string enttype{EchoReq::_type_.c_str()};
    entt::meta_factory<anson::EchoReq>()
        .type(enttype)
        .base<AnsonBody>()
        .ctor<>()
        .ctor<string>()
        .data<&anson::EchoReq::echo>("echo"_hs, "echo")
        ;

    AnsonBodyAst *echoAst = new AnsonBodyAst{};
    echoAst->dataAnclass = EchoReq::_type_;
    EnTTSaxParser handler(*echoAst, IJsonable::contxt_ptr);

    std::ifstream ifstream(ast_path);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", ast_path)));
    }

    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        string anclass = echoAst->dataAnclass;
        hashed_string enttype = hashed_string{anclass.c_str()};

        // meta_type portype =
        entt::meta_factory<anson::EchoReq>()
            .type(enttype)
            .base<AnsonBody>()
            .ctor<>()
            .ctor<string>()
            // .data<&EchoReq::a>("a")
            .data<&EchoReq::echo>("echo")
            ;

        // for field in portAst.fields
        //    type.data<field.name().for_class>();

        echoAst->enttypeid = enttype;

        asts[anclass] = unique_ptr<AnsonBodyAst>(echoAst);

        specialize_req<EchoReq>(asts, echoAst);
    }
    else
        anerror(string_view(std::format("Could not load AST from {}!", ast_path)));
}
 */

TEST(Anson, AnsonMsg_EchoReq) {
    IJsonable::contxt_ptr = &contxt;
    register_asts(enums);
    register_msg(enums);
    register_port(enums, "ast/port.ast.json");
    load_echoAst(enums, "ast/echo.ast.json");

    using Req = AnsonMsg<EchoReq>;

    string msgclass = Req().anclass;
    auto mt = entt::resolve(hashed_string{msgclass.c_str()});
    ASSERT_TRUE(mt) << "resolve " << msgclass;

    auto mv = mt.construct(Port(Port::echo));
    Req* msg = mv.try_cast<Req>();
    EchoReq body{"Hello"};
    body.a = "";
    // body.echo = "";
    msg->Body(body);

    cout << "[1] msg.port: " << msg->port << endl;
    ASSERT_EQ(msgclass, msg->anclass);
    ASSERT_EQ(Req::_type_, msg->type);
    ASSERT_EQ(Port::echo, msg->port) << "[1] msg->port";
    ASSERT_EQ("", msg->body.at(0)->a) << "[1] msg-body[0]";
    ASSERT_EQ("Hello", msg->body.at(0)->echo) << "[1] msg-body[0].echo";

    std::string json_input = R"({"type": "input", "port": "query", "body": [{"a": "test/echo"}]})";

    cout << "[2] " << json_input << endl;
    bool result = Anson::from_json(json_input, *msg);
    cout << "[3] ok: " << result << ", type: " << msg->anclass << ", port: " << msg->port << endl;

    ASSERT_TRUE(result);
    ASSERT_EQ(EchoReq()._type_special(AnsonMsg<EchoReq>::_type_), msg->anclass) << "msg->anclass";
    ASSERT_EQ("input", msg->type);
    EXPECT_EQ(Port::query, msg->port) << "[3] msg->port";

    EchoReq& reqbd = msg->Body();

    cout << "[4] body: " << msg->body.size() << ", type: " << reqbd.anclass << ", a: " << reqbd.a << endl;
    EXPECT_EQ(EchoReq::_type_, reqbd.anclass) << "[4] reqbd.anclass";
    EXPECT_EQ("test/echo", reqbd.a) << "[4] a = test/echo";
}

/*
TEST(Anson, Servialize_Msg) {

    using Req = AnsonMsg<EchoReq>;
    auto msg = std::make_shared<Req>(Port::query);
    msg->body.push_back(std::make_shared<EchoReq>("Hello World"));

    std::ostringstream oss;

    // serialize_recursive(msg, enums, oss);
    JsonOpt opts{&enums, &types};
    msg->toBlock(oss, opts);

    // Why move()? Gemini: instead of copying, stringstream is allowed to transfer
    // ownership of its internal memory directly to the returned std::string.
    std::string json_result = std::move(oss).str();

    std::cout << "Serialized JSON: " << json_result << std::endl;
    ASSERT_EQ(R"({"type": "io.odysz.jprotocol.AnsonMsg", "port": "query", )"
              R"("body": [{"a": "r/query", "echo": "Hello World"}]})",
              json_result);
}
*/
}
