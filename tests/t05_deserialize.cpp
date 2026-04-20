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
    register_msgs(enums);
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
    ASSERT_TRUE(portAst->decode.contains(Port::echo));
    ASSERT_TRUE(Port(Port::echo) == "echo");
    ASSERT_TRUE(portAst->decode.size() > 2);
    ASSERT_TRUE(portAst->encode.contains("echo"));
    ASSERT_EQ(portAst->enttypeid, portype);
}

TEST(Anson, AnsonMsg_EchoReq) {
    register_jserv(enums, contxt);
    load_echoAst(enums, "ast/echo.ast.json");

    using Req = AnsonMsg<EchoReq>;

    string msgclass = Req().anclass;
    auto mt = entt::resolve(hashed_string{msgclass.c_str()});
    ASSERT_TRUE(mt) << "resolve " << msgclass;

    auto mv = mt.construct(Port(Port::echo));
    Req* msg = mv.try_cast<Req>();
    EchoReq body{"Hello"};
    body.a = "";
    msg->Body(body);

    cout << "[1] msg.port: " << msg->port << endl;
    ASSERT_EQ(msgclass, msg->anclass);
    ASSERT_EQ(Req::_type_, msg->type);
    ASSERT_EQ(Port::echo, msg->port.url()) << "[1] msg->port";
    ASSERT_EQ("echo", msg->port) << "[1] msg->port";
    ASSERT_EQ("", msg->body.at(0)->a) << "[1] msg-body[0]";
    ASSERT_EQ("Hello", msg->body.at(0)->echo) << "[1] msg-body[0].echo";


    Req msg2{};
    std::string json_input = R"({"type": "input", "port": "query", "body": [{"a": "test/echo", "echo": "AnsonMsg_EchoReq!"}]})";

    cout << "[2] " << json_input << endl;
    bool result = Anson::from_json(json_input, msg2);
    cout << "[3] ok: " << result << ", anclass: " << msg2.anclass << ", port: " << msg2.port << endl;

    ASSERT_TRUE(result);
    ASSERT_EQ(EchoReq()._type_special(AnsonMsg<EchoReq>::_type_), msg2.anclass) << "msg->anclass";
    ASSERT_EQ("input", msg2.type);

    EXPECT_EQ(Port::query, msg2.port.url()) << "[3] msg->port";
    EXPECT_EQ("query", msg2.port) << "[3] msg->port";

    EchoReq reqbd = msg2.Body();

    cout << "[4] body: " << msg2.body.size() << ", type: " << reqbd.anclass << ", a: " << reqbd.a << endl;
    EXPECT_EQ(EchoReq::_type_, reqbd.anclass) << "[4] reqbd.anclass";
    EXPECT_EQ("AnsonMsg_EchoReq!", msg2.body[0]->echo) << "[4] msg->body.echo";
    EXPECT_EQ("test/echo", reqbd.a) << "[4] body[0].a = test/echo";
}

TEST(Anson, Servialize_Msg) {
    JsonOpt opts{&enums};
    register_jserv(enums, opts);
    load_echoAst(enums, "ast/echo.ast.json");
    anlog(to_aststring(enums), PrintFormat{.sep="\n"});

    using Req = AnsonMsg<EchoReq>;
    auto msg = std::make_shared<Req>(Port::query);
    msg->body.push_back(std::make_shared<EchoReq>("Hello World"));

    std::ostringstream oss;

    msg->toBlock(oss, opts);

    std::string json_result = std::move(oss).str();

    std::cout << "Serialized JSON: " << json_result << std::endl;
    ASSERT_EQ(R"({"type": "io.odysz.semantic.jprotocol.AnsonMsg",)"
              R"("body": [{"type": "io.odysz.semantic.jserv.echo.EchoReq","a": "r/query","uri": "","echo": "Hello World"}],)"
              R"("code": "null","port": "query"})"
              , json_result);
}
}
