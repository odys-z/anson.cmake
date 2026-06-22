#pragma once

#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>

namespace anson {
/**
 * @brief The WSPort class
 * TODO generate code
 */
class WSPort : public JavaEnum {
public:
    inline static const std::string _type_ = "io.oz.anclient.ipcagent.WSPort";
    inline static const std::string doclient = "doclient.ws";
    inline static const std::string configIPC = "config.ws";
    inline static const std::string echo = "echo.ws";
    inline static const std::string ping = "ping.ws";

    WSPort(): JavaEnum(_type_, "na") {
        andebug("WSPort Default Cosntructor");
    }

    WSPort(string enum_val) : JavaEnum(_type_, enum_val) {
        andebug("WSPort Cosntructor<string>("s + enum_val + ").enm = " + enm);
    }
};
}
