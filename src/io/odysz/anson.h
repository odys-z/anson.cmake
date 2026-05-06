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
        : asts(asts), primtypes(primtypes_c20),
        // this initializer can only be generated?
        astyps({{"io.odysz.anson.AnsonAst", "AnsonAst"},
                {"io.odysz.anson.AnsonJavaEnumAst", "AnsonJavaEnumAst"},
                {"io.odysz.anson.AnsonBodyAst", "AnsonBodyAst"},
                {"io.odysz.anson.AnsonMsgAst", "AnsonMsgAst"}}),
        serialize_type(false) {};

    template<typename AST>
    AST* ast(const string &astid) const {
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
};

class IJsonable {

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

    IJsonable(string anclass) : anclass(anclass) {}

    virtual const IJsonable* toBlock(ostream& os, const JsonOpt& opts) const = 0;

    /** @see #toBlock(OutputStream, JsonOpt...) */
    virtual string toBlock(JsonOpt& opt) {
        // ByteArrayOutputStream bos = new ByteArrayOutputStream();
        // toBlock(bos, opt);
        // return bos.toString(StandardCharsets.UTF_8.name());
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

    JavaEnum(string dataAnclass, string e);
    JavaEnum(const JavaEnum&) = default;
    JavaEnum(JavaEnum&&) = default;

    JavaEnum& operator=(const JavaEnum&) = default;
    JavaEnum& operator=(JavaEnum&&) noexcept = default;

    string url() {
        return enm;
    }

    string valof() const;
    // static JavaEnum& valof(string s, JavaEnum& stub) {
    //     return stub;
    // }

    const IJsonable* toBlock(ostream& stream, const JsonOpt& opts) const override {
        stream << this;
        return this;
    }

    const IJsonable* toJson(string& buf) const override {
        buf += '\"' + valof() + '\"';
        return this;
    }
};

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
public:
    virtual ~Anson() = default;

    inline static const string _type_ = "io.odysz.anson.Anson";

    string type;

    Anson() : IJsonable(_type_), type(_type_) {
    }

    Anson(string t) : IJsonable(t), type(t) {
        // andebug("override constructor, type = "s + t);
    }

    /**
     * @brief Anson
     * @param t
     * @param astid can be different if is a template specialized.
     */
    Anson(string t, string anclass) : IJsonable(anclass), type(t) {
    }

    template <typename T>
    static bool from_json(const string& json, T &an, const JsonOpt *opts = nullptr) {
        return parse(json, an, opts);
    }

    string toBlock(const JsonOpt &jsopt) {
        std::stringstream ss;
        toBlock(ss, jsopt);
        return std::move(ss).str();
    }

    const IJsonable* toBlock(ostream& os, const JsonOpt& opts) const override;
    // {
    //     serialize_envelope(os, *this, opts);
    //     return this;
    // }

    const IJsonable* toJson(string& buf) const override {
        anerror("Don't call this in cpp");
        return this;
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

    /**
     * When parsing a map or list's complex value, the element need to be created with
     * a constructor at compile time. See EnttSAXParser.start_array() & start_object().
     *
     * @brief nest_val_ctor
     */
    // std::function<meta_any()> nest_val_ctor = nullptr;

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
}

