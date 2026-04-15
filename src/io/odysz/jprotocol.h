#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include "common.h"
#include "anson.h"
#include "module/rs.h"

namespace anson {

/**
 * @brief The AnsonBody class
 * java type: io.odysz.semantic.jprotocol
 */
class AnsonBody : public anson::Anson {
public:
    inline static const string _type_ = "io.odysz.semantic.jprotocol.AnsonBody";
    virtual string _type_special(string msgtype) { return msgtype + "<" + _type_; }

    string a;
    string uri;

    AnsonBody() : Anson(_type_) {}

    AnsonBody(string a) : Anson(_type_) , a(a) {}

    AnsonBody(string a, string type) : Anson(type), a(a) {}
};

class EchoReq: public AnsonBody {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jserv.echo.EchoReq";
    string _type_special(string msgtype) { return msgtype + "<" + _type_; }

    string echo;

    EchoReq() : AnsonBody("r/query", EchoReq::_type_) {}

    EchoReq(string echo) : AnsonBody("r/query", EchoReq::_type_), echo(echo) {}
};

class UserReq : public AnsonBody {
public:
    inline static const string _type_ = "io.odysz.semantic.jprotocol.UserReq";
    map<string, entt::any> data;
    UserReq() : UserReq("null") {}
    UserReq(string a) : AnsonBody(a, _type_) {}
};

/**
 * (In cpp, Port cannot be a enum type)
 */
class Port : public JavaEnum {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jprotocol.Port";

    inline static const std::string query = "r.serv";
    inline static const std::string update = "u.serv";
    inline static const std::string insert = "c.serv";
    inline static const std::string del = "d.serv";
    inline static const std::string echo = "echo.less";
    inline static const std::string file = "file.serv";
    /** document manager's semantic tier ("docs.tier") */
    inline static const std::string docstier = "docs.tier";

    Port(): JavaEnum(_type_, "na") {
        andebug(string_view("Port Default Cosntructor"));
    }

    Port(string enum_val) : JavaEnum(_type_, enum_val) {
        andebug(string_view("Port Cosntructor<string>("s + enum_val + ").enm = " + enm));
    }
};

// MsgCode
template <typename E, size_t N>
struct EnEnregistrement {
    using Code = E;
    Code valeur;
    static const string _type_;
    static constexpr size_t compter = N;
    static const std::array<std::string_view, N> noms;

    EnEnregistrement(E v) : valeur(v) {}

    constexpr bool operator==(std::string_view s) const {
        for (int i = 0; i < compter; i++) {
            if (noms[i] == s) return true;
        }
        return false;
    }

    constexpr bool operator==(E v) const {
        return valeur == v;
    }
};

enum class MsgCodeEnum { ok, exSession, exSemantic, exIo, exTransct, exDA, exGeneral, ext, _sentinel_ };
using MsgCode = EnEnregistrement<MsgCodeEnum, static_cast<size_t>(MsgCodeEnum::_sentinel_) + 1>;
template<> const string MsgCode::_type_ = "io.odysz.semantic.jprotocol.MsgCode";

template<> constexpr std::array<std::string_view, 9> MsgCode::noms = {
    "ok", "exSession", "exSemantic", "exIo", "exTransct", "exDA", "exGeneral", "ext", "_sentinal_"
};

class AnsonResp : public AnsonBody{
public:
    inline static const string _type_ = "io.odysz.semantic.jprotocol.AnsonResp";

    // MsgCode code;
    string m;
    vector<AnResultset> rs;
    map<string, Anson> map;

    AnsonResp() : AnsonBody("NA", _type_) {}

    AnsonResp(string a) : AnsonBody(a, _type_) {}

    AnsonResp* msg(string_view m) {
        this->m = std::move(m);
        return this;
    }

    virtual string _type_special(string msgtype) { return msgtype + "<" + _type_; }
};

template <typename T //anson::AnsonBody
         >
class AnsonMsg: public Anson {
public:
    /**
     * Bodies are proected as the error of a unique-ptr will result in
     * memory error for deletion.
     *
     * Public only as needed for entt registration.
     */
    vector<shared_ptr<T>> body;

    inline static const std::string _type_ = "io.odysz.semantic.jprotocol.AnsonMsg";

    Port port;

    MsgCode::Code code;

    AnsonMsg() : Anson(_type_, T()._type_special(_type_)), port("_sentinel_") { }

    AnsonMsg(Port port) : Anson(_type_, T()._type_special(_type_)), port(port.enm) {
        cout << port.enm;
    }

    AnsonMsg(Port port, const T& body) : Anson(_type_, T()._type_special(_type_)), port(port.enm) {
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
        if (body.empty()) {
            throw std::runtime_error("Body list is empty");
        }
        return *body.at(0);
    }

    size_t body_size() {
        return body.size();
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
using OnError = std::function<void(MsgCode::Code code, std::string_view msg, vector<std::string_view> &args)>;

class OnProgress {
    virtual void progess(const string& path, std::string status);
};

class JProtocol {
public:
    string protocolpath;
};
}

