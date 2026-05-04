#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"
#include "jprotocol.h"

using namespace entt::literals;

namespace anson {
inline void register_meta() {

    entt::meta_factory<anson::Device>()
        .type("Device"_hs)
        .base<anson::Anson>()
        .data<&anson::io.odysz.semantic.tier.docs.Device::id>("id"_hs, "id")
        .data<&anson::io.odysz.semantic.tier.docs.Device::synode0>("synode0"_hs, "synode0")
        .data<&anson::io.odysz.semantic.tier.docs.Device::devname>("devname"_hs, "devname")
        .data<&anson::io.odysz.semantic.tier.docs.Device::tofolder>("tofolder"_hs, "tofolder")
        ;

    entt::meta_factory<anson::PageInf>()
        .type("PageInf"_hs)
        .base<anson::Anson>()
        .data<&anson::io.odysz.transact.sql.PageInf::page>("page"_hs, "page")
        .data<&anson::io.odysz.transact.sql.PageInf::size>("size"_hs, "size")
        .data<&anson::io.odysz.transact.sql.PageInf::total>("total"_hs, "total")
        .data<&anson::io.odysz.transact.sql.PageInf::arrCondts>("arrCondts"_hs, "arrCondts")
        .data<&anson::io.odysz.transact.sql.PageInf::mapCondts>("mapCondts"_hs, "mapCondts")
        ;

    entt::meta_factory<anson::PathsPage>()
        .type("PathsPage"_hs)
        .base<anson::Anson>()
        .data<&anson::io.odysz.semantic.tier.docs.PathsPage::device>("device"_hs, "device")
        .data<&anson::io.odysz.semantic.tier.docs.PathsPage::start>("start"_hs, "start")
        .data<&anson::io.odysz.semantic.tier.docs.PathsPage::end>("end"_hs, "end")
        .data<&anson::io.odysz.semantic.tier.docs.PathsPage::clientPaths>("clientPaths"_hs, "clientPaths")
        ;

    entt::meta_factory<anson::DocsReq>()
        .type("DocsReq"_hs)
        .base<anson::AnsonBody>()
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::synuri>("synuri"_hs, "synuri")
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::docTabl>("docTabl"_hs, "docTabl")
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::doc>("doc"_hs, "doc")
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::pageInf>("pageInf"_hs, "pageInf")
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::stamp>("stamp"_hs, "stamp")
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::device>("device"_hs, "device")
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::org>("org"_hs, "org")
        .data<&anson::io.odysz.semantic.tier.docs.DocsReq::syncingPage>("syncingPage"_hs, "syncingPage")
        ;

    entt::meta_factory<anson::AnsonMsg<DocsReq>>()
        .type("AnsonMsgDocsReq"_hs)
        .base<anson::Anson>()
        .ctor<>()
        .ctor<anson::IPort>()
        .ctor<anson::IPort, anson::MsgCode>()
        .data<&anson::AnsonMsg<DocsReq>::body>("body"_hs, "body")
        .data<&anson::AnsonMsg<DocsReq>::port>("port"_hs, "port")
        ;

}
}
    