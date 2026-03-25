#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

#include <io/odysz/semantier.h>
#include "io/odysz/jprotocol.h"

using json = nlohmann::json;
using namespace anson;


void register_asts(map<string, AnsonAst> &asts, map<string, meta_type> &enttypes) {
    hashed_string enttype;
    string anclass;
    //
    enttype = hashed_string{"io.odysz.anson.IJasonable"};
    entt::meta_factory<anson::Anson>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::IJsonable::anclass>("anclass"_hs, "anclass")
        ;

    AnsonAst ast = asts["io.odysz.anson.IJasonable"] = AnsonAst{"io.odysz.anson.IJsonable", false};
    ast.enttypeid = enttype;

    //
    enttype = hashed_string{Anson::_type_.c_str()};
    entt::meta_factory<anson::Anson>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::Anson::type>("type"_hs, "type")
        ;

    anclass = Anson().anclass;
    ast = asts[anclass] = AnsonAst{anclass, false};
    ast.base = "io.odysz.anson.IJsonable";
    ast.enttypeid = enttype;

    //
    enttype = hashed_string{AnsonAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonAst>()
        .type(enttype)
        .base<Anson>()
        .ctor<>()
        .ctor<string, bool>()
        .ctor<string, string>()
        .data<&anson::AnsonAst::isInt>("isInt"_hs, "isInt")
        .data<&anson::AnsonAst::isDouble>("isDouble"_hs, "isDouble")
        .data<&anson::AnsonAst::isEnum>("isEnum"_hs, "isEnum")
        .data<&anson::AnsonAst::isList>("isList"_hs, "isList")
        .data<&anson::AnsonAst::isMap>("isMap"_hs, "isMap")
        .data<&anson::AnsonAst::istring>("istring"_hs, "istring")
        .data<&anson::AnsonAst::isJsonable>("isJsonable"_hs, "isJsonable")
        .data<&anson::AnsonAst::isJavaEnum>("isJavaEnum"_hs, "isJavaEnum")
        .data<&anson::AnsonAst::enttypeid>("enttypeid"_hs, "enttypeid")
        .data<&anson::AnsonAst::dataAnclass>("dataAnclass"_hs, "dataAnclass")
        .data<&anson::AnsonAst::fields>("fields"_hs, "fields")
        .data<&anson::AnsonAst::enums>("enums"_hs, "enums")
        ;

    anclass = AnsonAst().anclass;
    ast = asts[anclass] = AnsonAst{anclass};
    ast.fields = map<string, AnstField>{
        {"isList", {.fieldname="isList", .dataAnclass="boolean"}},
        {"isJavaEnum", {.fieldname="isJavaEnum", .dataAnclass="boolean"}},
        {"dataAnclass", {.fieldname="dataAnclass", .dataAnclass="String"}},
        {"fields", {.fieldname="fields", .dataAnclass="map<string, io.odysz.anson.AnstField"}},
    };
    ast.enttypeid = enttype;

    //
    enttype = hashed_string{AnsonJavaEnumAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonJavaEnumAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonJavaEnumAst::encode>("encode"_hs, "encode")
        .data<&anson::AnsonJavaEnumAst::decode>("decode"_hs, "decode")
        ;

    anclass = AnsonJavaEnumAst::_type_; // AnsonJavaEnumAst().anclass;
    AnsonJavaEnumAst jeast = AnsonJavaEnumAst{AnsonJavaEnumAst::_type_};
    asts[anclass] = jeast;
    jeast.base = AnsonAst::_type_;
    jeast.enttypeid = enttype;

    //
    enttype = hashed_string{AnsonBodyAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonBodyAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonBodyAst::A>("A"_hs, "A")
        ;

    anclass = AnsonBodyAst().anclass;
    AnsonBodyAst bdast = AnsonBodyAst{anclass};
    asts[anclass] = bdast;
    bdast.base = AnsonAst::_type_;
    bdast.enttypeid = enttype;
}

void register_peersettings(map<string, AnsonAst> &asts, map<string, meta_type> &enttypes) {
    //
    hashed_string enttype = hashed_string{PeerSettings::_type_.c_str()};
    entt::meta_factory<anson::PeerSettings>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .data<&anson::PeerSettings::ansons>("ansons"_hs, "ansons")
        .data<&anson::PeerSettings::scopeEnums>("scopeEnums"_hs, "scopeEnums")
        .data<&anson::PeerSettings::javaEnums>("javaEnums"_hs, "javaEnums")
        .data<&anson::PeerSettings::ansonMsg>("ansonMsg"_hs, "ansonMsg")
        .data<&anson::PeerSettings::ansonBody>("ansonBody"_hs, "ansonBody")
        .data<&anson::PeerSettings::anRequests>("anRequests"_hs, "anRequests")
        ;

    string anclass = PeerSettings().anclass;
    AnsonAst ast = AnsonAst{anclass, false};
    ast.dataAnclass = PeerSettings::_type_;
    ast.base = Anson::_type_;
    ast.enttypeid = enttype;
    asts[anclass] = ast;
}

void register_port(map<string, AnsonAst> &asts, map<string, meta_type> &enttypes) {
    string javaenum = AnsonJavaEnumAst().anclass;
    hashed_string enttype = hashed_string{javaenum.c_str()};
    entt::meta_factory<anson::AnsonJavaEnumAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonJavaEnumAst::dataBase>("dataBase"_hs, "dataBase")
        .data<&anson::AnsonJavaEnumAst::dataAnclass>("dataAnclass"_hs, "dataAnclass")
        .data<&anson::AnsonJavaEnumAst::encode>("encode"_hs, "encode")
        .data<&anson::AnsonJavaEnumAst::decode>("decode"_hs, "decode")
        ;

    AnsonJavaEnumAst jeast = AnsonJavaEnumAst{AnsonJavaEnumAst::_type_};
    jeast.dataAnclass = javaenum;
    jeast.base = JavaEnum::_type_;
    jeast.enttypeid = enttype;

    andebug(string_view("===========" + jeast.anclass + ".decode === "));
    string s_decodes = serialize_map_str(jeast.decode, "map<string, string"s);
    andebug(string_view(s_decodes)); // must be the type
    andebug(string_view("===========" + jeast.anclass + ".encode === "));
    andebug(string_view(serialize_map_str(jeast.encode, "map<string, string"))); // must be the type

    jeast.fields = map<string, AnstField>{
        {"encode", {.fieldname="encode", .dataAnclass = "map<string, string"}},
        {"decode", {.fieldname="decode", .dataAnclass = "map<string, string"}},
    };

    asts[javaenum] = jeast;
}

void register_msgs(map<string, AnsonAst> &asts, map<string, meta_type> &enttypes) {

}

TEST(PeerSettings, Load) {
    aninfo(string_view(filesystem::current_path().string()));

    // entt::resolve<std::map<std::string, std::string>>();

    map<string, AnsonAst> asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts, enttypes);
    register_peersettings(asts, enttypes);


    PeerSettings settings;
    std::string json_input = std::format(R"({{"type": "{}"}})", PeerSettings::_type_);
    EnTTSaxParser handler(settings, IJsonable::contxt_ptr);

    cout << "[0] " << json_input << endl;
    bool result = nlohmann::json::sax_parse(json_input, &handler);

    ASSERT_TRUE(result);
    ASSERT_EQ(PeerSettings::_type_, settings.anclass) << "Errors on parssing {type: input}.";

    string t02_json = "t02-settings.json";
    std::ifstream ifstream(t02_json);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << t02_json << endl;
    }
    result = nlohmann::json::sax_parse(ifstream, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(PeerSettings::_type_, settings.anclass)
        << "Errors on parssing settings.json.";
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, settings.ansonMsg)
        << "expecting " << AnsonMsg<EchoReq>::_type_;
    ASSERT_EQ(AnsonBody::_type_, settings.ansonBody)
        << "expecting " << AnsonBody::_type_;

    aninfo(settings.javaEnums);
    aninfo(settings.scopeEnums);
    aninfo(settings.anRequests);

    ASSERT_EQ(vector<string>{"io.odysz.semantic.jprotocol.Port"}, settings.javaEnums);
    ASSERT_EQ(vector<string>{"io.odysz.semantic.jprotocol.MsgCode"}, settings.scopeEnums) << "expecting [MsgCode]";

    ASSERT_EQ((vector<string>{"io.odysz.semantic.jprotocol.EchoReq",
                             "io.odysz.semantic.jprotocol.SessionReq",
                             "io.odysz.semantic.jprotocol.QueryReq",
                             "io.odysz.semantic.jprotocol.UpdateReq",
                             "io.odysz.semantic.jprotocol.DeleteReq",
                             "io.odysz.semantic.jprotocol.UsersReq"}),
              settings.anRequests) << "settings.anRequests";
    aninfo(string_view(""));
}

/**
 * 1. Regiseter AnsonJavaEnumAst;
 * 2. Load port.ast.json with AnsonJavaEnumAst;
 * 3. Assert the port AST which is supposed to be in the IJonsalbe::contxt.
 */
TEST(AnsonAst, Load_Port) {
    map<string, AnsonAst> asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts, enttypes);
    register_port(asts, enttypes);

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

/*
TEST(AnsonAst, Load_Echo) {
    map<string, AnsonAst> asts;
    map<string, meta_type> enttypes;
    JsonOpt contxt{&asts, &enttypes};
    IJsonable::contxt_ptr = &contxt;

    register_asts(asts, enttypes);
    register_port(asts, enttypes);
    register_msgs(asts, enttypes);

    string ast_echo = "ast/echo.ast.json";
    std::ifstream ifstream(ast_echo);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << ast_echo << endl;
    }

    AnsonBodyAst echoAst;
    EnTTSaxParser handler(echoAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsonBodyAst::_type_, echoAst.type) << "echoAst.type.";
    ASSERT_EQ(AnsonBodyAst().anclass, echoAst.anclass) << "echoAst.anclass";
    ASSERT_EQ(Anson::_type_, echoAst.base) << "echoAst.base";
    // ASSERT_EQ(AnsonMsg<EchoReq>().anclass, echoAst.dataAnclass) << "echoAst.dataAnclass";

    ASSERT_EQ((map<string, string>{
                  {"ehco", "echo"},
              }), echoAst.A) << "echoAst.A";

    ASSERT_EQ((map<string, AnstField>{
                  {"echo", AnstField("echo", "String")},
              }), echoAst.fields) << "echoAst.fields";
}
*/
