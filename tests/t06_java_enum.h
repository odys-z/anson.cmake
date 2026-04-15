#pragma once

#include "io/odysz/anson.h"

namespace anson {

class TestPort : public JavaEnum {
public:
    inline static const std::string echo = "echo.test";
    inline static const std::string t_query = "r.test";
    inline static const std::string t_update = "u.test";

    TestPort(string e) : JavaEnum(e, echo) {}
};

}
