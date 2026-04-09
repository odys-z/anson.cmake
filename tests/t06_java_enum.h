#pragma once

#include "io/odysz/anson.h"

namespace anson {

class TestPort : public JavaEnum {
public:
    inline static const std::string echo = "echo.test";
    inline static const std::string t_query = "r.test";
    inline static const std::string t_update = "u.test";

    TestPort(string e) : JavaEnum(e) {}

    // 2. Explicitly enable Copy and Move (Required by EnTT)
    // TestPort(const TestPort &)            = default;
    // TestPort(TestPort &&) noexcept        = default;
    // TestPort & operator=(const TestPort &)     = default;
    // TestPort & operator=(TestPort &&) noexcept = default;
};

}
