#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

#include "io/odysz/json.h"
#include "io/odysz/jprotocol.h"
#include "io/odysz/semantier.h"

using json = nlohmann::json;
using namespace anson;

void register_peersettings(AstMap &asts, map<string, meta_type> &enttypes) {
    //
    hashed_string enttype = hashed_string{PeerSettings::_type_.c_str()};
    entt::meta_factory<anson::PeerSettings>()
        .type(enttype)
        .base<Anson>()
        .ctor<>()
        .data<&anson::PeerSettings::ansons>("ansons"_hs, "ansons")
        .data<&anson::PeerSettings::scopeEnums>("scopeEnums"_hs, "scopeEnums")
        .data<&anson::PeerSettings::javaEnums>("javaEnums"_hs, "javaEnums")
        .data<&anson::PeerSettings::ansonMsg>("ansonMsg"_hs, "ansonMsg")
        .data<&anson::PeerSettings::ansonBody>("ansonBody"_hs, "ansonBody")
        .data<&anson::PeerSettings::anRequests>("anRequests"_hs, "anRequests")
        ;

    string anclass = PeerSettings().anclass;
    AnsonAst *ast = new AnsonAst{anclass, false};
    ast->dataAnclass = PeerSettings::_type_;
    ast->base = Anson::_type_;
    ast->enttypeid = enttype;

    // ast.fields is only used for serialization?
    ast->fields = map<string, AnsonField>{
        {"ansonMsg",   {.fieldname="ansonMsg", .dataAnclass = "string"}},
        {"ansons",   {.fieldname="ansons", .dataAnclass = "list<string"}},
        {"scopeEnums",   {.fieldname="scopeEnums", .dataAnclass = "list<string"}},
        {"javaEnums",   {.fieldname="javaEnums", .dataAnclass = "list<string"}},
        {"ansonBody",   {.fieldname="ansonBody", .dataAnclass = "string"}},
        {"anRequests",   {.fieldname="anRequests", .dataAnclass = "list<string"}},
    };

    // asts[anclass] = unique_ptr<AnsonAst>(ast);
    asts.insert(make_pair(anclass, ast));
    enttypes[anclass] = entt::resolve<PeerSettings>();
}

void register_echoAst(AstMap &asts, map<string, meta_type> &enttypes) {
    hashed_string enttype{EchoReq::_type_.c_str()};
    entt::meta_factory<anson::EchoReq>()
        .type(enttype)
        .base<AnsonBody>()
        .ctor<>()
        .ctor<string>()
        .data<&anson::EchoReq::echo>("echo"_hs, "echo")
        ;

    string astclass = AnsonBodyAst().anclass;
    string echoclass = EchoReq().anclass;
    AnsonBodyAst *echoast = new AnsonBodyAst{astclass};
    echoast->dataAnclass = echoclass;
    echoast->enttypeid = enttype;

    echoast->A["echo"] = "echo";
    echoast->A["inet"] = "inet";

    echoast->fields = map<string, AnsonField>{
        {"echo",   {.fieldname="echo", .dataAnclass = "string"}}
    };
    asts.insert(make_pair(echoclass, echoast));
}

TEST(Load, PeerSettings) {
    aninfo(string_view(filesystem::current_path().string()));

    AstMap asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_peersettings(asts, enttypes);


    PeerSettings settings;
    std::string json_input = std::format(R"({{"type": "{}"}})", PeerSettings::_type_);
    EnTTSaxParser handler(settings, IJsonable::contxt_ptr);

    cout << "[0] " << json_input << endl; settings.type = "";
    bool result = nlohmann::json::sax_parse(json_input, &handler);

    ASSERT_TRUE(result);
    ASSERT_EQ(PeerSettings::_type_, settings.anclass) << "Errors on parssing {type: input}.";

    string t02_json = "t03-settings.json";
    std::ifstream ifstream(t02_json);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << t02_json << endl;
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

    ASSERT_EQ(vector<string>{"io.odysz.semantic.jprotocol.Port"}, settings.javaEnums);
    ASSERT_EQ(vector<string>{"io.odysz.semantic.jprotocol.MsgCode"}, settings.scopeEnums) << "settings.scopeEnums";

    ASSERT_EQ((vector<string>{"io.odysz.semantic.jprotocol.EchoReq",
                             "io.odysz.semantic.jprotocol.SessionReq",
                             "io.odysz.semantic.jprotocol.QueryReq",
                             "io.odysz.semantic.jprotocol.UpdateReq",
                             "io.odysz.semantic.jprotocol.DeleteReq",
                             "io.odysz.semantic.jprotocol.UsersReq"}),
              settings.anRequests) << "settings.anRequests";
}

/**
 * 1. Regiseter AnsonJavaEnumAst;
 * 2. Load port.ast.json with AnsonJavaEnumAst;
 * 3. Assert the port AST which is supposed to be in the IJonsalbe::contxt.
 */
TEST(Load, AnsonAst_Port) {
    AstMap asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_port(asts, "ast/port.ast.json");

    string ast_port = "ast/port.ast.json";
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
    ASSERT_EQ(JavaEnum::_type_, portAst.dataBase) << "portAst.dataBase";

    string port_anclass = Port().anclass;
    andebug(string_view(port_anclass));

    ASSERT_EQ(Port().anclass, portAst.dataAnclass) << "portAst.dataAnclass";

    ASSERT_EQ((map<string, string>{
              {"echo", "echo.less"},
              {"del", "d.serv"},
              {"heartbeat", "ping.serv"},
              }), portAst.decode) << "echoAst.decode";

    ASSERT_EQ((map<string, string>{
              {"echo.less", "echo"},
              {"d.serv", "del"},
              {"ping.serv", "heartbeat"},
              }), portAst.encode) << "echoAst.encode";
}

TEST(Load, EchoReq) {
    AstMap asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_port(asts, "ast/port.ast.json");
    register_msg(asts);
    register_echoAst(asts, enttypes);

    string t02_echo_json = "t03_echo.body.json";
    std::ifstream ifstream(t02_echo_json);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << t02_echo_json << endl;
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
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts);
    register_port(asts, "ast/port.ast.json");

    string echo_msg = "ast/echo-msg.ast.json";
    std::ifstream ifmsgstream(echo_msg);
    if (!ifmsgstream.is_open()) {
        FAIL() << "Could not open the file! " << echo_msg << endl;
    }
    AnsonMsgAst echomsgAst;
    EnTTSaxParser handler(echomsgAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifmsgstream, &handler);
    ASSERT_TRUE(result);

    // string ast_echo = echomsgAst.json  "ast/echo.ast.json";
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
    // ASSERT_EQ(EchoReq().anclass, echoAst.dataAnclass) << "echoAst.dataAnclass";

    ASSERT_EQ((map<string, string>{
                {"echo", "echo"}, {"inet", "inet"}
              }), echoAst.A) << "echoAst.A";

    ASSERT_EQ((map<string, AnsonField>{
                {"echo", AnsonField("", "java.lang.String")},
              }), echoAst.fields) << "echoAst.fields";
}
