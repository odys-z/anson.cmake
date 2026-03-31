/**
 * The eqivalent of gen.antlr.json + JSONAnsonListener
 */
#pragma once

#include <nlohmann/json.hpp>
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>
#include <entt/meta/container.hpp>
#include <string>
#include "anson.h"
#include "jprotocol.h"

namespace anson {

using namespace entt;
using namespace entt::literals;

void register_asts(map<string, AnsonAst> &asts) {
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
    AnsonAst ast = AnsonAst{anclass, false};
    ast.enttypeid = enttype;
    asts[anclass] = ast;

    //
    enttype = hashed_string{AnsonField_type.c_str()};
    entt::meta_factory<anson::AnsonField>()
        .type(enttype)
        .data<&anson::AnsonField::fieldname>("fieldname")
        .data<&anson::AnsonField::dataAnclass>("dataAnclass")
        .data<&anson::AnsonField::valType>("valType")
        ;

    ast = AnsonAst(AnsonField_type);
    ast.enttypeid = enttype;
    ast.base = "";
    ast.fields = map<string, AnsonField>{
        {"fieldname", {.fieldname="fieldname", .dataAnclass="string"}},
        {"valType", {.fieldname="valType", .dataAnclass="string"}},
        {"dataAnclass", {.fieldname="dataAnclass", .dataAnclass="string"}}
    };

    asts[AnsonField_type] = ast;

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
    ast = AnsonAst{anclass, false};
    ast.base = "io.odysz.anson.IJsonable";
    ast.enttypeid = enttype;
    asts[anclass] = ast;


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
    ast = AnsonAst{anclass};
    ast.fields = map<string, AnsonField>{
        {"isList", {.fieldname="isList", .dataAnclass="boolean"}},
        {"isJavaEnum", {.fieldname="isJavaEnum", .dataAnclass="boolean"}},
        {"dataAnclass", {.fieldname="dataAnclass", .dataAnclass="String"}},
        {"fields", {.fieldname="fields", .dataAnclass="map<string, io.odysz.anson.AnstField"}},
    };
    ast.enttypeid = enttype;
    asts[anclass] = ast;

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
    jeast.base = AnsonAst::_type_;
    jeast.enttypeid = enttype;
    asts[anclass] = jeast;

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
    AnsonBodyAst bdast = AnsonBodyAst{anclass};
    bdast.base = AnsonAst::_type_;
    bdast.dataAnclass = anclass;
    bdast.enttypeid = enttype;

    bdast.fields = map<string, AnsonField>{
        {"uri", {.fieldname="uri", .dataAnclass = "string"}},
        {"A",   {.fieldname="A", .dataAnclass = "map<string, string"}}
    };
    asts[anclass] = bdast;

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
    AnsonMsgAst msgast = AnsonMsgAst{anclass};
    msgast.base = AnsonAst::_type_;
    msgast.enttypeid = enttype;
    msgast.fields = map<string, AnsonField> {
        {"bodyAnclass", {.fieldname="bodyAnclass", .dataAnclass = "string"}},
        {"bodyAst", {.fieldname="bodyAst", .dataAnclass = "string"}},
        {"portAnclass", {.fieldname="portAnclass", .dataAnclass = "string"}},
        {"portAst", {.fieldname="portAst", .dataAnclass = "string"}}
    };
    asts[anclass] = msgast;
}

void register_msg(map<string, AnsonAst> &asts) {
    //
    hashed_string enttype{AnsonBody::_type_.c_str()};
    entt::meta_factory<anson::AnsonBody>()
        .type(enttype)
        .base<Anson>()
        .ctor<string>()
        .ctor<string, string>()
        .data<&anson::AnsonBody::a>("a"_hs, "a") // This is what justifying all fields in ast must be regstered.
        ;

    string anclass = AnsonBody().anclass;
    AnsonAst bdast = AnsonBodyAst{anclass};
    bdast.base = AnsonAst::_type_;
    bdast.enttypeid = enttype;
    bdast.fields = map<string, AnsonField> {
        {"a", {.fieldname="a", .dataAnclass = "string"}}
    };
    asts[anclass] = bdast;

}

void register_port(map<string, AnsonAst> &asts) {
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

    jeast.fields = map<string, AnsonField>{
        {"encode", {.fieldname="encode", .dataAnclass = "map<string, string"}},
        {"decode", {.fieldname="decode", .dataAnclass = "map<string, string"}},
    };

    asts[javaenum] = jeast;
}

inline void register_meta(map<string, AnsonAst>& asts, map<string, meta_type> &meta_types) {
    // Register Anson Base
    entt::meta_factory<anson::Anson>()
        .type(hashed_string{Anson::_type_.c_str()})
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::Anson::type>("type")
        ;

    AnsonAst ast = asts[Anson::_type_]; // already loaded = AnsonAst{true};
    // ast.base = "IJsonable";

    // Register SemanticObject
    entt::meta_factory<anson::SemanticObject>()
        .type("SemanticObject"_hs)
        .ctor<>()
        .base<anson::Anson>();

    // Register AnsonBody
    entt::meta_factory<anson::AnsonBody>()
        .type("AnsonBody"_hs)
        .ctor<const std::string&>()
        .ctor<const std::string&, const std::string&>()
        .base<anson::Anson>()
        .data<&anson::AnsonBody::a>("a");

    // Register UserReq
    entt::meta_factory<anson::UserReq>()
        .type("UserReq"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::UserReq::data>("data");

    // Register EchoReq
    entt::meta_factory<anson::EchoReq>()
        .type("EchoReq"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::EchoReq::echo>("echo");

    // Register AnsonResp
    entt::meta_factory<anson::AnsonResp>()
        .type("AnsonResp"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>();

    // Register AnsonMsg template (example for EchoReq)
    AnsonMsg<EchoReq> msg_echoreq;
    string ast_id = msg_echoreq.anclass; // "io.odysz.jprotocol.AnsonMsg<EchoReq>"
    entt::meta_factory<anson::AnsonMsg<anson::EchoReq>>()
        // .type("AnsonMsgEchoReq"_hs)
        .type(entt::hashed_string{ast_id.c_str()})
        .ctor<>()
        .ctor<anson::Port>()
        .base<anson::Anson>()
        .data<&anson::AnsonMsg<anson::EchoReq>::port>("port")
        .data<&anson::AnsonMsg<anson::EchoReq>::body>("body");

    ast = asts[ast_id] = AnsonAst(AnsonMsg<EchoReq>::_type_, true);
    ast.base = Anson::_type_;
    ast.enttypeid = hashed_string{ast_id.c_str()};

    hashed_string astid;
    astid = hashed_string{Port::_type_.c_str()};
    // ast.fields["port"].enttype_id(astid);

    astid = hashed_string{EchoReq::_type_.c_str()};
    // ast.fields["body"].enttype_id(astid);


    // Register Port enum
    entt::meta_factory<anson::JavaEnum>()
        .type("JavaEnumPort"_hs, "JavaEnumPort")
        .base<IJsonable>()
        .data<&anson::JavaEnum::enm>("enm")
        ;

    entt::meta_factory<anson::Port>()
        .type("Port"_hs, "Port")
        .base<anson::JavaEnum>()
        .ctor<std::string>() // TODO: Force to check
        .data<&anson::Port::echo>("echo")
        ;

    {
        // Port::decode["query"]   = "r.serv";
        // Port::decode["update"]  = "u.serv";
        // Port::decode["insert"]  = "c.serv";
        // Port::decode["del"]     = "d.serv";
        // Port::decode["echo"]    = "echo.less";
        // Port::decode["file"]    = "file.serv";
        // Port::decode["docstie"] = "docs.tier";

        // Port::encode["r.serv"]   = "query";
        // Port::encode["u.serv"]   = "update";
        // Port::encode["c.serv"]   = "insert";
        // Port::encode["d.serv"]   = "del";
        // Port::encode["echo.less"]= "echo";
        // Port::encode["file.serv"]= "file";
        // Port::encode["docs.tier"]= "docstie";
    }

    // Register MsgCode enum
    entt::meta_factory<anson::MsgCode>()
        .type("MsgCode"_hs)
        .data<anson::MsgCode::ok>("ok"_hs)
        .data<anson::MsgCode::exSession>("exSession"_hs)
        .data<anson::MsgCode::exSemantic>("exSemantic"_hs)
        .data<anson::MsgCode::exIo>("exIo"_hs)
        .data<anson::MsgCode::exTransct>("exTransct"_hs)
        .data<anson::MsgCode::exDA>("exDA"_hs)
        .data<anson::MsgCode::exGeneral>("exGeneral"_hs)
        .data<anson::MsgCode::ext>("ext"_hs);

    // map<string, int> *msgCodeMap = new map<string, int>{
    //     {"ok", 0}, {"exSession", 1}, {"exSemantic", 2}, {"exIo", 3}, {"exTransct", 4}, {"exDA", 5}, {"exGeneral", 6}, {"ext", 7}
    // };

    ast = asts["MsgCode"] = AnsonAst("MsgCode", true);

    // AnsonAst port_ast{"MsgCode", true};
    ast.anclass = "MsgCode";
    ast.isEnum = true;
    /*
    ast.fields["ok"] = AnstField{.datatype="int", .fieldname = "ok", .static_val="0"};
    ast.fields["exSession"] = AnstField{.datatype="int", .fieldname = "exSession", .static_val="1"};
    ast.fields["exSemantic"] = AnstField{.datatype="int", .fieldname = "exSemantic", .static_val="2"};
    ast.fields["exIo"] = AnstField{.datatype="int", .fieldname = "exIo", .static_val="3"};
    ast.fields["exTransct"] = AnstField{.datatype="int", .fieldname = "exIo", .static_val="4"};
    ast.fields["exDA"] = AnstField{.datatype="int", .fieldname = "exDA", .static_val="5"};
    ast.fields["exGeneral"] = AnstField{.datatype="int", .fieldname = "exGeneral", .static_val="6"};
    ast.fields["ext"] = AnstField{.datatype="int", .fieldname = "ext", .static_val="7"};
    */
}

inline void register_meta(map<string, map<string, int>*>& enum_vals) {
    using namespace entt::literals;

    // Register Anson Base
    entt::meta_factory<anson::Anson>()
        .type("Anson"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<IJsonable>()
        ; //.data<&anson::Anson::type>("type"_hs, "type");

    // Register SemanticObject
    entt::meta_factory<anson::SemanticObject>()
        .type("SemanticObject"_hs)
        .ctor<>()
        .base<anson::Anson>();

    // Register AnsonBody
    entt::meta_factory<anson::AnsonBody>()
        .type("AnsonBody"_hs)
        .ctor<const std::string&>()
        .ctor<const std::string&, const std::string&>()
        .base<anson::Anson>()
        .data<&anson::AnsonBody::a>("a"_hs, "a");

    // Register UserReq
    entt::meta_factory<anson::UserReq>()
        .type("UserReq"_hs)
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::UserReq::data>("data"_hs, "data");

    // Register EchoReq
    entt::meta_factory<anson::EchoReq>()
        .type("EchoReq"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::EchoReq::echo>("echo"_hs, "echo");

    // Register AnsonResp
    entt::meta_factory<anson::AnsonResp>()
        .type("AnsonResp"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>();

    // Register AnsonMsg template (example for EchoReq)
    entt::meta_factory<anson::AnsonMsg<anson::EchoReq>>()
        .type("AnsonMsgEchoReq"_hs)
        .ctor<anson::Port>()
        .base<anson::Anson>()
        .data<&anson::AnsonMsg<anson::EchoReq>::port>("port"_hs, "port")
        .data<&anson::AnsonMsg<anson::EchoReq>::body>("body"_hs, "body");

    // Register Port enum
    entt::meta_factory<anson::JavaEnum>()
        .type("JavaEnumPort"_hs, "JavaEnumPort")
        .base<IJsonable>()
        .data<&anson::JavaEnum::enm>("enm"_hs, "enm")
        ;

    entt::meta_factory<anson::Port>()
        .type("Port"_hs, "Port")
        .base<anson::JavaEnum>()
        .ctor<std::string>() // TODO: Force to check
        .data<&anson::Port::echo>("echo"_hs, "echo")
        ;

    /*{
        Port::decode["query"]   = "r.serv";
        Port::decode["update"]  = "u.serv";
        Port::decode["insert"]  = "c.serv";
        Port::decode["del"]     = "d.serv";
        Port::decode["echo"]    = "echo.less";
        Port::decode["file"]    = "file.serv";
        Port::decode["docstie"] = "docs.tier";

        Port::encode["r.serv"]   = "query";
        Port::encode["u.serv"]   = "update";
        Port::encode["c.serv"]   = "insert";
        Port::encode["d.serv"]   = "del";
        Port::encode["echo.less"]= "echo";
        Port::encode["file.serv"]= "file";
        Port::encode["docs.tier"]= "docstie";
    }*/

    // Register MsgCode enum
    entt::meta_factory<anson::MsgCode>()
        .type("MsgCode"_hs)
        .data<anson::MsgCode::ok>("ok"_hs)
        .data<anson::MsgCode::exSession>("exSession"_hs)
        .data<anson::MsgCode::exSemantic>("exSemantic"_hs)
        .data<anson::MsgCode::exIo>("exIo"_hs)
        .data<anson::MsgCode::exTransct>("exTransct"_hs)
        .data<anson::MsgCode::exDA>("exDA"_hs)
        .data<anson::MsgCode::exGeneral>("exGeneral"_hs)
        .data<anson::MsgCode::ext>("ext"_hs);

    map<string, int> *msgCodeMap = new map<string, int>{
        {"ok", 0}, {"exSession", 1}, {"exSemantic", 2}, {"exIo", 3}, {"exTransct", 4}, {"exDA", 5}, {"exGeneral", 6}, {"ext", 7}
    };

    enum_vals["MsgCode"] = msgCodeMap;
}
}
