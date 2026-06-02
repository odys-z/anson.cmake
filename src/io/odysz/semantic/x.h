#pragma once

#include "../anson.h"

namespace anson {
class TransException : public IJsonable, public std::logic_error {
};
}
