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

inline void register_meta(map<string, AnsonAst>& asts, map<string, meta_type> meta_types) {
    using namespace entt::literals;

    // entt::meta_factory<anson::IJsonable>()
    //     .type("IJsonable"_hs)
    //     .ctor<const std::string&>()
    //     ;

    // Register Anson Base
    entt::meta_factory<anson::Anson>()
        .type("Anson"_hs)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        ; // .data<&anson::Anson::type>("type"_hs, "type");

    AnsonAst ast = asts["Anson"] = AnsonAst("Anson", true);
    ast.base = "IJsonable";

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
        .ctor<>()
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
    AnsonMsg<EchoReq> msg_echoreq;
    string ast_id = msg_echoreq.anclass; // "io.odysz.jprotocol.AnsonMsg<EchoReq>"
    entt::meta_factory<anson::AnsonMsg<anson::EchoReq>>()
        // .type("AnsonMsgEchoReq"_hs)
        .type(entt::hashed_string{ast_id.c_str()})
        .ctor<>()
        .ctor<anson::Port>()
        .base<anson::Anson>()
        .data<&anson::AnsonMsg<anson::EchoReq>::port>("port"_hs, "port")
        .data<&anson::AnsonMsg<anson::EchoReq>::body>("body"_hs, "body");

    ast = asts[ast_id] = AnsonAst(AnsonMsg<EchoReq>::_type_, true);
    ast.base = Anson::_type_;
    // ast.meta_type = entt::resolve(hashed_string{ast_id.c_str()});
    ast.enttypeid = hashed_string{ast_id.c_str()};

    hashed_string astid;
    astid = hashed_string{Port::_type_.c_str()};
    ast.fields["port"] = AnstField{.fieldname = "port", .astid = Port::_type_, .enttypeid = astid};

    // if is template, generate somthing like:
    // EchoReq req{};
    // astid = req.anclass;
    astid = hashed_string{EchoReq::_type_.c_str()};
    ast.fields["body"] = AnstField{.fieldname="body", .astid=EchoReq::_type_, .enttypeid=astid};




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

    {
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
    ast.fields["ok"] = AnstField{.datatype="int", .fieldname = "ok", .static_val="0"};
    ast.fields["exSession"] = AnstField{.datatype="int", .fieldname = "exSession", .static_val="1"};
    ast.fields["exSemantic"] = AnstField{.datatype="int", .fieldname = "exSemantic", .static_val="2"};
    ast.fields["exIo"] = AnstField{.datatype="int", .fieldname = "exIo", .static_val="3"};
    ast.fields["exTransct"] = AnstField{.datatype="int", .fieldname = "exIo", .static_val="4"};
    ast.fields["exDA"] = AnstField{.datatype="int", .fieldname = "exDA", .static_val="5"};
    ast.fields["exGeneral"] = AnstField{.datatype="int", .fieldname = "exGeneral", .static_val="6"};
    ast.fields["ext"] = AnstField{.datatype="int", .fieldname = "ext", .static_val="7"};
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
