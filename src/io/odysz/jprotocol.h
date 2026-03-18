#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include "anson.h"
#include "common.h"

namespace anson {

class AnResultset : public Anson {
    struct Column {
        int colx;
        string col_id;
    };

    vector<vector<vector<string>>> rows;
    map<string, Column> colmuns;
};

/**
 * @brief The AnsonBody class
 * java type: io.odysz.semantic.jprotocol
 */
class AnsonBody : public anson::Anson {
public:
    inline static const string _type_ = "io.odysz.jprotocol.AnsonBody";
    virtual string _type_special(string msgtype) { return msgtype + "<AnsonBody>"; }

    string a;

    AnsonBody() : Anson(_type_) {}

    AnsonBody(string a) : Anson(_type_) , a(a) {}

    AnsonBody(string a, string type) : Anson(type), a(a) {}
};

class EchoReq: public AnsonBody {
public:
    inline static const std::string _type_ = "io.odysz.jprotocol.EchoReq";
    string _type_special(string msgtype) { return msgtype + "<EchoReq>"; }

    string echo;

    EchoReq() : AnsonBody("r/query", EchoReq::_type_) {}

    EchoReq(string echo) : AnsonBody("r/query", EchoReq::_type_), echo(echo) {}
};

class UserReq : public AnsonBody {
public:
    inline static const string _type_ = "io.odysz.jprotocol.UserReq";
    map<string, entt::any> data;
    UserReq() : UserReq("null") {}
    UserReq(string a) : AnsonBody(a, _type_) {}
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
    string m;
    vector<AnResultset> rs;
    map<string, Anson> map;

    AnsonResp() : AnsonBody("NA", "io.odysz.semantic.jprotocol.AnsonResp") {}

    AnsonResp(string a) : AnsonBody(a, "io.odysz.semantic.jprotocol.AnsonResp") {}

    AnsonResp& Code(MsgCode c) {
        code = c;
        return *this;
    }

    AnsonResp* msg(string_view m) {
        this->m = std::move(m);
        return this;
    }
};

/// TODO? c20 template<std::derived_from<AnsonBody> T = AnsonBody>
/// With anson::AnsonBoyd, any subclass of AnsonBody will be registered by specialize this templated class, AnsonMsg.
template <
    typename T //anson::AnsonBody
    >
class AnsonMsg: public Anson {
public:
    inline static const std::string _type_ = "io.odysz.jprotocol.AnsonMsg";

    vector<shared_ptr<T>> body;

    Port port;

    AnsonMsg() : Anson(_type_, T()._type_special(_type_)), port("NA") {
        // anclass = T()._type_special(_type_);
    }

    // AnsonMsg(Port port) : Anson(_type_ + std::string(typeid(T).name())), port(port) {
    AnsonMsg(Port port) : Anson(_type_, T()._type_special(_type_)), port(port) {
        // anclass = T()._type_special(_type_);
        cout << port.enm;
    }

    AnsonMsg(Port port, const T& body) : Anson(_type_, T()._type_special(_type_)), port(port) {
        // anclass = T()._type_special(_type_);
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

class JServUrl : public HttpParts {
public:
    // static UrlValidator urlValidator;
    string protocolroot;

    JServUrl(string url) : HttpParts() {
        HttpParts parts;
        Regex::getHttpParts(url, parts);

        this->https = parts.https;
        this->port = parts.port;
        this->scheme = std::move(parts.scheme);
        this->host = std::move(parts.host);
        this->paths = std::move(parts.paths);
        this->query = std::move(parts.query);
        this->fragment = std::move(parts.fragment);
    }

    string jserv() {
        return format("{}://{}:{}/{}", this->scheme, this->host, this->port, this->protocolroot);
    }
};

class OnOk {
    virtual void ok(const AnsonResp &resp);
};

// class OnError {
//     // virtual void err(MsgCode c, string& e, string... args);
//     virtual void err(MsgCode code, std::string_view msg,std::initializer_list<std::string_view> args);
// };
using OnError = std::function<void(MsgCode code, std::string_view msg, vector<std::string_view> &args)>;

class OnProgress {
    // using OnProgress = std::function<void(const string& path, std::string status)>;
    virtual void progess(const string& path, std::string status);
};

class JProtocol {
public:

    string protocolpath;
};
}

