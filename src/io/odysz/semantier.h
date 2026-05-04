#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"
#include "jprotocol.h"

namespace anson {
class EchoReq: public AnsonBody {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jserv.echo.EchoReq";

    struct A {
        inline static const string echo = "echo";
        inline static const string inet = "inet";
    };

    string echo;

    EchoReq() : AnsonBody("na", EchoReq::_type_) {}

    /**
     * Java:
     * @AnsonCtor(initialist="echo(m)", base={"na"})
     *
     * AST.ctors[1]:
     * [["echo", "string", "m"], ["na"]]
     *
     * @brief EchoReq
     * @param m
     */
    EchoReq(string m) : AnsonBody("na", EchoReq::_type_), echo(m) {}
};

}
