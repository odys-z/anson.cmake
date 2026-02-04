/**
 * The eqivalent of gen.antlr.json + JSONAnsonListener
 */
#pragma once

// #include <nlohmann/json.hpp>
#include "anson.h"
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>
#include <entt/meta/container.hpp>
#include <string>
#include "jprotocol.h"

namespace anson {

using namespace entt::literals;

inline void register_meta() {
    using namespace entt::literals;

    // Register Anson Base
    entt::meta_factory<anson::Anson>()
        .type("Anson"_hs)
        .ctor<>()
        .ctor<const std::string&>()
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
        .type("AnsonMsgEcho"_hs)
        .ctor<anson::Port>()
        .base<anson::Anson>()
        .data<&anson::AnsonMsg<anson::EchoReq>::port>("port"_hs, "port")
        .data<&anson::AnsonMsg<anson::EchoReq>::body>("body"_hs, "body");

    // Register Port enum
    entt::meta_factory<anson::Port>()
        .type("Port"_hs)
        .data<anson::Port::query>("query"_hs)
        .data<anson::Port::update>("update"_hs)
        .data<anson::Port::echo>("echo"_hs);

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
}

}
