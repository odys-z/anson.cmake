#pragma once

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>

#include "common.h"

namespace anson {

using namespace std ;
using namespace entt;

class  AnsonAst;
struct AnsonField;

using AstMap = map<string, unique_ptr<AnsonAst>>;

class JsonOpt {
protected:
    map <string, string> polymorphs;

    /**
     * ISSUE Not multiple level of polymorph?
     * @brief polymorph
     * @param astid
     * @return the target (subclass) type to be morphed.
     */
    string polymorph(const string& astid) const {
        return polymorphs.contains(astid) ? polymorphs.at(astid) : astid;
    }

public:
    bool serialize_type;
    bool escape4DB;
    string doubleFormat;
    string indent;

    const map<string, string> astyps;
    /** @deprecated */
    const map<string, string> primtypes;
    const AstMap *asts;

    JsonOpt(const AstMap *asts)
        : asts(asts), primtypes(primtypes_c20()),
        astyps({{"io.odysz.anson.AnsonAst", "AnsonAst"},
                {"io.odysz.anson.AnsonJavaEnumAst", "AnsonJavaEnumAst"},
                {"io.odysz.anson.AnsonBodyAst", "AnsonBodyAst"},
                {"io.odysz.anson.AnsonMsgAst", "AnsonMsgAst"}}),
        serialize_type(true) {
        andebug(std::format("prim-types's size: {}", primtypes.size()));
    };

    template<typename AST>
    AST* ast(const string &astId) const {
        string astid = polymorph(astId);

        auto it = asts->find(astid);
        if (it != asts->end()) {
            return dynamic_cast<AST*>(it->second.get());
        }
        if (!primtypes.contains(astid))
            if (!LangExt::has_ctor(astid))
                anerror(std::format("JsonOpt.ast(): cannot find ast with id: '{}'.", astid));
        return nullptr;
    }

    bool has_ast(const string &astid) const {
        return asts->contains(astid);
    }

    bool is_ast(const string &astid) const {
        return astyps.contains(astid);
    }

    bool register_polymorph(const string& super_name, const string& subclass_name) {
        bool has = polymorphs.contains(super_name);
        polymorphs[super_name] = subclass_name;
        return has;
    }
};

class anostream : public std::streambuf {
private:
    std::streambuf* destBuffer;
    const JsonOpt& options;

protected:
    int overflow(int ch) override {
        if (ch == EOF) return EOF;
        char c = static_cast<char>(ch);

        if      (c == '\n') return writeStr("\\n");
        else if (c == '\t') return writeStr("\\t");
        else if (c == '\r') return writeStr("\\r");
        else if (c == '\"') return writeStr("\\\"");
        else if (c == '\\') return writeStr("\\\\");
        else if (options.escape4DB && c == '\'') return writeStr("''");

        return destBuffer->sputc(c);
    }

private:
    int writeStr(const std::string& s) {
        for (char c : s) {
            if (destBuffer->sputc(c) == EOF) {
                return EOF;
            }
        }
        return 0;
    }

public:
    std::ostream& os;

    anostream(std::ostream& os, const JsonOpt& opts)
        : os(os), destBuffer(os.rdbuf()), options(opts) {
        os.rdbuf(this);
    }

    ~anostream() {
        os.rdbuf(destBuffer);
    }
};

inline ostream& LangExt::serialize_var(ostream& os, const entt::meta_any & v, const JsonOpt& opt) {
    using namespace entt::literals;

    if (!v) return os << "null";

    if (auto* var = v.try_cast<LangExt::VarType>()) {
        return std::visit([&](auto&& arg) -> ostream& {
            return serialize_var(os, entt::meta_any{arg}, opt);
        }, *var);
    }

    auto type = v.type();

    if (type == entt::resolve<int>()) return os << v.cast<int>();
    if (type == entt::resolve<double>() || type == entt::resolve<float>()) {
        std::string s = std::to_string(v.cast<double>());
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s += '0';
        return os << s;
    }

    if (type == entt::resolve<std::string>() || type == entt::resolve<const std::string>() ||
        type == entt::resolve<char*>() || type == entt::resolve<const char*>()) {

        os << '"';
        {
            anostream filter(os, opt);
            // if (auto* s = v.try_cast<std::string>())       os << *s;
            // else if (auto* s = v.try_cast<char*>())        os << *s;
            // else if (auto* s = v.try_cast<const char*>())  os << *s;

            if (auto* s = v.try_cast<std::string>())
                os << *s;
            else if (auto* s = v.try_cast<const std::string>())
                os << *s;
            else if (auto* s = v.try_cast<char*>())
                os << *s;
            else if (auto* s = v.try_cast<const char*>())
                os << *s;
        }
        return os << '"';
    }

    if (auto* tp = v.try_cast<std::chrono::system_clock::time_point>())
        return os << format("{:%Y-%m-%d %H:%M:%S}", floor<std::chrono::seconds>(*tp));

    return os << "null";
}

class IJsonable {
protected:
    void Anclass(const string & anc) {anclass = anc;}

    /** Only for direct subclasses, Anson, JavaEnum, etc. */
    IJsonable(const string &ancls): anclass(ancls) {}

public:
    inline static const string _anclass_ = "io.odysz.anson.IJsonable";

    inline static JsonOpt* contxt_ptr = nullptr;

    /**
     * Backgroud:
     *
     * 1. Java template type is type-erasing, by losting Template type parameter's value;
     *
     * 2. Java has Object.getClass().getName(), wich is missing in c++.
     *
     * 3. Entt meta needs to register specialized template classes, so this field is used
     *    as an AST id, and is missing/ignored in java.
     */
    string anclass;

    IJsonable() {}

    virtual const IJsonable* toBlock(ostream& os, const JsonOpt& opts) const = 0;

    /** @see #toBlock(OutputStream, JsonOpt...) */
    virtual string toBlock(JsonOpt& opt) const {
        std::ostringstream bos;
        toBlock(bos, opt);
        return bos.str();
    }

    /**
     * @param buf
     * @return this
     * @throws IOException
     * @throws AnsonException
     */
    virtual const IJsonable* toJson(string& buf) const = 0;

    virtual ~IJsonable() {}
};

/** This is invisible in java. */
class JavaEnum : public IJsonable {

public:
    inline static const string _type_ = "io.odysz.anson.JavaEnum";

    string enm;

    JavaEnum(const string& dataAnclass, const string& e_v);
    JavaEnum(const JavaEnum& e) { enm = e.enm; anclass = e.anclass; }
    JavaEnum(JavaEnum&&) = default;

    JavaEnum& operator=(const JavaEnum&);
    // JavaEnum& operator=(const JavaEnum&) = default;
    JavaEnum& operator=(JavaEnum&&) noexcept = default;

    string url() {
        return enm;
    }

    string valof() const;

    const IJsonable* toBlock(ostream& stream, const JsonOpt& opts) const override {
        stream << this;
        return this;
    }

    const IJsonable* toJson(string& buf) const override {
        buf += '\"' + valof() + '\"';
        return this;
    }
    // bool operator==(const JavaEnum& other) const {
    //     return enm == other.enm && anclass == other.anclass;
    // }
};

inline JavaEnum& JavaEnum::operator=(const JavaEnum& other) {
    enm = other.enm;
    return *this;
}

inline static std::ostream& operator<<(std::ostream& os, const JavaEnum& enm) {
    return os << '\"' << enm.valof() << '\"';
}

template<typename T>
class EnTTSaxParser;
class Anson;

template<typename T>
inline static bool parse(const string& json, T &an, const JsonOpt *opts);

/**
 * @brief The Anson class
 * java type: io.odysz.anson.Anson
 */
class Anson : public IJsonable {
protected:
    void Type(const string &t, const string &anclass) {
        type = t;
        Anclass(anclass);
    }

    void Type(const string &t) { Type(t, t); }

    /** Shortcut for direct subclasses */
    Anson(const string &tp) : IJsonable(tp), type(tp) {}

    /**
     * Shortcut for direct subclasses
     * @brief Anson
     * @param tp
     * @param ancls (astid) can be different if is a template specialized.
     */
    Anson(const string &tp, const string &ancls) : IJsonable(ancls), type(tp) {}

public:
    virtual ~Anson() = default;

    inline static const string _type_ = "io.odysz.anson.Anson";

    string type;

    Anson() : type(_type_) { Type(_type_); }

    template <typename T>
    static bool from_json(const string& json, T &an, const JsonOpt *opts = nullptr) {
        return parse(json, an, opts);
    }

    template <typename An>
    static bool from_file(const string& pth, An & an);

    string toBlock(const JsonOpt &jsopt = *IJsonable::contxt_ptr) const {
        std::stringstream ss;
        toBlock(ss, jsopt);
        return std::move(ss).str();
    }

    const IJsonable* toBlock(ostream& os, const JsonOpt& opts) const override;

    const IJsonable* toJson(string& buf) const override {
        anerror("Don't call this in cpp");
        return this;
    }

    static std::string escape(const std::string& s, const JsonOpt& opts) {
        std::stringstream ss;
        {
            anostream filter(ss, opts);
            filter.os << s;
        }
        return ss.str();
    }

    static std::string unescape(const std::string& s, const JsonOpt& opts) {
        // shouldn't be used?
        return s;
    }
};

inline static const string AnsonField_type = "io.odysz.anson.AnsonField";
struct AnsonField {

    /** No conter-part in java */
    string fieldname;

    /** ast-id, i.e. the java valType */
    string dataAnclass;

    /** Not planned to support ptr members, but parsing/serialization requires. */
    string isptr = "false";

    /**
     * Element type. Not used currently.
     *
     * dataAnclass represent the field itself's type, while valType is the map's value type.
     * if dataAnclass == map<string, list<string, valuType == list<string.
     */
    string valType;

    bool operator==(const AnsonField& other) const {
        return fieldname == other.fieldname
               && dataAnclass == other.dataAnclass
               && valType == other.valType;
    }

    friend std::ostream& operator<<(std::ostream& os, const AnsonField& obj) {
        return os << "{fieldname: \"" << obj.fieldname << "\", "
                  << "dataAnclass: \"" << obj.dataAnclass << "\", "
                  << "valType: \"" << obj.valType << "\" }";
    }

};

class SemanticObject : public Anson {
public:
    inline static string _type_ = "io.odysz.semantics.SemanticObject";

    map<string, std::any> data;

    SemanticObject() : Anson(_type_) { }
};

class SessionInf : public Anson {

public:
    inline static string _type_ = "io.odysz.semantics.SessionInf";
    string ssid;
    string uid;
    string roleId;
    string userName;
    string roleName;

    /**
     * Session Token
     * @since 1.4.37
     */
    string ssToken;

    /**
     * Last Sequence
     * @since 1.4.37
     */
    int seq;

    string device;

    SessionInf() : Anson(_type_) { }
};

class AnsonException : public Anson, public std::runtime_error {
    inline static const string _type_ = "io.odysz.anson.AnsonException";
protected:
    AnsonException(const string& typ, const string & err) : Anson(typ), runtime_error(err) {}

public:
    AnsonException() : AnsonException(_type_, _type_) {}

    AnsonException(const string & err) : AnsonException(_type_, err) {}
};

template <typename An>
bool Anson::from_file(const string& pth, An & an) {
    EnTTSaxParser h(an, IJsonable::contxt_ptr);
    an.type = "";

    if (!h.contxt || !h.contxt->asts || !h.contxt->has_ast(Anson::_type_))
        throw AnsonException("Cannot deserialize json without contxt asts registered.");

    std::ifstream ifstream(pth);
    if (!ifstream.is_open()) {
        throw runtime_error("Could not open the file! "s + pth);
    }

    return nlohmann::json::sax_parse(ifstream, &h);
}

}
