#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include "anson.h"

namespace anson {
public class EchoReq : public anson::AnsonBody {
public:
    struct A {
        inline static const string echo = "echo";
        inline static const string inet = "inet";
    };
};

public class WSEchoReq : public anson::AnsonBody {
public:
    struct A {
        inline static const string echo = "echo";
        inline static const string x = "x";
    };
    echo String;
};

}