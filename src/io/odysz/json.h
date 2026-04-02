/**
 * The eqivalent of gen.antlr.json + JSONAnsonListener
 */
#pragma once

#include <nlohmann/json.hpp>
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>
#include <entt/meta/container.hpp>
#include <string>
#include <fstream>
#include "anson.h"
#include "jprotocol.h"

namespace anson {

using namespace entt;
using namespace entt::literals;

inline static void register_asts(AstMap &asts) {
    hashed_string enttype;
    string anclass;
    //
    enttype = hashed_string{"io.odysz.anson.IJasonable"};
    entt::meta_factory<anson::IJsonable>()
        .type(enttype)
        // .ctor<>()
        // .ctor<const std::string&>()
        .data<&anson::IJsonable::anclass>("anclass")
        ;

    anclass = IJsonable::_anclass_;
    AnsonAst *ast = new AnsonAst(anclass, false);
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    // asts[anclass] = unique_ptr<AnsonAst>(ast);
    asts.insert(make_pair(anclass, ast));

    //
    enttype = hashed_string{AnsonField_type.c_str()};
    entt::meta_factory<anson::AnsonField>()
        .type(enttype)
        .data<&anson::AnsonField::fieldname>("fieldname")
        .data<&anson::AnsonField::dataAnclass>("dataAnclass")
        .data<&anson::AnsonField::valType>("valType")
        ;

    ast = new AnsonAst(AnsonField_type);
    ast->base = "";
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    ast->fields = map<string, AnsonField>{
        {"fieldname", {.fieldname="fieldname", .dataAnclass="string"}},
        {"valType", {.fieldname="valType", .dataAnclass="string"}},
        {"dataAnclass", {.fieldname="dataAnclass", .dataAnclass="string"}}
    };

    asts[AnsonField_type] = unique_ptr<AnsonAst>(ast);
    asts.insert(make_pair(AnsonField_type, ast));

    //
    enttype = hashed_string{Anson::_type_.c_str()};
    entt::meta_factory<anson::Anson>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::Anson::type>("type")
        ;

    anclass = Anson().anclass;
    ast = new AnsonAst{anclass, false};
    ast->base = "io.odysz.anson.IJsonable";
    ast->dataAnclass = anclass;
    ast->enttypeid = enttype;
    // asts[anclass] = unique_ptr<AnsonAst>(ast);
    asts.insert(make_pair(anclass, ast));


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
    ast = new AnsonAst{anclass};
    ast->dataAnclass = anclass;
    ast->fields = map<string, AnsonField>{
        {"isList", {.fieldname="isList", .dataAnclass="boolean"}},
        {"isJavaEnum", {.fieldname="isJavaEnum", .dataAnclass="boolean"}},
        {"dataAnclass", {.fieldname="dataAnclass", .dataAnclass="String"}},
        {"fields", {.fieldname="fields", .dataAnclass="map<string, io.odysz.anson.AnstField"}},
    };
    ast->enttypeid = enttype;
    // asts[anclass] = unique_ptr<AnsonAst>(ast);
    asts.insert(make_pair(anclass, ast));

    //
    string javaenum = AnsonJavaEnumAst().anclass;
    enttype = hashed_string{javaenum.c_str()};
    entt::meta_factory<anson::AnsonJavaEnumAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonJavaEnumAst::dataBase>("dataBase"_hs, "dataBase")
        .data<&anson::AnsonJavaEnumAst::dataAnclass>("dataAnclass"_hs, "dataAnclass")
        .data<&anson::AnsonJavaEnumAst::encode>("encode"_hs, "encode")
        .data<&anson::AnsonJavaEnumAst::decode>("decode"_hs, "decode")
        ;

    AnsonJavaEnumAst *jeast = new AnsonJavaEnumAst(AnsonJavaEnumAst::_type_);
    jeast->dataAnclass = javaenum;
    jeast->base = JavaEnum::_type_;
    jeast->enttypeid = enttype;

    jeast->fields = map<string, AnsonField>{
        {"encode", {.fieldname="encode", .dataAnclass = "map<string, string"}},
        {"decode", {.fieldname="decode", .dataAnclass = "map<string, string"}},
    };
    asts.insert(make_pair(javaenum, jeast));

    //
    enttype = hashed_string{AnsonBodyAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonBodyAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonBodyAst::uri>("uri"_hs, "uri")
        .data<&anson::AnsonBodyAst::A>("A"_hs, "A")
        ;

    anclass = AnsonBodyAst().anclass;
    AnsonBodyAst *bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->dataAnclass = anclass;
    bdast->enttypeid = enttype;

    bdast->fields = map<string, AnsonField>{
        {"uri", {.fieldname="uri", .dataAnclass = "string"}},
        {"A",   {.fieldname="A", .dataAnclass = "map<string, string"}}
    };
    // asts[anclass] = unique_ptr<AnsonBodyAst>(&bdast);
    asts.insert(make_pair(anclass, bdast));

    //
    enttype = hashed_string{AnsonMsgAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonMsgAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonMsgAst::bodyAnclass>("bodyAnclass"_hs, "bodyAnclass")
        .data<&anson::AnsonMsgAst::bodyAst>("bodyAst"_hs, "bodyAst")
        .data<&anson::AnsonMsgAst::portAnclass>("portAnclass"_hs, "portAnclass")
        .data<&anson::AnsonMsgAst::portAst>("portAst"_hs, "portAst")
        ;

    anclass = AnsonMsgAst().anclass;
    AnsonMsgAst *msgast = new AnsonMsgAst(anclass);
    msgast->base = AnsonAst::_type_;
    msgast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    msgast->fields = map<string, AnsonField> {
        {"bodyAnclass", {.fieldname="bodyAnclass", .dataAnclass = "string"}},
        {"bodyAst", {.fieldname="bodyAst", .dataAnclass = "string"}},
        {"portAnclass", {.fieldname="portAnclass", .dataAnclass = "string"}},
        {"portAst", {.fieldname="portAst", .dataAnclass = "string"}}
    };
    asts[anclass] = unique_ptr<AnsonMsgAst>(msgast);
}

inline static void register_msg(AstMap &asts) {
    //
    hashed_string enttype{SemanticObject::_type_.c_str()};
    entt::meta_factory<anson::SemanticObject>()
    .type("SemanticObject"_hs)
        .ctor<>()
        .base<anson::Anson>()
        .data<&anson::SemanticObject::data>("data")
        ;

    string anclass = SemanticObject().anclass;
    AnsonAst *ast = new AnsonAst(anclass);
    ast->base = Anson::_type_;
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    ast->fields = map<string, AnsonField> {
        {"data", {.fieldname="data", .dataAnclass = "map<string, TODO"}}
    };
    asts[anclass] = unique_ptr<AnsonAst>(ast);

    //
    enttype = hashed_string{AnsonBody::_type_.c_str()};
    entt::meta_factory<anson::AnsonBody>()
        .type(enttype)
        .base<Anson>()
        .ctor<string>()
        .ctor<string, string>()
        .data<&anson::AnsonBody::a>("a") // This is what justifying all fields in ast must be regstered.
        ;

    anclass = AnsonBody().anclass;
    AnsonBodyAst *bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->enttypeid = enttype;
    bdast->dataAnclass = anclass;
    bdast->fields = map<string, AnsonField> {
        {"a", {.fieldname="a", .dataAnclass = "string"}}
    };
    asts[anclass] = unique_ptr<AnsonBodyAst>(bdast);

    //
    enttype = hashed_string{UserReq::_type_.c_str()};
    entt::meta_factory<anson::UserReq>()
        .type(enttype)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::UserReq::data>("data");

    anclass = UserReq().anclass;
    bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->enttypeid = enttype;
    bdast->dataAnclass = anclass;
    bdast->fields = map<string, AnsonField> {
        {"data", {.fieldname="data", .dataAnclass = SemanticObject::_anclass_}}
    };
    asts[anclass] = unique_ptr<AnsonBodyAst>(bdast);

    //
    enttype = hashed_string{AnsonResp::_type_.c_str()};
    entt::meta_factory<anson::AnsonResp>()
        .type(enttype)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::AnsonResp::m>("m")
        .data<&anson::AnsonResp::rs>("rs")
        .data<&anson::AnsonResp::map>("map")
        ;

    anclass = AnsonResp().anclass;
    bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->enttypeid = enttype;
    bdast->dataAnclass = anclass;
    bdast->fields = map<string, AnsonField> {
        {"m", {.fieldname="m", .dataAnclass = "string"}},
        {"rs", {.fieldname="rs", .dataAnclass = AnResultset::_anclass_}},
        {"map", {.fieldname="map", .dataAnclass = SemanticObject::_anclass_}}
    };
    asts[anclass] = unique_ptr<AnsonBodyAst>(bdast);
}

inline static void register_port(AstMap &asts, string port_ast) {
    // string javaenum = AnsonJavaEnumAst().anclass;
    // hashed_string enttype = hashed_string{javaenum.c_str()};
    // entt::meta_factory<anson::AnsonJavaEnumAst>()
    //     .type(enttype)
    //     .base<AnsonAst>()
    //     .ctor<string>()
    //     .data<&anson::AnsonJavaEnumAst::dataBase>("dataBase"_hs, "dataBase")
    //     .data<&anson::AnsonJavaEnumAst::dataAnclass>("dataAnclass"_hs, "dataAnclass")
    //     .data<&anson::AnsonJavaEnumAst::encode>("encode"_hs, "encode")
    //     .data<&anson::AnsonJavaEnumAst::decode>("decode"_hs, "decode")
    //     ;

    // AnsonJavaEnumAst *jeast = new AnsonJavaEnumAst(AnsonJavaEnumAst::_type_);
    // jeast->dataAnclass = javaenum;
    // jeast->base = JavaEnum::_type_;
    // jeast->enttypeid = enttype;

    // jeast->fields = map<string, AnsonField>{
    //     {"encode", {.fieldname="encode", .dataAnclass = "map<string, string"}},
    //     {"decode", {.fieldname="decode", .dataAnclass = "map<string, string"}},
    // };

    // asts.insert(make_pair(javaenum, jeast));
    // asts[javaenum] = unique_ptr<AnsonJavaEnumAst>(jeast);

    std::ifstream ifstream(port_ast);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", port_ast)));
    }

    AnsonJavaEnumAst *portAst = new AnsonJavaEnumAst{};
    portAst->dataAnclass = Port::_type_;
    EnTTSaxParser handler(*portAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        string anclass = portAst->dataAnclass;
        hashed_string enttype = hashed_string{anclass.c_str()};

        // meta_type portype =
        entt::meta_factory<anson::Port>()
            .type(enttype)
            .base<JavaEnum>()
            .ctor<>()
            .ctor<string>()
            ;

        // for field in portAst.fields
        //    type.data<Port::type.name()>();

        portAst->enttypeid = enttype;

        asts[anclass] = unique_ptr<AnsonJavaEnumAst>(portAst);
        // asts.insert(make_pair(anclass, portAst));
    }
    else
        anerror(string_view(std::format("Could not load AST from {}!", port_ast)));
}

// inline void register_meta(map<string, map<string, int>*>& enum_vals) {
//     using namespace entt::literals;

//     // Register Anson Base
//     entt::meta_factory<anson::Anson>()
//         .type("Anson"_hs)
//         .ctor<>()
//         .ctor<const std::string&>()
//         .base<IJsonable>()
//         ; //.data<&anson::Anson::type>("type"_hs, "type");

//     // Register SemanticObject
//     entt::meta_factory<anson::SemanticObject>()
//         .type("SemanticObject"_hs)
//         .ctor<>()
//         .base<anson::Anson>();

//     // Register AnsonBody
//     entt::meta_factory<anson::AnsonBody>()
//         .type("AnsonBody"_hs)
//         .ctor<const std::string&>()
//         .ctor<const std::string&, const std::string&>()
//         .base<anson::Anson>()
//         .data<&anson::AnsonBody::a>("a"_hs, "a");

//     // Register UserReq
//     entt::meta_factory<anson::UserReq>()
//         .type("UserReq"_hs)
//         .ctor<const std::string&>()
//         .base<anson::AnsonBody>()
//         .data<&anson::UserReq::data>("data"_hs, "data");

//     // Register EchoReq
//     entt::meta_factory<anson::EchoReq>()
//         .type("EchoReq"_hs)
//         .ctor<>()
//         .ctor<const std::string&>()
//         .base<anson::AnsonBody>()
//         .data<&anson::EchoReq::echo>("echo"_hs, "echo");

//     // Register AnsonResp
//     entt::meta_factory<anson::AnsonResp>()
//         .type("AnsonResp"_hs)
//         .ctor<>()
//         .ctor<const std::string&>()
//         .base<anson::AnsonBody>();

//     // Register AnsonMsg template (example for EchoReq)
//     entt::meta_factory<anson::AnsonMsg<anson::EchoReq>>()
//         .type("AnsonMsgEchoReq"_hs)
//         .ctor<anson::Port>()
//         .base<anson::Anson>()
//         .data<&anson::AnsonMsg<anson::EchoReq>::port>("port"_hs, "port")
//         .data<&anson::AnsonMsg<anson::EchoReq>::body>("body"_hs, "body");

//     // Register Port enum
//     entt::meta_factory<anson::JavaEnum>()
//         .type("JavaEnumPort"_hs, "JavaEnumPort")
//         .base<IJsonable>()
//         .data<&anson::JavaEnum::enm>("enm"_hs, "enm")
//         ;

//     entt::meta_factory<anson::Port>()
//         .type("Port"_hs, "Port")
//         .base<anson::JavaEnum>()
//         .ctor<std::string>() // TODO: Force to check
//         .data<&anson::Port::echo>("echo"_hs, "echo")
//         ;

//     /*{
//         Port::decode["query"]   = "r.serv";
//         Port::decode["update"]  = "u.serv";
//         Port::decode["insert"]  = "c.serv";
//         Port::decode["del"]     = "d.serv";
//         Port::decode["echo"]    = "echo.less";
//         Port::decode["file"]    = "file.serv";
//         Port::decode["docstie"] = "docs.tier";

//         Port::encode["r.serv"]   = "query";
//         Port::encode["u.serv"]   = "update";
//         Port::encode["c.serv"]   = "insert";
//         Port::encode["d.serv"]   = "del";
//         Port::encode["echo.less"]= "echo";
//         Port::encode["file.serv"]= "file";
//         Port::encode["docs.tier"]= "docstie";
//     }*/

//     // Register MsgCode enum
//     entt::meta_factory<anson::MsgCode>()
//         .type("MsgCode"_hs)
//         .data<anson::MsgCode::ok>("ok"_hs)
//         .data<anson::MsgCode::exSession>("exSession"_hs)
//         .data<anson::MsgCode::exSemantic>("exSemantic"_hs)
//         .data<anson::MsgCode::exIo>("exIo"_hs)
//         .data<anson::MsgCode::exTransct>("exTransct"_hs)
//         .data<anson::MsgCode::exDA>("exDA"_hs)
//         .data<anson::MsgCode::exGeneral>("exGeneral"_hs)
//         .data<anson::MsgCode::ext>("ext"_hs);

//     map<string, int> *msgCodeMap = new map<string, int>{
//         {"ok", 0}, {"exSession", 1}, {"exSemantic", 2}, {"exIo", 3}, {"exTransct", 4}, {"exDA", 5}, {"exGeneral", 6}, {"ext", 7}
//     };

//     enum_vals["MsgCode"] = msgCodeMap;
// }
}
