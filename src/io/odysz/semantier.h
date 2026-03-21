#pragma once


#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"
#include "common.h"

namespace anson {

class PeerSettings : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.PeerSettings";

    vector<string> ansons;
    vector<string> scopeEnums;
    vector<string> javaEnums;
    string ansonMsg;
    string ansonBody;
    vector<string> ansonMsgs;
    vector<string> anRequests;

    PeerSettings() : Anson(_type_) {}
    
};
}
