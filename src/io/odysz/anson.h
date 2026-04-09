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
    bool escape4DB;
    string doubleFormat;
    string indent;

    const map<string, string> primtypes;
    const AstMap *asts;

    JsonOpt(const AstMap *asts)
        : asts(asts), primtypes({
            {"String", "string"}, {"string", "string"}, {"java.lang.String", "string"},
            {"int", "int"}, {"Integer", "int"}, {"java.lang.Integer", "int"},
            {"short", "int"}, {"Short", "int"}, {"java.lang.Short", "int"},
            {"long", "long"}, {"Long", "long"}, {"java.lang.Long", "long"},
            {"float", "float"}, {"Float", "float"}, {"java.lang.Float", "float"},
            {"double", "double"}, {"Double", "double"}, {"java.lang.Double", "double"},
            {"boolean", "boolean"}, {"Boolean", "boolean"}, {"java.lang.Boolean", "boolean"},
        }) { }

    template<typename T>
    T* ast(string astid) const {
        auto it = asts->find(astid);
        if (it != asts->end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    bool has_ast(const string& astid) const {
        return asts->contains(astid);
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

    // static JavaEnum& valof(string s, JavaEnum& stub) {
    //     return stub;
    // }

    IJsonable* toBlock(ostream& stream, const JsonOpt& opts) override {
        stream << this;
        return this;
    }

    IJsonable* toJson(string& buf) override {
        buf += '\"' + enm + '\"';
        return this;
    }
};

inline std::ostream& operator<<(std::ostream& os, const JavaEnum& enm) {
    return os << '\"' << enm.enm << '\"';
}

template<typename T>
class EnTTSaxParser;
class Anson;

inline static ostream& serialize_envelope(ostream &os, Anson& anson, const JsonOpt &opts);

template<typename T>
inline static bool parse(const string& json, T &an, const JsonOpt *opts);

/**
 * @brief The Anson class
 * java type: io.odysz.anson.Anson
 */
class Anson : public IJsonable {
public:
    inline static const string _type_ = "io.odysz.anson.Anson";

    string type;

    Anson() : IJsonable(_type_), type(_type_) {
        // andebug("defalut contructor");
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
        // andebug("override constructor, anclass = " + anclass);
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

    IJsonable* toBlock(ostream& os, const JsonOpt& opts) override {
        serialize_envelope(os, *this, opts);
        return this;
    }

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
    bool isJavaEnum = false;
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
        isInt(false), isDouble(false), isEnum(false), isList(false), isMap(false), istring(false), isJsonable(true), isJavaEnum(false) { }

    AnsonAst(string anclass, bool isEnum = false) : Anson(_type_, anclass),
        isInt(false), isDouble(false), isEnum(isEnum), isList(false), isMap(false), istring(false), isJsonable(true), isJavaEnum(false) { }

    AnsonAst(string anclass, string type) : Anson(type, anclass),
        isInt(false), isDouble(false), isEnum(false), isList(false), isMap(false), istring(false), isJsonable(true), isJavaEnum(false) { }
};

inline static const string AnsonField_type = "io.odysz.anson.AnsonField";
struct AnsonField {

    /** No conter-part in java */
    string fieldname;

    /** ast-id, i.e. the java valType */
    string dataAnclass;

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

class AnsonJavaEnumAst: public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonJavaEnumAst";

    map<string, string> encode;
    map<string, string> decode;

    AnsonJavaEnumAst() : AnsonJavaEnumAst(_type_) {}

    AnsonJavaEnumAst(string anclass) : AnsonAst(anclass, anclass) {
        // isJavaEnum = true; Ast is not a JavaEnum
    }
};

class AnsonBodyAst : public AnsonAst {
 public:
    inline static const string _type_ = "io.odysz.anson.AnsonBodyAst";

    // ISSUE not adding uri to fields?
    string uri;
    map<string, string> A;

    AnsonBodyAst() : AnsonAst(_type_, _type_) { }

    AnsonBodyAst(string anclass) : AnsonAst(anclass, _type_) { }
};

class AnsonMsgAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonMsgAst";

    string bodyAnclass;
    string bodyAst;
    string portAnclass;
    string portAst;

    AnsonMsgAst() : AnsonAst() { }

    AnsonMsgAst(string anclass) : AnsonAst(anclass, _type_) { }
};

inline JavaEnum:: JavaEnum(string anclass, string e) : enm(std::move(e)), IJsonable(anclass) {
    if (contxt_ptr->asts->contains(anclass)) {
        map<string, string> encode = dynamic_cast<AnsonJavaEnumAst*>(contxt_ptr->asts->at(anclass).get())->encode;
        if (encode.contains(enm)) {
            enm = encode[enm];
            return;
        }
    }
    andebug("JavaEnum: "s + enm);
}

inline static entt::meta_data find_field_recursive(entt::meta_type type, id_type key) {
    // Check current type
    if (auto data = type.data(key); data) {
        return data;
    }

    // Check base classes (requires entt::meta<Derived>().base<Base>() registration)
    for (auto [id, base] : type.base()) {
        if (auto data = find_field_recursive(base, key); data) {
        return data;
        }
    }

    return {}; // Null handle
}

/**
 * @brief serialize_enum
 * @param instance any type as enum in cpp has no base class
 * @param type
 * @param ast
 * @param os
 * @return os
 */
inline static ostream& serialize_enum(const meta_any &instance, const meta_type &type,
              const AnsonAst &ast, std::ostream &os) {
    if (ast.isEnum) {
        if (auto value = instance.try_cast<int>(); value) {
            os << *value;
        } else if (auto u_value = instance.try_cast<unsigned int>(); u_value) {
            os << *u_value;
        } else {
            // Fallback: try to convert via the meta system to a type we can print
            // This works if the enum is registered as a meta type
            os << instance.cast<int>();
        }
    }
    return os;
}

inline static ostream& serialize_list(ostream& os, const meta_any &list_any, const vector<string> &val_ast_id) {
    if (val_ast_id[0] == "string") {
        vector<string> list = list_any.cast<vector<string>>();
        os << '[';
        bool first = true;
        for (string e : list) {
            if (first) first = false; else os << ',';
            os << '"' << e << '"';
        }
        os << ']';
    }
    else
        anerror("Todo: list of "s + val_ast_id[0] + ", ptr " + val_ast_id[1]);
    return os;
}

inline static ostream& serialize_map(ostream& os, const meta_any &val, const string &dataclass) {
    // const AnsonAst &fd_map_ast,
    if ("map<string, string" != dataclass)
        anerror("TODO: map of " + dataclass);

    if (auto view = val.as_associative_container(); view) {
        os << '{';
        bool first = true;
        for (auto [key, value] : view) {
            if (first) first = false;
            else os << ", ";

            // key and value are also entt::meta_any
            std::string k = key.cast<std::string>();
            os << '\"' << k << "\": ";

            // std::string v = value.cast<std::string>();
            // if (auto* str_ptr = value.try_cast<std::string>()) {
            if (value.type() == entt::resolve<std::string>()) {
                os << '\"' << value.cast<const std::string&>() << '\"';
            }
            // Maybe it's an int?
            else if (auto* int_ptr = value.try_cast<int>()) {
                os << *int_ptr;
            }
            // Fallback: Just print the type name
            else {
                os << "\"(unknown type): " << value.type().info().name() << '\"';
            }
        }
        os << '}';
    }
    return os;
}

inline static ostream& serialize_field(ostream &os, IJsonable& jsonable,
              const AnsonAst &fd_ast, const meta_type &fd_type, const JsonOpt &opts) {

    entt::meta_any val{entt::forward_as_any(jsonable)};

    if (fd_ast.isList) {
        anerror("There is not ast of list, and list should be handled in serialize_field, so why reached here?");
        // serialize_list(os, val, fd_ast, fd_type); // Not Reachable
    }
    else if (fd_ast.isMap)
        // serialize_map(os, val, fd_ast, fd_type, opts);
        serialize_map(os, val, fd_ast.dataAnclass);
    else if (fd_ast.isEnum)
        serialize_enum(val, fd_type, fd_ast, os);
    else if (fd_ast.isJavaEnum)
        os << val.try_cast<JavaEnum>();
    else if (fd_ast.istring) {
        auto s = val.try_cast<std::string>();
        os << '\"' << *s << '\"';
    }
    else if (fd_ast.isJsonable)
        jsonable.toBlock(os, opts);
    else if (fd_ast.isInt) {
        auto s = val.try_cast<int>();
        os << '\"' << s << '\"';
    }
    else if (fd_ast.isDouble) {
        auto s = val.try_cast<std::double_t>();
        os << '\"' << s << '\"';
    }
    else  os << R"("Cannot handle value of )" << fd_ast.anclass << '\"';
    return os;
}

inline static ostream& serialize_fields(ostream &os,
        const map<string, AnsonField> &fields, anson::Anson& anson, const JsonOpt &opts, bool first) {

     if (fields.size() > 0) {
        meta_any instance{anson};

        // bool first = true;
        for (auto[fn, f] : fields) {
            if (first)
                first = false;
            else os << ",";

            os << '\"' << fn << R"(": )";

            if (opts.asts->contains(f.dataAnclass)) {
                AnsonAst* fd_ast = opts.ast<AnsonAst>(f.dataAnclass);
                meta_type fd_type = resolve(hashed_string{f.dataAnclass.c_str()});

                serialize_field(os, anson, *fd_ast, fd_type, opts);
            }
            else if (f.dataAnclass.starts_with("list<")) {
                meta_type enttype = resolve(hashed_string{anson.anclass.c_str()});
                vector<string> valtype = Regex::parseListValtype(f.dataAnclass);
                hashed_string data_key{fn.c_str()};
                meta_data data = find_field_recursive(enttype, data_key);
                string fn = data.name();
                meta_any list = data.get(instance);

                if (list)
                    serialize_list(os, anson, valtype);
                else
                    os << "null";
            }
            else {
                os << '\"' << f << '\"';
            }
        }
    }
    return os;
}

inline static ostream& serialize_kvs(ostream &os, Anson& anson, const JsonOpt &opts, bool first=true) {
    // Java class can has only on base class
    AnsonAst *ast       = opts.ast<AnsonAst>(anson.anclass);
    // AnsonMsgAst *msgast = opts.ast<AnsonMsgAst>(anson.anclass);
    if (opts.asts->find(ast->dataBaseAst) != opts.asts->end()) {
        // auto base_fields = opts.asts->at(ast->dataBaseAst)->fields;
        AnsonAst *base_ast = opts.asts->at(ast->dataBaseAst).get();
        if (opts.has_ast(base_ast->dataAnclass)) {
            auto base_fields = opts.asts->at(base_ast->dataAnclass)->fields;
            serialize_fields(os, base_fields, anson, opts, first);

            first = base_fields.size() == 0;
        }
    }

    auto fields = opts.asts->at(anson.anclass)->fields;

    // if (fields.size() > 0) {
    //     meta_type enttype = opts.types->at(anson.anclass);
    //     entt::meta_any instance{entt::forward_as_any(anson)};

    //     bool first = true;
    //     for (auto[fn, f] : fields) {
    //     if (first) first = false;
    //     else os << ",";

    //     os << '\"' << fn << R"(": ")";

    //     AnsonAst fd_ast = opts.asts->at(f.astid);
    //     meta_type fd_type = opts.types->at(fn);

    //     serialize_field(os, anson, fd_ast, fd_type, opts);
    //     }
    // }
    // return os;
    return serialize_fields(os, fields, anson, opts, first);
}

inline static ostream& serialize_envelope(ostream &os, Anson& anson, const JsonOpt &opts) {
    os << R"({"type": ")" + anson.type + '"';
    serialize_kvs(os, anson, opts, false);
    return  os << '}';
}

template <typename ValT>
inline static string serialize_map_str(const map<string, ValT> &m, const string & map_type) {
    stringstream ss;
    serialize_map(ss, forward_as_meta(m), map_type);
    return ss.str();
}

class SemanticObject : public Anson {
public:
    inline static string _type_ = "io.odysz.semantics.SemanticObject";

    map<string, std::any> data;

    SemanticObject() : Anson(_type_) { }
};

struct ParseNode {
    meta_any instance;

    string val_astid;
    bool is_val_ptr = false;

    bool is_list = false;
    bool is_map = false;
    bool resolve_map2fields = false;

    // map<string, string> shadow_fields;
    // vector<string> shadow_list;

    id_type activekey = 0;
    string map_key;
};

template<typename T>
class EnTTSaxParser : public nlohmann::json_sax<nlohmann::json> {
private:
    const JsonOpt *contxt;

    id_type active_key{0};


    AnsonAst* find_field_ast(const AnsonAst *inst_ast, const std::string &fieldname) {
        if (inst_ast->fields.contains(fieldname))
            return contxt->ast<AnsonAst>(inst_ast->fields.at(fieldname).dataAnclass);
        else if (AnsonAst *base_ast = contxt->ast<AnsonAst>(inst_ast->dataBaseAst); base_ast)
            return find_field_ast(base_ast, fieldname);

        return nullptr;
    }

    template<typename V>
    void set_value(V&& val) {
        if (stack.empty()){
            anerror("set_value(): setting val to empty object");
            return;
        }

        auto& top = stack.back();

        if (top.is_list) {
            andebug(std::format("set_value(): setting string in list: {}", top.map_key));
            auto view = top.instance.as_sequence_container();
            if (view) {
                view.insert(view.end(), std::forward<V>(val));
                andebug("set_value(): List size: " + std::to_string(view.size()));
            }
            else
                anerror(std::format("set_value(): Failed to set list value: {}", val));
            // top.shadow_list.push_back(std::any_cast<string_t&>(val));
            return;
        }// not the branch of is_map?
        else if (active_key != 0) {
            auto& top = stack.back();

            if (contxt->primtypes.contains(top.val_astid)) {
                if (!top.is_map)
                    anerror("set_value(): Why here\n\n??????????\n\n");

                andebug("set_value(): set to supposed map");
                auto view = top.instance.as_associative_container();
                if (view) {
                    view.insert(top.map_key, std::forward<V>(val));
                }
                else
                    anerror("set_value(): Why cannot set value to map?");

                andebug("set_value(): Map size after insert: " + std::to_string(view.size()));
                return;
            }

            auto data = find_field_recursive(top.instance.type(), active_key);
            if (data) {
                std::string fieldname = data.name();
                andebug(std::format("set_value(): setting {}, active_key: {}",
                                fieldname, active_key));

                if (!contxt->asts->contains(top.val_astid)) {
                    anerror("set_value(): Cannot find AST "s + top.val_astid);
                    return;
                }

                AnsonAst* ast = contxt->ast<AnsonAst>(top.val_astid);
                AnsonAst *fd_ast = find_field_ast(ast, fieldname);

                if (fd_ast != nullptr) {
                    if (fd_ast->isJavaEnum) {
                        std::string string_val;

                        if constexpr (std::is_same_v<std::decay_t<V>, std::string>) {
                            string_val = std::forward<V>(val);
                        } else if constexpr (std::is_same_v<std::decay_t<V>, bool>) {
                            string_val = val ? "true" : "false";
                        } else {
                            // This handles ints, floats, etc., safely
                            string_val = std::to_string(val);
                        }
                        auto v = data.type().construct(string_val);


                        JavaEnum *je = v.template try_cast<JavaEnum>();
                        bool res = data.set(top.instance, v);
                        return;
                    }
                    if (fd_ast->isEnum) { // TODO
                        anerror("TODO: "s + fd_ast->dataAnclass);
                        return;
                    }
                }
                else {
                    bool res = data.set(top.instance, std::forward<V>(val));
                    if (!res)
                        anerror("set_value(): failed to set"s + fieldname);
                }
            }
            else {
                // Debug Notes: to avoid error: SEH exception, don't call top.instance.type().name()
                anerror(std::format(
                    "set_value(): Cannot find field by key-id: {}", active_key));
            }
        }
    }

public:
    std::deque<ParseNode> stack;

    /**
     * Parser for AST loading.
     * When parsing AST strings, the ast_id can be different to obj.anclass.
     *
     * @brief EnTTSaxParser
     * @param obj
     * @param ast_id
     * @param opts
     */
    EnTTSaxParser(T& obj, std::string ast_id, const JsonOpt *opts = nullptr) : contxt(opts) {
        push(obj, ast_id);
        contxt = opts == nullptr ? IJsonable::contxt_ptr : opts;
        active_key = 0;
    }

    EnTTSaxParser(T& obj, const JsonOpt *opts = nullptr) : EnTTSaxParser(obj, obj.anclass, opts) {}

    bool start_object(std::size_t size) override {
        bool k0 = active_key != 0;
        bool es = !stack.empty();

        if (active_key != 0 && !stack.empty()) {
            ParseNode top = stack.back();
            meta_type type = top.instance.type();
            auto datafield = find_field_recursive(type, active_key);
            if (datafield) {
                std::string fieldname = datafield.name();
                andebug("start_obj(): Starting object, field name: "s + fieldname);

                if (contxt->asts->contains(top.val_astid)) {
                    AnsonAst *ast = contxt->ast<AnsonAst>(top.val_astid);
                    std::string fd_astid;
                    // Notes 26 Mar 2026:
                    // Fields is the definition of an AST, and must be merged back to a being loading AST.
                    // The loading of AST according to AST stop the recursive traversal here.
                    bool resolving_map2Fields = false;
                    if ("fields" == fieldname && true) { // How do I know I am loading an AST of an AST?
                        fd_astid = "map<string,"s + AnsonField_type;
                        resolving_map2Fields = true;
                    }

                    else if (!ast->fields.contains(fieldname))
                        anerror(std::format(
                            "start_obj(): AST {{anclass: {}, datatype: {}}} has no field {}.",
                            ast->anclass, ast->dataAnclass, fieldname));

                    else
                        fd_astid = ast->fields.at(fieldname).dataAnclass;

                    if (contxt->asts->contains(fd_astid))
                        stack.push_back({.instance = datafield.get(stack.back()),
                                     .val_astid=fd_astid});
                    else { // e.g. map<string, string
                        meta_any inst = datafield.get(stack.back().instance);
                        push_map(inst, active_key, fd_astid, resolving_map2Fields);
                    }
                }
            } else if (top.is_map) {
                // e.g. val_type = AnsonField in a Map, the top.instance. String in map won't reach here, starting obj.
                std::string fieldname = top.map_key;
                andebug(std::format("start_obj(): Starting object in map, field key: {}", active_key));

                std::string fd_astid;
                if (contxt->asts->contains(top.val_astid)) {
                    AnsonAst *ast = contxt->ast<AnsonAst>(top.val_astid);
                    fd_astid = top.val_astid;

                    meta_type type = resolve(hashed_string{fd_astid.c_str()});
                    if (type) {
                        meta_any inst = type.construct();
                        if (contxt->asts->contains(fd_astid))
                            stack.push_back({.instance = inst, .val_astid=fd_astid, .activekey=active_key});
                        else
                            push_map(inst, active_key, fd_astid, false);
                    }
                    else {
                        anerror("start_obj(): Primative types can not be here?");
                        anerror("start_obj(): Cannot start an object in a map with ast id:"s + fd_astid);
                    }
                }
            } else if (top.is_list) {
                // top.map_key should be empty;
                andebug(std::format("start_obj(): Starting object in list, field key: {}", active_key));

                if (contxt->asts->contains(top.val_astid)) {
                    AnsonAst *ast = contxt->ast<AnsonAst>(top.val_astid);

                    std::string fd_astid;
                    fd_astid = top.val_astid;
                    meta_type type = resolve(hashed_string{fd_astid.c_str()});
                    if (type) {
                        meta_any inst = type.construct();
                        if (contxt->asts->contains(fd_astid))
                            stack.push_back({.instance = inst, .val_astid=fd_astid, .activekey=active_key});
                        else
                            push_map(inst, active_key, fd_astid, false);
                    }
                    else {
                        anerror("start_obj(): Primative types can not be here?");
                        anerror("start_obj(): Cannot start an object in a list with ast id:"s + fd_astid);
                    }
                }
                else
                    anerror("start_obj(): Cannot find ast "s + top.val_astid);

            } else {
                anerror(std::format(
                    "start_obj(): Starting object, cannot find object field, key: {}, type: {}, top.map_key: {}",
                    std::to_string(active_key), top.val_astid, top.map_key));

                anerror(type.info().name());
                return true;
            }
        } else if (stack.empty()) {
            // This is the root object.
            anerror("start_obj(): Starting object with empty stack.");
            return true;
        }
        return true;
    }

    bool key(string_t& val) override {
        active_key = hashed_string{val.c_str()};
        andebug(std::format("key(): deserializing key {}, key-id: {}", val, active_key));

        if (!stack.empty()) {
            stack.back().map_key = val;
        }

        return true;
    }

    bool end_object() override {
        if (stack.size() > 1) {
            ParseNode top = stack.back();
            id_type key0 = top.activekey;
            stack.pop_back();

            if (!stack.empty() && key0 != 0) {
                auto data = find_field_recursive(stack.back().instance.type(), key0);
                if (data) {
                    if (!stack.back().resolve_map2fields)
                        data.set(stack.back().instance, top.instance);
                    else
                        anerror("end_obj(): Upto 349fef9620674c8b65857616ddddb6d5dc516e7c : Not reachable, and is anti-intution to recursive map parsing (starting shadow_map).");
                }
                else if (stack.back().is_map) {
                    meta_type val_type = resolve(hashed_string{stack.back().val_astid.c_str()});
                    meta_type obj_type = top.instance.type();
                    andebug(std::format("end_obj(): fd_type.id() {}, map_type.id() {}", val_type.id(), obj_type.id()));
                    if (val_type.id() == obj_type.id()) {
                        auto view = stack.back().instance.as_associative_container();
                        if (view) {
                            entt::meta_any key{std::move(stack.back().map_key)};

                            bool success = view.insert(std::move(key), top.instance);

                            if (!success) {
                                anerror("end_obj(): Handle failure (type mismatch or key already exists");
                            }
                        }
                    }
                }
                else if (stack.back().is_list) {
                    meta_type obj_type = top.instance.type();
                    std::string typname{obj_type.info().name()};
                    andebug(std::format("end_obj(): list-container.type-id() {}, info: {}", obj_type.id(), typname));
                        auto view = stack.back().instance.as_sequence_container();
                        if (view) {

                            meta_any success;
                            if (stack.back().is_val_ptr) {
                                auto func = obj_type.func("create_ptr"_hs);

                                if (!func) {
                                    anerror(std::format("end_obj(): create_ptr({}) is not registered.", typname));
                                    return false;
                                }

                                andebug(std::format("end_obj(): Stack back instance's func<create_ptr> arity: {}",
                                                                func.arity()));

                                success = obj_type.func("create_ptr"_hs).invoke(top.instance);
                                if (success)
                                    success = view.insert(view.end(), success);
                            }
                            else
                                success = view.insert(view.end(), top.instance);

                            if (!success)
                                anerror("end_obj(): Setting back the list is failed!");
                            else
                                andebug(std::format("end_obj(): Ok! Size of the inserted list of obj: {}",
                                                                view.size()));
                        }
                }
            }
            active_key = key0;
        }
        // else: TODO needing a stack resetting for re-enter.
        return true;
    }

    // 2. Data Type Handling
    bool number_float(number_float_t val, const string_t&) override {
        set_value(static_cast<float>(val));
        return true;
    }

    bool number_integer(number_integer_t val) override {
        set_value(static_cast<int>(val));
        return true;
    }

    bool string(string_t& val) override {
        andebug(std::format("string(): {}, top.is_list: {}, top.is_map: {}, map_key: {}",
                        val, stack.back().is_list, stack.back().is_map, stack.back().map_key));

        set_value(val);
        return true;
    }

    bool boolean(bool val) override {
        set_value(val);
        return true;
    }

    bool null() override { return true; }

    bool number_unsigned(number_unsigned_t val) override {
        set_value(static_cast<uint64_t>(val));
        return true;
    }

    bool binary(binary_t&) override { return true; }
    bool start_array(std::size_t) override {

        andebug(std::format("start_array(): [0] list container addr: {:P}",
                (void*)stack.front().instance.try_cast<Anson>()));

        if (active_key != 0 && !stack.empty()) {
            auto data = find_field_recursive(stack.back().instance.type(), active_key);
            if (data) {
                std::string fieldname = data.name();
                andebug(std::format("start_array(): starting, field key {}, name {}",
                                                active_key, fieldname));

                std::string val_astid;
                AnsonAst *ast = contxt->ast<AnsonAst>(stack.back().val_astid);
                if (!ast->fields.contains(fieldname))
                    anerror(std::format(
                        "start_array(): AST {{anclass: {}, datatype: {}}} has no field {}.",
                        ast->anclass, ast->dataAnclass, fieldname));
                else
                    val_astid = ast->fields.at(fieldname).dataAnclass;

                meta_any list = data.get(stack.back().instance);
                push_list(list, active_key, val_astid);

                andebug(std::format("start_array(): [1] list container addr: {:P}",
                        (void*)stack.front().instance.try_cast<Anson>()));
            }
        }
        return true;
    }

    bool end_array() override {

        Anson* stack_ptr = stack.front().instance.try_cast<Anson>();
        andebug(std::format("end_array(): Stack back addr: {:P}", (void*)stack_ptr));

        if (!stack.empty() && stack.back().is_list) {
            auto finished_list = stack.back().instance;
            id_type key_to_update = stack.back().activekey;
            std::string field_to_update = stack.back().map_key;
            id_type field_id = hashed_string(field_to_update.c_str());

            stack.pop_back();

            if (!stack.empty() && key_to_update != 0) {
                auto data = find_field_recursive(stack.back().instance.type(), key_to_update);
                if (data) {
                    andebug(std::format(
                        "end_array(): Setting back list {} : {}, size: {:#x} -> {}",
                        data.name(), key_to_update,
                        finished_list.as_sequence_container() ? finished_list.as_sequence_container().size() : -1,
                        data.get(stack.back().instance).as_sequence_container().size()));

                    bool res = data.set(stack.back().instance, finished_list);

                    Anson* stack_ptr = stack.front().instance.try_cast<Anson>();
                    andebug(std::format("end_array(): Stack back addr: {:P}", (void*)stack_ptr));

                    if (res) {
                        andebug(std::format("end_array(): The reference of this list is found. size: {}, field: {}, key-id: {}",
                                                        data.get(stack.back().instance).as_sequence_container().size(),
                                                        data.name(), key_to_update));
                    }
                    else
                        anerror("end_array(): Failed to set back (copy) "s + data.name());

                    if (Anson* v = stack.back().instance.try_cast<anson::Anson>())
                        andebug(v->toBlock(*IJsonable::contxt_ptr));
                }
            }
        }
        return true;
    }


    /////////////////////////////////////////////////////////////////
    bool parse_error(std::size_t, const std::string&, const nlohmann::detail::exception&) override { return false; }

    void push(T &obj, std::string astid) {
        stack.push_back({.instance = forward_as_meta(obj), .val_astid=astid, .is_list=false, .is_map=false, .activekey=0});
    }

    void push_list(meta_any ref, id_type active_key, std::string list_type) {
        vector<std::string> val_anclass = Regex::parseListValtype(list_type);
        andebug(std::format("List value data class: {}, ptr {}", val_anclass[0], val_anclass[1]));

        bool is_ptr = val_anclass[1] == "true";

        stack.push_back({.instance = ref,
                         .val_astid=val_anclass[0], .is_val_ptr=is_ptr,
                         .is_list=true, .is_map=false,
                         .activekey=active_key});
    }

    void push_map(meta_any &map_inst, const id_type active_key, const std::string & map_type, bool resolve_map2fields) {

        vector<std::string> val_anclass = Regex::parseMapValtype(map_type);
        andebug(std::format("Map value data class: {}, ptr {}", val_anclass[0], val_anclass[1]));

        stack.push_back({.instance = map_inst,
                         .val_astid=val_anclass[0], .is_val_ptr=(val_anclass[1] == "true"),
                         .is_list=false, .is_map=true,
                         .resolve_map2fields=resolve_map2fields,
                         .activekey=active_key});
    }
};

template<typename T>
inline static bool parse(const string& json, T &an, const JsonOpt *opts) {
    EnTTSaxParser handler{an, opts};
    return nlohmann::json::sax_parse(json, &handler);
}
}

