#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/odysz/jprotocol.h"
#include "io/odysz/entt_jserv.h"


using namespace anson;
using namespace std;

static AstMap asts;

static JsonOpt contxt{&asts};

TEST(AnsonRespons, Deserialize) {
    register_jserv(asts, contxt);
    anlog(to_aststring(asts), PrintFormat{.sep="\n"});

    using Rep = AnsonMsg<AnsonResp>;

    string msgclass = Rep().anclass;
    auto mt = entt::resolve(hashed_string{msgclass.c_str()});
    ASSERT_TRUE(mt) << "resolve " << msgclass;

    Rep resp{};
    std::string json_input = std::format(R"({{"type": "{}", "port": "query", "code": "ok", "body": [{{"m": "Rep 111"}}]}})",
                                         AnsonMsg<AnsonResp>::_type_);

    cout << "[1] " << json_input << endl;
    bool result = Anson::from_json(json_input, resp);
    anlog(std::format("[2] ok: {}, anclass: {}, port: {}", result, resp.anclass, resp.port.valof()));

    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonMsg<AnsonResp>::_type_, resp.type);

    EXPECT_EQ("query", resp.port.valof()) << "[3] resp->port.valof()";
    EXPECT_EQ(Port::query, resp.port.url()) << "[3.1] resp->port";
    EXPECT_EQ("query", resp.port) << "[3.2] resp->port";

    AnsonResp repbd = resp.Body();

    cout << "[4] body: " << resp.body_size() << ", type: " << repbd.anclass << endl;
    EXPECT_EQ("ok", AnsonJavaEnumAst::name<MsgCode>(resp.code)) << "[4] body[0].code = 'ok'";
    EXPECT_EQ(MsgCode::Code::ok, resp.code) << "[4] body[0].code = ok";

    // issue: there should be an event of on value pasing in the parser.
    string issue = R"({"type": "io.odysz.semantic.jprotocol.AnsonMsg", )"
        R"("code": "ok", "opts": null, "port": "echo", "header": null, )"
        R"("body": [{"type": "io.odysz.semantic.jprotocol.AnsonResp", "rs": null, )"
        R"(parent": "io.odysz.semantic.jprotocol.AnsonMsg", "a": null, "m": "TEST Echo...", )"
        R"("map": {"interfaces": ["/127.0.0.1", "/0:0:0:0:0:0:0:1", "/abcd:0:0:0:abcd:abcd:abcd:abcd%wlan0", "/192.168.0.111"]}, )"
        R"("uri": null}], "addr": null, "version": "1.1", "seq": 0})";

    anwarn("ISSUE: map<string, list<string");
    anwarn(issue);
}

TEST(AnsonResponse, Serialize) {
    register_jserv(asts, contxt);
    anlog(to_aststring(asts), PrintFormat{.sep="\n"});

    using Req = AnsonMsg<AnsonResp>;
    auto msg = std::make_shared<Req>(Port::query);
    AnsonResp bd;
    msg->code = MsgCode::Code::ok;
    bd.m = "TEST AnsonRespse Serialize";
    bd.uri = "AnsonResponse.Serialize";
    msg->Body(bd);

    std::ostringstream oss;

    msg->toBlock(oss, contxt);
    std::string json_result = std::move(oss).str();

    std::cout << endl << "Serialized JSON: " << json_result << std::endl;
    ASSERT_EQ((R"({"type": "io.odysz.semantic.jprotocol.AnsonMsg",)"
               R"("body": [{"type": "io.odysz.semantic.jprotocol.AnsonResp",)"
               R"("a": "NA","uri": "AnsonResponse.Serialize","m": "TEST AnsonRespse Serialize",)"
               R"("map": {},"rs": []}],"code": "ok",)"
               R"("header": {"type": "io.odysz.semantic.jprotocol.AnsonHeader","iv64": "","ssToken": "","ssid": "","uid": "","usrAct": []},)"
               R"("port": "query","seq": -842150451,"version": ""})")
              , json_result) << "serialzied josn string";
}
