#pragma once

#include "../anson.h"

namespace anson {

class SemanticException : public IJsonable, public std::logic_error {
public:

    SemanticException(string m) : logic_error(m) {}

    const IJsonable* toBlock(ostream& os, const JsonOpt& opts) const {
        os << "\"TODO SemanticExcetion: " << what() << '"';
        return this;
    }

    const IJsonable* toJson(string& buf) const {
        buf += "\"TODO SemanticExcetion: ";
        buf += what();
        buf += '"';
        return this;
    }
};

}
