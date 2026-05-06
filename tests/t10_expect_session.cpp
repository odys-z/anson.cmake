#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/module/rs.h>
#include "io/odysz/entt_jserv.h"
#include "expect/t_02_jserv.hpp"


using namespace anson;
using namespace std;

static AstMap asts;

static JsonOpt contxt{&asts};

TEST(AUTOGEN, SessionReq) {
    register_jserv(asts, contxt);
    load_sessionreqAst(asts, "ast-cpy/sessionreq.ast.json");
    load_sessionrespAst(asts, "ast-cpy/sessionresp.ast.json");

    using Req = AnsonMsg<SessionReq>;

    string msgclass = Req().anclass;
    auto mt = entt::resolve(hashed_string{msgclass.c_str()});
    ASSERT_TRUE(mt) << "resolve " << msgclass;

    auto mv = mt.construct(Port(Port::echo));
    Req* msg = mv.try_cast<Req>();
    SessionReq body{};
    body.a = SessionReq.A.login;
    msg->Body(body);

    cout << "[1] msg.port: " << msg->port << endl;
    ASSERT_EQ(msgclass, msg->anclass);
    ASSERT_EQ(Req::_type_, msg->type);
    ASSERT_EQ(Port::echo, msg->port.url()) << "[1] msg->port";
    ASSERT_EQ("session", msg->port) << "[1] msg->port";
    ASSERT_EQ("login", msg->body.at(0)->a) << "[1] msg-body[0]";
    ASSERT_EQ("ody", msg->body.at(0)->uid) << "[1] msg-body[0].uid";


    using Resp = AnsonMsg<SessionResp>;
    Resp resp{};
    std::string json_input = R"({"type": "io.odysz.jprotocol.AnsonMsg", "port": "session",)"
                             R"("body": [{"a": "login", "ssInf": {"ssid": "abc", "token": ""}}]})";

    cout << "[2] " << json_input << endl;
    bool result = Anson::from_json(json_input, resp);
    cout << "[3] ok: " << result << ", anclass: " << resp.anclass << ", port: " << resp.port << endl;

    ASSERT_TRUE(result);
    ASSERT_EQ(Resp().anclass, resp.anclass) << "msg->anclass";
    ASSERT_EQ(AnsonMsg<SessionResp>::_type_, resp.type);

    EXPECT_EQ(Port::session, resp.port.url()) << "[3] msg->port";
    EXPECT_EQ("query", resp.port) << "[3] msg->port";

    SessionResp repbd = resp.Body();

    cout << "[4] body: " << resp.body.size() << ", type: " << repbd.anclass << ", a: " << repbd.a << endl;
    EXPECT_EQ(SessionResp::_type_, repbd.anclass) << "[4] repbd.anclass";
    EXPECT_EQ("ody", repbd.body[0]->ssInf.uid) << "[4] msg->body.uid";
    EXPECT_EQ("login", repbd.a) << "[4] body[0].a";
}

