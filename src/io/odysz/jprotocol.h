#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include "anson.h"
#include "common.h"

namespace anson {

class AnResultset : public Anson {
public:
    inline static const string _type_ = "io.odysz.module.rs.AnResultset";

    struct Column {
        inline static const string _type_ = AnResultset::_type_ + ".Column";
        int colx;
        string col_id;
    };

    vector<vector<vector<string>>> rows;
    map<string, Column> colnames;

    /** start at 0 */
    int rowIdx;
    int rowCnt;
    int colcnt;
    long total;

    AnResultset() : Anson(_type_), rowIdx(-1), rowCnt(0), colcnt(0), total(0){}
};

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

// inline static const string MsgCode_anclass_ = "io.odysz.semantic.jprotocol.MsgCode";
// enum class MsgCode { ok, exSession, exSemantic, exIo, exTransct, exDA, exGeneral, ext, _sentinel_ };

// constexpr bool operator==(MsgCode code, std::string_view s) {
//     switch (code) {
//     case MsgCode::ok:        return s == "ok";
//     case MsgCode::exSession: return s == "exSession";
//     case MsgCode::exSemantic:return s == "exSemantic";
//     case MsgCode::exIo:      return s == "exIo";
//     case MsgCode::exTransct: return s == "exTransact";
//     case MsgCode::exDA:      return s == "exDA";
//     case MsgCode::exGeneral: return s == "exGeneral";
//     case MsgCode::ext:       return s == "ext";
//     default:                 return false;
//     }
// };

/**
 * Design Notes:
 *
 * As this module is used for converting string to enumeration back
 * and forth, the bare c++ enum is not enough.
 *
 * Usage:
 *
 * MsgCode::Enm code = MsgCode::ok;  // Clean syntax
 *
 * auto s = MsgCode::toString(code);   // Accesses the "member" logic
 */
namespace MsgCode {
    static inline const string _type_ = "io.odysz.semantic.jprotocol.MsgCode";

    /**
     * Usage:
     *
     * MsgCode::Enm code = MsgCode::ok;
     *
     * auto s = MsgCode::toString(code);
     */
    enum C { ok, exSession, exSemantic, exIo, exTransct, exDA, exGeneral, ext, _sentinel_ };

    static constexpr std::array names
        {"ok", "exSession", "exSemantic", "exIo", "exTransct", "exDA", "exGeneral", "ext"};

    inline constexpr std::string_view toString(C v) {
        auto idx = static_cast<size_t>(v);
        return (idx < names.size()) ? names[idx] : "null";
    }

    constexpr bool operator==(C code, std::string_view s) {
        auto idx = static_cast<size_t>(code);
        if (idx < names.size())
            return names[idx] == s;
        return false;
    }

    constexpr bool operator==(std::string_view s, C code) {
        return code == s;
    }
}

class AnsonResp : public AnsonBody{
public:
    inline static const string _type_ = "io.odysz.semantic.jprotocol.AnsonResp";

    MsgCode::C code;
    string m;
    vector<AnResultset> rs;
    map<string, Anson> map;

    AnsonResp() : AnsonBody("NA", _type_) {}

    AnsonResp(string a) : AnsonBody(a, _type_) {}

    AnsonResp& Code(MsgCode::C c) {
        code = c;
        return *this;
    }

    AnsonResp* msg(string_view m) {
        this->m = std::move(m);
        return this;
    }

    virtual string _type_special(string msgtype) { return msgtype + "<" + _type_; }
};

template <
    typename T //anson::AnsonBody
    >
class AnsonMsg: public Anson {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jprotocol.AnsonMsg";

    vector<shared_ptr<T>> body;

    Port port;

    MsgCode::C code;

    AnsonMsg() : Anson(_type_, T()._type_special(_type_)), port("NA") { }

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
using OnError = std::function<void(MsgCode::C code, std::string_view msg, vector<std::string_view> &args)>;

class OnProgress {
    virtual void progess(const string& path, std::string status);
};

class JProtocol {
public:
    string protocolpath;
};
}

