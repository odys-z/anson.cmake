#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <io/odysz/jprotocol.h>
#include "expect/t_10_doctier.hpp"
#include "io/odysz/entt_jserv.h"

#include "echoreq.expect.h"
#include "expect/wsport_tobe_generated.hpp"
#include "expect/t_10_doctier.hpp"


using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

AstMap asts;
map<string, meta_type> enttypes;

TEST(T06_JAVAENUM, PORT) {
    JsonOpt contxt{&asts};
    register_jserv(&contxt);
    load_usereqAst_test(&contxt, "ast/usereq.ast.json");

    andebug(to_aststring(asts));

    auto p_type = entt::resolve(hashed_string{Port::_type_.c_str()});
    meta_any ptr = p_type.construct(static_cast<const JsonOpt*>(&contxt));
    Port& port = ptr.cast<anson::Port&>();

    anlog(std::format("[1] Port Value: {}", port.enm));

    ASSERT_EQ("na", port.enm);

    port = Port{&contxt, Port::update};

    string echo_str = Port(&contxt, Port::echo).valof();
    std::string json_input = format(R"({{"type": "{}", "port": "{}"}})",
                                    AnsonMsg<EchoReq>::_type_,
                                    echo_str);
    p_type = entt::resolve(hashed_string{AnsonMsg<UserReq>(&contxt).anclass.c_str()});
    ptr = p_type.construct(static_cast<const JsonOpt*>(&contxt));
    AnsonMsg<UserReq> usreq = ptr.cast<anson::AnsonMsg<UserReq>&>();

    EnTTSaxParser handler(usreq, &contxt);
    bool result = nlohmann::json::sax_parse(json_input, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonMsg<UserReq>(&contxt).anclass, usreq.anclass) << "usreq.anclass";
    ASSERT_EQ("echo", usreq.port)  << "usreq.echo [1]";
    ASSERT_EQ((Port{&contxt, Port::echo}), usreq.port)  << "usreq.echo [2]";

    usreq.code = MsgCode::Code::ok;
    usreq.seq = 0;
    json_input = usreq.toBlock(contxt);
    ASSERT_EQ(R"({"type": "io.odysz.semantic.jprotocol.AnsonMsg","body": [],)"
              R"("code": "ok","header": {"type": "io.odysz.semantic.jprotocol.AnsonHeader","iv64": "","ssToken": "","ssid": "","uid": "","usrAct": []},)"
              R"("port": "echo","seq": 0,"version": ""})",
              json_input)  << "serialize usreq";
}

TEST(T06_JAVAENUM, WSPORT) {
    JsonOpt contxt{&asts};
    register_jserv(&contxt);
    // load_usereqAst_test(asts, "ast/usereq.ast.json");
    register_doctier(&contxt, "ast");
    register_iport<WSPort>(&contxt, "ast/wsport.ast.json");

    std::string json_input = format(R"({{"type": "{}", "port": "{}"}})",
                                    AnsonMsg<DocsReq>::_type_,
                                    WSPort(&contxt, WSPort::ping).valof());

    AnsonMsg<DocsReq> dreq{&contxt};
    bool res = Anson::from_json(json_input, dreq, &contxt);

    ASSERT_TRUE(res);
    // ISSUE
    // This will fail:  ASSERT_EQ(WSPort{WSPort::ping}, dreq.port);
    // So WSPort is redundant or needing an other type of DocsReq?
    string port_anclass = dreq.port.anclass;
    WSPort wsport_x{&contxt, WSPort::ping};
    ASSERT_EQ(wsport_x.valof(), dreq.port.valof());
    ASSERT_EQ((WSPort{&contxt, WSPort::ping}), dreq.port);

    dreq.port = WSPort{&contxt, WSPort::ping};

    std::regex json_port{R"("port": "ping")"};
    std::regex json_port_val{R"("port": "ping.ws")"};

    string json_output = dreq.toBlock(contxt);
    ASSERT_TRUE(regex_search(json_output, json_port));
    ASSERT_FALSE(regex_search(json_output, json_port_val));
}
