#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/module/rs.h>
#include "io/odysz/entt_jserv.h"
#include "expect/t_01_echomsg.hpp"
#include "expect/t_02_semantier.hpp"


using namespace anson;
using namespace std;

static AstMap asts;

static JsonOpt contxt{&asts};

TEST(AUTOGEN, AnsonMsg_EchoReq2) {
    register_jserv(asts, contxt);
    load_echoreq2Ast(asts, "ast/echo2.ast.json");

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

TEST(AUTOGEN, SessionReq) {
    register_jserv(asts, contxt);
    load_ansessionreqAst(asts, "ast/session-req.ast.json");

    using Req = AnsonMsg<AnSessionReq>;

    string msgclass = Req().anclass;
    auto mt = entt::resolve(hashed_string{msgclass.c_str()});
    ASSERT_TRUE(mt) << "resolve " << msgclass;

    auto mv = mt.construct(Port(Port::session));
    Req* msg = mv.try_cast<Req>();
    AnSessionReq body{};
    body.a = AnSessionReq::A::login;
    body.uid = "ody";
    msg->Body(body);

    cout << "[1] msg.port: " << msg->port << endl;
    ASSERT_EQ(msgclass, msg->anclass);
    ASSERT_EQ(Req::_type_, msg->type);
    ASSERT_EQ(Port::session, msg->port.url()) << "[1] msg->port";
    ASSERT_EQ("session", msg->port.valof()) << "[1] msg->port";
    ASSERT_EQ("login", msg->body.at(0)->a) << "[1] msg-body[0]";
    ASSERT_EQ("ody", msg->body.at(0)->uid) << "[1] msg-body[0].uid";


    Req msg2{};
    std::string json_input = R"({"type": "input", "port": "session", "body": [{"a": "test/echo", "uid": "Mr. Zelenskyy"}]})";

    anlog("[2] "s + json_input);
    bool result = Anson::from_json(json_input, msg2);
    anlog(std::format("[3] ok: {}, anclass: {}, port: {}", result, msg2.anclass, msg2.port.url()));

    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonMsg<AnSessionReq>().anclass, msg2.anclass) << "msg->anclass";
    ASSERT_EQ("input", msg2.type);

    ASSERT_EQ(Port::session, msg2.port.url()) << "[3] msg->port";
    ASSERT_EQ("session", msg2.port) << "[3] msg->port";

    AnSessionReq reqbd = msg2.Body();

    cout << "[4] body: " << msg2.body.size() << ", type: " << reqbd.anclass << ", a: " << reqbd.a << endl;
    ASSERT_EQ(AnSessionReq::_type_, reqbd.anclass) << "[4] reqbd.anclass";
    ASSERT_EQ("test/echo", reqbd.a) << "[4] body[0].a = test/echo";
    ASSERT_EQ("Mr. Zelenskyy", reqbd.uid) << "[4] body[0].uid";
}

TEST(AUTOGEN, SessionResp) {

    register_jserv(asts, contxt);
    // load_ansessionrespAst(asts, "ast-cpy/session-resp.ast.json");
    register_semantier(asts, "./");

    using Resp = AnsonMsg<AnSessionResp>;
    Resp msg2{};
    std::string json_input = std::format(
        R"({{"type": "{}", "port": "session", "body": [{{"a": "{}", "ssInf": {{"uid": "Mr. Zelenskyy"}}}}]}})",
        Resp::_type_, AnSessionReq::A::pswd);

    anlog(json_input);
    bool result = Anson::from_json(json_input, msg2);
    anlog(std::format("ok: {}, anclass: {}, port: {}", result, msg2.anclass, msg2.port.valof()));

    ASSERT_TRUE(result);
    ASSERT_EQ(Resp().anclass, msg2.anclass) << "msg->anclass";
    ASSERT_EQ(Resp::_type_, msg2.type) << "msg->type";

    ASSERT_EQ(Port::session, msg2.port.url()) << "[] msg->port";
    ASSERT_EQ("session", msg2.port) << "[] msg->port";

    AnSessionResp repbd = msg2.Body();

    anlog(std::format("[] body: {}, type: {}, a: {}", msg2.body.size(), repbd.anclass, repbd.a));

    ASSERT_EQ(AnSessionResp::_type_, repbd.anclass) << "[4] reqbd.anclass";
    ASSERT_EQ(AnSessionReq::A::pswd, repbd.a) << "[4] body[0].a ";
    ASSERT_EQ("Mr. Zelenskyy", repbd.ssInf.uid) << "[4] body[0].uid";

    // Real Example, jsample
    json_input =R"({"type": "io.odysz.semantic.jprotocol.AnsonMsg", )"
                R"("code": "exSession", "opts": null, "port": "session", "header": null, )"
                R"("body": [{"type": "io.odysz.semantic.jprotocol.AnsonResp", "rs": null, "parent": "io.odysz.semantic.jprotocol.AnsonMsg", )"
                R"("a": null, "m": "Since 2.0.0, client uri cannot be empty for session checking, logging in, etc.", )"
                R"("map": null, "uri": null}
], "addr": null, "version": "1.1", "seq": 0})";

    result = Anson::from_json(json_input, msg2);
    repbd = msg2.Body();
    ASSERT_TRUE(result);
    // EXPECT_EQ(1, msg2.body_size()) << "TODO: clean message body before deserilize list";
    if (msg2.body_size() > 1)
        anwarn("TODO\nTODO\nTODO\nTODO\nTODO: clean message body before deserilize list");
    AnSessionResp *bd2 = msg2.body.at(1).get();
    ASSERT_EQ("Since 2.0.0, client uri cannot be empty for session checking, logging in, etc.", bd2->m)
        << "session error msg";
}

// TODO
// TEST(AUTOGEN, DATASET) { }
// TEST(AUTOGEN, STREE) { }
