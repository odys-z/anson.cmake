#pragma once

#include "io/odysz/anson.h"

namespace anson {

class TestPort : public JavaEnum<TestPort> {
public:
    inline static const std::string echo = "echo.test";
    inline static const std::string t_query = "r.test";
    inline static const std::string t_update = "u.test";

    TestPort() : JavaEnum<TestPort>("na-test") {}
};

}
