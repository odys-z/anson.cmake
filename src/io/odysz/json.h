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

inline void register_meta(map<string, map<string, int>*>& enum_vals) {
    using namespace entt::literals;

    // Register Anson Base
    entt::meta_factory<anson::Anson>()
        .type("Anson"_hs)
        .ctor<>()
        .ctor<const std::string&>()
        .base<IJsonable>()
        .data<&anson::Anson::type>("type"_hs, "type");

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
    entt::meta_factory<anson::JavaEnum<Port>>()
        .type("JavaEnumPort"_hs, "JavaEnumPort")
        .base<IJsonable>()
        .data<&anson::JavaEnum<Port>::enm>("enm"_hs, "enm")
        ;

    entt::meta_factory<anson::Port>()
        .type("Port"_hs, "Port")
        .base<anson::JavaEnum<Port>>()
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

    map<string, int> *msgCodeMap = new map<string, int>{
        {"ok", 0}, {"exSession", 1}, {"exSemantic", 2}, {"exIo", 3}, {"exTransct", 4}, {"exDA", 5}, {"exGeneral", 6}, {"ext", 7}
    };

    enum_vals["MsgCode"] = msgCodeMap;
}
}
