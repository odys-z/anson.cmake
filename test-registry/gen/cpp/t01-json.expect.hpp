#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"
#include "jprotocol.h"

using namespace entt::literals;

namespace anson {
inline void register_meta() {

    entt::meta_factory<anson::EchoReq>()
        .type("EchoReq"_hs)
        .base<anson::AnsonBody>()
        ;

    entt::meta_factory<anson::AnsonMsg<EchoReq>>()
        .type("AnsonMsgEchoReq"_hs)
        .base<anson::Anson>()
        .ctor<>()
        .ctor<anson::IPort>()
        .ctor<anson::IPort, anson::MsgCode>()
        .data<&anson::AnsonMsg<EchoReq>::body>("body"_hs, "body")
        .data<&anson::AnsonMsg<EchoReq>::port>("port"_hs, "port")
        ;

    entt::meta_factory<anson::WSEchoReq>()
        .type("WSEchoReq"_hs)
        .base<anson::AnsonBody>()
        .data<&anson::io.odysz.semantic.jserv.echo.WSEchoReq::echo>("echo"_hs, "echo")
        ;

    entt::meta_factory<anson::AnsonMsg<WSEchoReq>>()
        .type("AnsonMsgWSEchoReq"_hs)
        .base<anson::Anson>()
        .ctor<>()
        .ctor<anson::IPort>()
        .ctor<anson::IPort, anson::MsgCode>()
        .data<&anson::AnsonMsg<WSEchoReq>::body>("body"_hs, "body")
        .data<&anson::AnsonMsg<WSEchoReq>::port>("port"_hs, "port")
        ;

}
}
    