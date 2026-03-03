#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"
#include "jprotocol.h"

using namespace entt::literals;

namespace anson {
inline void register_meta() {
    // Register Port enum
    entt::meta_factory<anson::Port>()
        .type("Port"_hs)
        ;

    // Register MsgCode enum
    entt::meta_factory<anson::MsgCode>()
        .type("MsgCode"_hs)
        ;

    entt::meta_factory<anson::IJsonable>()
        .type("IJsonable"_hs)
        // .data<&anson::IJsonable::s_test0>("s_test0"_hs, "s_test0")
        // .data<&anson::IJsonable::s_test1>("s_test1"_hs, "s_test1")
        .data<nullptr, [](IJsonable &instance) -> char& { return instance.s_test0; }>("s_test0"_hs, "s_test0")
        .data<nullptr, [](IJsonable &instance) -> char& { return instance.s_test1; }>("s_test1"_hs, "s_test1")
        .data<&anson::IJsonable::s_test2>("s_test2"_hs, "**s_test2")
        .data<&anson::IJsonable::s_test3>("s_test3"_hs, "**s_test3")
        ;

    entt::meta_factory<anson::Anson>()
        .type("Anson"_hs)
        .ctor<>()
        .ctor<string>()
        .data<&anson::Anson::type>("type"_hs, "type")
        ;

    entt::meta_factory<anson::SemanticObject>()
        .type("SemanticObject"_hs)
        .data<&anson::SemanticObject::data>("data"_hs, "data")
        ;

    entt::meta_factory<anson::AnsonBody>()
        .type("AnsonBody"_hs)
        .ctor<>()
        .ctor<string>()
        .ctor<string, string>()
        .data<&anson::AnsonBody::a>("a"_hs, "a")
        ;

    entt::meta_factory<anson::EchoReq>()
        .type("EchoReq"_hs)
        .base<anson::EchoReq>()
        .ctor<>()
        .ctor<string>()
        .data<&anson::EchoReq::echo>("echo"_hs, "echo")
        ;

    entt::meta_factory<anson::UserReq>()
        .type("UserReq"_hs)
        .ctor<string>()
        .data<&anson::UserReq::data>("data"_hs, "data")
        ;

    entt::meta_factory<anson::AnsonResp>()
        .type("AnsonResp"_hs)
        .ctor<>()
        .ctor<string>()
        .data<&anson::AnsonResp::code>("code"_hs, "code")
        ;

    entt::meta_factory<anson::AnsonMsg<EchoReq>>()
        .type("AnsonMsgEchoReq"_hs)
        .ctor<Port>()
        .ctor<Port>()
        .data<&anson::AnsonMsg<EchoReq>::body>("body"_hs, "body")
        .data<&anson::AnsonMsg<EchoReq>::port>("port"_hs, "port")
        ;

    entt::meta_factory<anson::OnError>()
        .type("OnError"_hs)
        ;

}
}
