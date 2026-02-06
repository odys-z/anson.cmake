#pragma once
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"
#include "jprotocol.h"

namespace anson {
inline void register_meta() {
    using namespace entt::literals;

    entt::meta_factory<anson::Port>()
        .type("Port"_hs)
        .data<anson::Port::query>("query"_hs, "query")
        .data<anson::Port::update>("update"_hs, "update")
        .data<anson::Port::echo>("echo"_hs, "echo")
        .data<anson::Port::docstier>("docstier"_hs, "docstier")
        ;

    entt::meta_factory<anson::MsgCode>()
        .type("MsgCode"_hs)
        .data<anson::MsgCode::ok>("ok"_hs, "ok")
        .data<anson::MsgCode::exSession>("exSession"_hs, "exSession")
        .data<anson::MsgCode::exSemantic>("exSemantic"_hs, "exSemantic")
        .data<anson::MsgCode::exIo>("exIo"_hs, "exIo")
        .data<anson::MsgCode::exTransct>("exTransct"_hs, "exTransct")
        .data<anson::MsgCode::exDA>("exDA"_hs, "exDA")
        .data<anson::MsgCode::exGeneral>("exGeneral"_hs, "exGeneral")
        .data<anson::MsgCode::ext>("ext"_hs, "ext")
        ;

    entt::meta_factory<anson::IJsonable>()
        .type("IJsonable"_hs)
        .data<&anson::IJsonable::* toBlock(ostream& os, JsonOpt& opts)>("* toBlock(ostream& os, JsonOpt& opts)"_hs, "* toBlock(ostream& os, JsonOpt& opts)")
        .data<&anson::IJsonable::* toJson(string& buf)>("* toJson(string& buf)"_hs, "* toJson(string& buf)");

    entt::meta_factory<anson::Anson>()
        .type("Anson"_hs)
        .data<&anson::Anson::type>("type"_hs, "type");

    entt::meta_factory<anson::SemanticObject>()
        .type("SemanticObject"_hs)
        .data<&anson::SemanticObject::data>("data"_hs, "data");

    entt::meta_factory<anson::AnsonBody>()
        .type("AnsonBody"_hs)
        .data<&anson::AnsonBody::a>("a"_hs, "a");

    entt::meta_factory<anson::EchoReq>()
        .type("EchoReq"_hs)
        .data<&anson::EchoReq::echo>("echo"_hs, "echo");

    entt::meta_factory<anson::UserReq>()
        .type("UserReq"_hs)
        .data<&anson::UserReq::data>("data"_hs, "data");

    entt::meta_factory<anson::AnsonResp>()
        .type("AnsonResp"_hs)
        .data<&anson::AnsonResp::code>("code"_hs, "code");

    entt::meta_factory<anson::OnError>()
        .type("OnError"_hs)
        .data<&anson::OnError::err(MsgCode code, std::string_view msg,std::initializer_list<std::string_view> args)>("err(MsgCode code, std::string_view msg,std::initializer_list<std::string_view> args)"_hs, "err(MsgCode code, std::string_view msg,std::initializer_list<std::string_view> args)");

}
}