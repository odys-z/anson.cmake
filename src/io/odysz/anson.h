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
        : asts(asts), primtypes(primtypes_c20()),
        astyps({{"io.odysz.anson.AnsonAst", "AnsonAst"},
                {"io.odysz.anson.AnsonJavaEnumAst", "AnsonJavaEnumAst"},
                {"io.odysz.anson.AnsonBodyAst", "AnsonBodyAst"},
                {"io.odysz.anson.AnsonMsgAst", "AnsonMsgAst"}}),
        serialize_type(true) {
        andebug(std::format("prim-types's size: {}", primtypes.size()));
    };

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

    // IJsonable(string anclass) : anclass(anclass) {}
    IJsonable() {}

    virtual const IJsonable* toBlock(ostream& os, const JsonOpt& opts) const = 0;

    /** @see #toBlock(OutputStream, JsonOpt...) */
    virtual string toBlock(JsonOpt& opt) const {
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

    JavaEnum(const string& dataAnclass, const string& e_v);
    JavaEnum(const JavaEnum&) = default;
    JavaEnum(JavaEnum&&) = default;

    JavaEnum& operator=(const JavaEnum&) = default;
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
    static bool from_file(const string t03_json, An & an) {
        std::ifstream ifstream(t03_json);
        if (!ifstream.is_open()) {
            throw runtime_error("Could not open the file! "s + t03_json);
        }

        EnTTSaxParser h(an, IJsonable::contxt_ptr);
        an.type = "";
        return nlohmann::json::sax_parse(ifstream, &h);
    }

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

class AnsonException : public IJsonable, public std::runtime_error {
};

}
