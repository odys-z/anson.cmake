#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

#include "io/odysz/entt_jserv.h"
#include "io/odysz/semantier.h"
#include "echoreq.expect.h"

using json = nlohmann::json;
using namespace anson;

TEST(Load, PeerSettings) {
    aninfo(filesystem::current_path().string());

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_peersettings(asts);

    PeerSettings settings;
    std::string json_input = std::format(R"({{"type": "{}"}})", PeerSettings::_type_);
    EnTTSaxParser handler(settings, IJsonable::contxt_ptr);

    andebug("[0] " + json_input);
    settings.type = "";
    bool result = nlohmann::json::sax_parse(json_input, &handler);

    ASSERT_TRUE(result);
    ASSERT_EQ(PeerSettings::_type_, settings.anclass) << "Errors on parssing {type: input}.";

    string t03_json = "t03-settings.json";
    std::ifstream ifstream(t03_json);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << t03_json << endl;
    }

    EnTTSaxParser handler2(settings, IJsonable::contxt_ptr);
    settings.type = "";
    result = nlohmann::json::sax_parse(ifstream, &handler2);
    ASSERT_TRUE(result);

    aninfo(settings.javaEnums);
    aninfo(settings.scopeEnums);
    aninfo(settings.anRequests);

    ASSERT_EQ(PeerSettings::_type_, settings.anclass)
        << "settings.anclass";
    ASSERT_EQ(PeerSettings::_type_, settings.type)
        << "settings.type";
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, settings.ansonMsg)
        << "settings.ansonMsg";
    ASSERT_EQ(AnsonBody::_type_, settings.ansonBody)
        << "settings.ansonBody";

    ASSERT_EQ(vector<string>{"io.odysz.semantic.jprotocol.MsgCode"}, settings.scopeEnums) << "settings.scopeEnums";
    ASSERT_EQ(vector<string>{"io.odysz.semantic.jprotocol.Port"}, settings.javaEnums) << "settings.javaEnums";

    ASSERT_EQ((vector<string>{"ast/echo.ast.json",
                             "io.odysz.semantic.jprotocol.SessionReq",
                             "io.odysz.semantic.jprotocol.QueryReq",
                             "io.odysz.semantic.jprotocol.UpdateReq",
                             "io.odysz.semantic.jprotocol.DeleteReq",
                             "io.odysz.semantic.jprotocol.UsersReq"}),
              settings.anRequests) << "settings.anRequests";

    ASSERT_EQ("semantier.gen.hpp", settings.cpp_gen) << "settings.cpp_gen";
}

/**
 * 1. Regiseter AnsonJavaEnumAst;
 * 2. Load port.ast.json with AnsonJavaEnumAst;
 * 3. Assert the port AST which is supposed to be in the IJonsalbe::contxt.
 */
TEST(Load, AnsonAst_Port) {
    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_port(asts);
    anlog(to_aststring(asts), PrintFormat{.sep="\n"});

    string ast_port = "ast/port.ast-only-4-t03.json";
    std::ifstream ifstream(ast_port);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << ast_port << endl;
    }

    aninfo(string_view("Parsing Port..."));
    AnsonJavaEnumAst portAst{};
    portAst.dataAnclass = Port::_type_;
    EnTTSaxParser handler(portAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonJavaEnumAst::_type_, portAst.type) << "portAst.type";
    ASSERT_EQ(AnsonJavaEnumAst().anclass, portAst.anclass) << "portAst.anclass";
    ASSERT_EQ("io.odysz.anson.JavaEnum", portAst.baseAnclass) << "portAst.baseAnclass";

    string port_anclass = Port().anclass;
    andebug(string_view(port_anclass));

    ASSERT_EQ(Port().anclass, portAst.dataAnclass) << "portAst.dataAnclass";

    ASSERT_EQ((map<string, string>{
                {"heartbeat", "ping.serv"},
                {"session", "login.serv"},
                {"echo", "echo.less"},
                {"del", "d.serv"},
                {"query", "r.serv"},
                {"insert", "c.serv"},
                {"update", "u.serv"},
                {"file", "file.serv"},
                {"userstier", "users.tier"},
                {"stree", "s-tree.serv"},
                {"dataset", "ds.serv"},
                {"datasetier", "ds.tier"},
                {"docstier", "docs.tier"},
                {"syntier", "sync.tier"}
        }), portAst.encode) << "echoAst.encode";

    ASSERT_EQ((map<string, string>{
                {"ping.serv", "heartbeat"},
                {"login.serv", "session"},
                {"echo.less", "echo"},
                {"d.serv", "del"},
                {"r.serv", "query"},
                {"c.serv", "insert"},
                {"u.serv", "update"},
                {"file.serv", "file"},
                {"users.tier", "userstier"},
                {"s-tree.serv", "stree"},
                {"ds.serv", "dataset", },
                {"ds.tier", "datasetier"},
                {"docs.tier", "docstier"},
                {"sync.tier", "syntier"}
              }), portAst.decode) << "echoAst.decode";
}

TEST(Load, EchoReq) {
    AstMap asts;
    JsonOpt contxt{&asts};
    register_jserv(asts, contxt);
    load_echoAst_test(asts, "ast/echo.ast.json");

    string t03_echo_json = "t03_echo.body.json";
    std::ifstream ifstream(t03_echo_json);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << t03_echo_json << endl;
    }

    EchoReq echo{};
    EnTTSaxParser handler(echo, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(EchoReq::_type_, echo.anclass)
        << "Errors on parssing settings.json.";
    ASSERT_EQ("echo", echo.a) << "echo.a";
    ASSERT_EQ("echo ...", echo.echo) << "echo.echo";
}

TEST(Load, EchoAst) {
    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_port(asts);

    string echo_msg = "ast/echo-msg.ast-only-4-t03.json";
    std::ifstream ifmsgstream(echo_msg);
    if (!ifmsgstream.is_open()) {
        FAIL() << "Could not open the file! " << echo_msg << endl;
    }
    AnsonMsgAst echomsgAst;
    EnTTSaxParser handler(echomsgAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifmsgstream, &handler);
    ASSERT_TRUE(result);

    ASSERT_EQ("ast/echo.ast.json", echomsgAst.bodyAst) << "echo-msg.json";
    ASSERT_EQ(AnsonMsgAst::_type_, echomsgAst.type) << "echo-msg.type";
    ASSERT_EQ(AnsonMsgAst().anclass, echomsgAst.anclass) << "echo-msg.anclass";
    ASSERT_EQ(Port::_type_, echomsgAst.portAnclass) << "echo-msg.portAnclass";

    std::ifstream ifstream(echomsgAst.bodyAst);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << echomsgAst.bodyAst << endl;
    }

    AnsonBodyAst echoAst;
    EnTTSaxParser handler2(echoAst, IJsonable::contxt_ptr);
    result = nlohmann::json::sax_parse(ifstream, &handler2);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonBodyAst::_type_, echoAst.type) << "echoAst.type.";
    ASSERT_EQ(AnsonBodyAst().anclass, echoAst.anclass) << "echoAst.anclass";
    ASSERT_EQ(Anson::_type_, echoAst.base) << "echoAst.base";

    ASSERT_EQ((map<string, string>{
                {"echo", "echo"}, {"inet", "inet"}
              }), echoAst.A) << "echoAst.A";

    ASSERT_EQ((map<string, AnsonField>{
                {"echo", AnsonField("", "java.lang.String")},
              }), echoAst.fields) << "echoAst.fields";
}
