#pragma once

#include "../anson.h"

namespace anson {

class SemanticException : public IJsonable, public std::logic_error {
};

}
