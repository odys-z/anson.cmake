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
    const map<string, string> primtypes;
    const AstMap *asts;

    JsonOpt(const AstMap *asts)
        : asts(asts), primtypes{
            {"String", "string"}, {"string", "string"}, {"java.lang.String", "string"},
            {"int", "int"}, {"Integer", "int"}, {"java.lang.Integer", "int"},
            {"short", "int"}, {"Short", "int"}, {"java.lang.Short", "int"},
            {"long", "long"}, {"Long", "long"}, {"java.lang.Long", "long"},
            {"float", "float"}, {"Float", "float"}, {"java.lang.Float", "float"},
            {"double", "double"}, {"Double", "double"}, {"java.lang.Double", "double"},
            {"boolean", "boolean"}, {"Boolean", "boolean"}, {"java.lang.Boolean", "boolean"},
            {"VarType", "VarType"}, {"LangExt::VarType", "VarType"}, {"anson::LangExt::VarType", "VarType"},
        },
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

    virtual IJsonable* toBlock(ostream& os, const JsonOpt& opts) = 0;

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
    virtual IJsonable* toJson(string& buf) = 0;

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

    IJsonable* toBlock(ostream& stream, const JsonOpt& opts) override {
        stream << this;
        return this;
    }

    IJsonable* toJson(string& buf) override {
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

// inline static ostream& serialize_envelope(ostream &os, Anson& anson, const JsonOpt &opts);

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

    IJsonable* toBlock(ostream& os, const JsonOpt& opts) override;
    // {
    //     serialize_envelope(os, *this, opts);
    //     return this;
    // }

    IJsonable* toJson(string& buf) override {
        anerror("Don't call this in cpp");
        return this;
    }
};

/**
 * @brief The AnsonAst class
 *
 *  isEnum: bool
 *  base: Union[str, None]
 *  anclass: str
 *  fields: dict
 *  enums: Union[dict, None]
 *  ctors: List[List[str]]
 */
class AnsonAst : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonAst";

    virtual ~AnsonAst() = default;

    inline static bool valid_type(const string& typ) {
        return !LangExt::isblank(typ);
    }

    bool isInt = false;
    bool isDouble = false;
    bool isEnum = false;
    bool isList = false;
    bool isMap = false;
    bool istring = false;
    bool isJsonable = false;
    bool isPortEnum = false;
    bool isVar = false;
    string base = "io.odysz.anson.Anson";

    string dataBaseAst;
    /** The target's anson type or data type, e.g. string */
    string dataAnclass;
    AnsonAst& data_anclass(const string & cls) { dataAnclass = cls; return *this; }

    map<string, AnsonField> fields;
    map<string, int> enums;

    /** Only one static string value is allowed in semantic-* ? */
    string static_val;

    hashed_string enttypeid = hashed_string{_type_.c_str()};

    AnsonAst() : Anson(_type_),
        isInt(false), isDouble(false), isEnum(false), isList(false), isMap(false), istring(false), isJsonable(true), isPortEnum(false) { }

    AnsonAst(string anclass, bool isEnum = false) : Anson(_type_, anclass),
        isInt(false), isDouble(false), isEnum(isEnum), isList(false), isMap(false), istring(false), isJsonable(true), isPortEnum(false) { }

    AnsonAst(string anclass, string type) : Anson(type, anclass),
        isInt(false), isDouble(false), isEnum(false), isList(false), isMap(false), istring(false), isJsonable(true), isPortEnum(false) { }

    std::function<meta_any(IJsonable&, id_type)> get_entt_instance = [](IJsonable& j, id_type t) -> meta_any {
        return {};
    };

    std::function<meta_any(const IJsonable&, const string& fieldname)> get_field_instance =
        [this] (const IJsonable& ans, string fn) -> meta_any{
        if ("type" == fn) {
            auto& concrete = static_cast<const Anson&>(ans);
            return entt::forward_as_meta(concrete.type);
        }
        anwarn(std::format("[WARN] AnsonAst({}): Requring field '{}.{}', this is supposed to be overriden by AST {}.",
                            this->dataAnclass, ans.anclass, fn, ans.anclass));
        return meta_any{false};
    };

    inline const string find_field_astid(const AstMap* asts, const string & fieldname) const ;
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

const string AnsonAst::find_field_astid(const AstMap* asts, const string & fieldname) const {
    if (fields.contains(fieldname))
        return fields.at(fieldname).dataAnclass;
    else if (asts->contains(dataBaseAst)) {
        const AnsonAst *baseAst = asts->at(dataBaseAst).get();
        return baseAst->find_field_astid(asts, fieldname);
    }
    return "";
}

class AnsonJavaEnumAst: public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonJavaEnumAst";

    map<string, string> encode;
    map<string, string> decode;

    AnsonJavaEnumAst() : AnsonJavaEnumAst(_type_) {}

    /**
     * @brief AnsonJavaEnumAst
     * @param anclass
     * @param isEnum This parameter is used only for templatized registration, not used.
     * (AST is not a JavaEnum)
     */
    AnsonJavaEnumAst(string anclass, bool isEnum = false) : AnsonAst(anclass, isEnum) {
    }

    std::function<string(const meta_any& val)> name_of =
        [this] (const meta_any& val) -> string{
            anerror("The <funcion>name_of must be overriden by auto-generated code.");
        return "null";
    };

    template <typename C>
    static inline string name(C e) {
        if (IJsonable::contxt_ptr && IJsonable::contxt_ptr->has_ast(C::_type_)) {
            AnsonJavaEnumAst * jeast = IJsonable::contxt_ptr->ast<AnsonJavaEnumAst>(C::_type_);
            return jeast->name_of({e.valeur});
        }
        return "null";
    }
};

inline bool operator==(const anson::JavaEnum& p, const std::string& s) {
    if (IJsonable::contxt_ptr && IJsonable::contxt_ptr->asts->contains(p.anclass)) {
        AnsonJavaEnumAst * ast = dynamic_cast<AnsonJavaEnumAst*>(IJsonable::contxt_ptr->asts->at(p.anclass).get());
        return p.enm == ast->encode[s];
    }
    return false;
}

inline bool operator==(const std::string& s, const anson::JavaEnum& p) {
    return p == s;
}

inline bool operator==(const anson::JavaEnum& p, const anson::JavaEnum& q) {
    return p.enm == q.enm;
}

class AnsonBodyAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonBodyAst";

    // ISSUE not adding uri to fields?
    string uri;
    map<string, string> A;

    AnsonBodyAst() : AnsonAst(_type_, _type_) { }

    AnsonBodyAst(string anclass, bool isEnum = false) : AnsonAst(anclass, _type_) { }
};

class AnsonMsgAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonMsgAst";

    string bodyAnclass;
    string bodyAst;
    string portAnclass;
    string portAst;

    AnsonMsgAst() : AnsonAst() { }

    AnsonMsgAst(string anclass, bool isEnum = false) : AnsonAst(anclass, _type_) { }
};

inline JavaEnum:: JavaEnum(string anclass, string e) : enm(std::move(e)), IJsonable(anclass) {
    this->anclass = anclass;
    if (contxt_ptr->asts->contains(anclass)) {
        map<string, string> encode = dynamic_cast<AnsonJavaEnumAst*>(contxt_ptr->asts->at(anclass).get())->encode;
        if (encode.contains(enm)) {
            enm = encode[enm];
            return;
        }
    }
    andebug("JavaEnum: "s + enm);
}

inline string JavaEnum::valof() const {
    if (contxt_ptr->asts->contains(anclass)) {
        map<string, string> decode =
            dynamic_cast<AnsonJavaEnumAst*>(contxt_ptr->asts->at(anclass).get())->decode;

        if (decode.contains(enm)) {
            return decode[enm];
        }
    }
    return enm;
}

class SemanticObject : public Anson {
public:
    inline static string _type_ = "io.odysz.semantics.SemanticObject";

    map<string, std::any> data;

    SemanticObject() : Anson(_type_) { }
};
}

