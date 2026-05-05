#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/module/rs.h>
#include "io/odysz/entt_jserv.h"
#include "expect/t_01_semantier.hpp"


using namespace anson;
using namespace std;

static AstMap asts;

static JsonOpt contxt{&asts};

TEST(AUTOGEN, AnsonMsg_EchoReq2) {
    register_jserv(asts, contxt);
    load_echoreq2Ast(asts, "ast-cpy/echo2.ast.json");

    using Req = AnsonMsg<EchoReq2>;

    string msgclass = Req().anclass;
    auto mt = entt::resolve(hashed_string{msgclass.c_str()});
    ASSERT_TRUE(mt) << "resolve " << msgclass;

    auto mv = mt.construct(Port(Port::echo));
    Req* msg = mv.try_cast<Req>();
    EchoReq2 body{"Hello"};
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
    ASSERT_EQ(AnsonMsg<EchoReq2>().anclass, msg2.anclass) << "msg->anclass";
    ASSERT_EQ("input", msg2.type);

    EXPECT_EQ(Port::query, msg2.port.url()) << "[3] msg->port";
    EXPECT_EQ("query", msg2.port) << "[3] msg->port";

    EchoReq2 reqbd = msg2.Body();

    cout << "[4] body: " << msg2.body.size() << ", type: " << reqbd.anclass << ", a: " << reqbd.a << endl;
    EXPECT_EQ(EchoReq2::_type_, reqbd.anclass) << "[4] reqbd.anclass";
    EXPECT_EQ("AnsonMsg_EchoReq!", msg2.body[0]->echo) << "[4] msg->body.echo";
    EXPECT_EQ("test/echo", reqbd.a) << "[4] body[0].a = test/echo";
}

// TEST(AUTOGEN, DocTier) { }
