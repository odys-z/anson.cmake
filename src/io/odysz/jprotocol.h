#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include "anson.h"

namespace anson {

/**
 * @brief The AnsonBody class
 * java type: io.odysz.semantic.jprotocol
 */
class AnsonBody : public anson::Anson {
public:
    inline static const string _type_ = "io.odysz.jprotocol.AnsonBody";

    string a;

    AnsonBody() : Anson(_type_) {}

    AnsonBody(string a) : Anson(_type_) , a(a) {}

    AnsonBody(string a, string type) : Anson(type), a(a) {}
};

class EchoReq: public AnsonBody {
public:
    inline static const std::string _type_ = "io.odysz.jprotocol.EchoReq";

    string echo;

    EchoReq() : AnsonBody("r/query", EchoReq::_type_) {}

    EchoReq(string echo) : AnsonBody("r/query", EchoReq::_type_), echo(echo) {}
};

class UserReq : public AnsonBody {
public:
    map<string, entt::any> data;
    UserReq(string a) : AnsonBody(a, "io.odysz.jprotocol.UserReq") {}
};

/**
 * (In cpp, Port cannot be a enum type)
 */
class Port : public JavaEnum {
public:
    inline static const std::string query = "r.serv";
    inline static const std::string update = "u.serv";
    inline static const std::string insert = "c.serv";
    inline static const std::string del = "d.serv";
    inline static const std::string echo = "echo.less";
    inline static const std::string file = "file.serv";
    /** document manager's semantic tier ("docs.tier") */
    inline static const std::string docstier = "docs.tier";

    Port(string enum_val) : JavaEnum(enum_val) { }
};

inline bool operator==(const Port& p, const Port& q) {
    return p.enm == q.enm;
}

inline bool operator==(const anson::Port& p, const std::string& s) {
    return p.enm == s;
}

inline bool operator==(const std::string& s, const anson::Port& p) {
    // return p == from_enum_string<Port>(s);
    return p == s;
}

// inline std::ostream& operator<<(std::ostream& os, const anson::Port& port_instance) {
//     using namespace entt::literals;

//     auto type = entt::resolve<anson::Port>();

//     if (type) {
//         // 1. Get the meta data for the member variable 'p'
//         // Assuming 'p' was registered with the id "p"_hs
//         auto prop_p = type.data("enm"_hs);

//         if (prop_p) {
//             // Get the actual string value of 'p' from the instance
//             auto value_any = prop_p.get(port_instance);
//             if (auto* val_ptr = value_any.try_cast<std::string>()) {

//                 // 2. Iterate through all registered meta data to find a match
//                 for (auto [id, data] : type.data()) {
//                     // We check if the meta data is a constant matching our value
//                     // This allows you to map "e.less" back to the name "echo"
//                     auto meta_val = data.get({}); // Get static/constant value

//                     if (meta_val && meta_val.try_cast<std::string>() &&
//                         *meta_val.try_cast<std::string>() == *val_ptr) {

//                         // We found a field whose value matches port_instance.p
//                         // Use data.id() or a custom property for the "field name"
//                         return os << "[ data.name() ] " << data.name() << std::endl;
//                     }
//                 }
//             }
//         }
//     }

//     // Fallback if no meta-match is found
//     std::cerr << "WARNING Unknown Port " << port_instance.enm << " (No meta-match found)\n";
//     return os << "\"" << port_instance.enm << "\"";
// }

template<typename E>
std::optional<E> from_enum_string(const std::string& s) {
    using namespace entt::literals;
    auto type = entt::resolve<E>();

    if (type) {
        for (auto [id, data] : type.data()) {
            if (auto prop = data.name()) {
                if (prop == s) {
                    return data.get({}).template cast<E>();
                }
            }
        }
    }
    return std::nullopt;
}

enum class MsgCode { ok, exSession, exSemantic, exIo, exTransct, exDA, exGeneral, ext };

class AnsonResp : public AnsonBody{
public:
    MsgCode code;

    AnsonResp() : AnsonBody("NA", "io.odysz.semantic.jprotocol.AnsonResp") {}

    AnsonResp(string a) : AnsonBody(a, "io.odysz.semantic.jprotocol.AnsonResp") {}
};

// c20 template<std::derived_from<AnsonBody> T = AnsonBody>
// typename T //, typename = std::enable_if_t<std::is_base_of_v<AnsonBody, T>>
/// With anson::AnsonBoyd, any subclass of AnsonBody will be registered by specialize this templated class, AnsonMsg.
template <
    typename T //anson::AnsonBody
    >
class AnsonMsg: public Anson {
public:
    inline static const std::string _type_ = "io.odysz.jprotocol.AnsonMsg";

    vector<shared_ptr<T>> body;

    Port port;

    AnsonMsg(Port port) : Anson(_type_), port(port) {
        cout << "AnsosMsg to port " << port.enm << endl;
    }

    AnsonMsg(Port port, const T& body) : Anson(_type_), port(port) {
        this->Body(body);
    }

    AnsonMsg<T>& Body(const T& body) {
        this->body.push_back(make_shared<T>(body));
        return *this;
    }

    /**
     * The R-value version of AnsonMsg::Body(T&);
     * @brief Body
     * @param b
     * @return this
     * @details
     * These to overriden methods can be merged in the future, like
     * <pre>template<typename U>
     * AnsonMsg<T>& Body(U&& arg) {
     *   // This creates the shared_ptr and handles both copies and moves perfectly
     *   this->body.push_back(std::make_shared<T>(std::forward<U>(arg)));
     *   return *this;
     * }</pre>
     */
    AnsonMsg<T>& Body(T&& body) {
        this->body.push_back(std::make_shared<T>(std::move(body)));
        return *this;
    }

    T& Body() {
        return *body.at(0);
    }
};

class OnError {
    // virtual void err(MsgCode c, string& e, string... args);
    virtual void err(MsgCode code, std::string_view msg,std::initializer_list<std::string_view> args);
};
}

