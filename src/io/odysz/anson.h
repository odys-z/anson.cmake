/** 9bbe3a552ef8b154454af904eeaa1f1af8f60fc3 */
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

class JsonOpt {
public:
    bool escape4DB;
    string doubleFormat;
    string indent;

    const map<string, string> primtypes;
    const map<string, AnsonAst> *asts;
    const map<string, meta_type> *enttypes;
    JsonOpt(const map<string, AnsonAst> *asts, const map<string, meta_type> *types)
        : asts(asts), enttypes(types), primtypes({
            {"String", "string"}, {"string", "string"}, {"java.lang.String", "string"},
            {"int", "int"}, {"Integer", "int"}, {"java.lang.Integer", "int"},
            {"short", "int"}, {"Short", "int"}, {"java.lang.Short", "int"},
            {"long", "long"}, {"Long", "long"}, {"java.lang.Long", "long"},
            {"float", "float"}, {"Float", "float"}, {"java.lang.Float", "float"},
            {"double", "double"}, {"Double", "double"}, {"java.lang.Double", "double"},
            {"boolean", "boolean"}, {"Boolean", "boolean"}, {"java.lang.Boolean", "boolean"},
        }) { }
};

class IJsonable {

public:
    // inline static const map<string, hashed_string> enttypes;
    // inline static const map<string, AnsonAst> asts;
    // static JsonOpt &contxt;
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

    // std::function<string(JavaEnum)> decoder;
    string enm;

    // template<typename SubClass>
    JavaEnum(string dataAnclass, string e);

    JavaEnum& operator=(const JavaEnum&) = default;
    JavaEnum& operator=(JavaEnum&&) noexcept = default;

    static string url(string enm) {
        return enm; // decode[enm];
    }

    static JavaEnum& valof(string s, JavaEnum& stub) {
        return stub;
    }

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
// ostream& serialize_envelope(ostream &os, Anson* anson, const JsonOpt &opts);
bool parse(const string& json, IJsonable& an, const JsonOpt &opts);

/**
 * @brief The Anson class
 * java type: io.odysz.anson.Anson
 */
class Anson : public IJsonable {
public:
    inline static const string _type_ = "io.odysz.anson.Anson";

    string type;

    Anson() : IJsonable(_type_), type(_type_) {
        aninfo(string_view("defalut contructor"));
    }

    Anson(string t) : IJsonable(t), type(t) {
        andebug(string_view("override constructor, type = " + t));
    }

    /**
     * @brief Anson
     * @param t
     * @param astid can be different if is a template specialized.
     */
    Anson(string t, string anclass) : IJsonable(anclass), type(t) {
        andebug(string_view("override constructor, anclass = " + anclass));
    }

    template <typename T>
    static bool from_json(const string& json, IJsonable& an, const JsonOpt &opts) {
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

    string dataBase;
    /** The target's anson type or data type, e.g. string */
    string dataAnclass;
    AnsonAst& data_anclass(const string & cls) { dataAnclass = cls; return *this; }

    map<string, AnsonField> fields;
    map<string, int> enums;

    /** Only one static string value is allowed in semantic-* ? */
    string static_val;

    // vector<vector<string>> ctors;
    // map<string, string> encode;
    // map<string, string> decode;

    // meta_type meta_type;
    hashed_string enttypeid = hashed_string{_type_.c_str()};

    AnsonAst() : isEnum(false), Anson(_type_) { }

    AnsonAst(string anclass, bool isEnum = false) : isEnum(isEnum), Anson(_type_, anclass) { }

    AnsonAst(string anclass, string type) : isEnum(false), Anson(type, anclass) { }
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
        map<string, string> encode = any_cast<AnsonJavaEnumAst>(contxt_ptr->asts->at(anclass)).encode;
        if (encode.contains(e)) {
            enm = encode[e];
            return;
        }
    }
    enm = e;
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

inline static ostream& serialize_list(ostream& os, const meta_any &list_any, const string &val_ast_id) {
    if (val_ast_id == "string") {
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
        anerror(string_view("Todo: list of "s + val_ast_id));
    return os;
}

inline static ostream& serialize_map(ostream& os, const meta_any &val, const string &dataclass) {
    // const AnsonAst &fd_map_ast,
    if ("map<string, string" != dataclass)
        anerror(string_view("TODO: map of " + dataclass));

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
        anerror(string_view("There is not ast of list, and list should be handled in serialize_field, so why reached here?"));
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
        // serialize_jsonable(os, val, fd_ast, fd_type, opts);
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
        const map<string, AnsonField> &fields, anson::Anson& anson, const JsonOpt &opts) {

     if (fields.size() > 0) {
        // meta_type enttype = opts.enttypes->at(anson.anclass);
        // meta_any instance = entt::forward_as_meta(anson);
        meta_any instance{anson};

        bool first = true;
        for (auto[fn, f] : fields) {
            if (first)
                first = false;
            else os << ",";

            os << '\"' << fn << R"(": )";

            if (opts.asts->contains(f.dataAnclass)) {
                AnsonAst fd_ast = opts.asts->at(f.dataAnclass);
                meta_type fd_type = opts.enttypes->at(fn);

                serialize_field(os, anson, fd_ast, fd_type, opts);
            }
            else if (f.dataAnclass.starts_with("list<")) {
                if (!opts.enttypes->contains(anson.anclass)) {
                    anerror(string_view("Unknown entt type to serialize: "s + anson.anclass));
                    os << '"' << anson.anclass << '"';
                }
                else {
                    meta_type enttype = opts.enttypes->at(anson.anclass);
                    string valtype = Regex::parseListValtype(f.dataAnclass);
                    hashed_string data_key{fn.c_str()};
                    meta_data data = find_field_recursive(enttype, data_key);
                    string fn = data.name();
                    meta_any list = data.get(instance);
                    if (list)
                        serialize_list(os, anson, valtype);
                    else
                        os << "null";
                }
            }
            else {
                os << '\"' << f << '\"';
            }
        }
    }
    return os;
}

inline static ostream& serialize_kvs(ostream &os, Anson& anson, const JsonOpt &opts) {

    // MARK break point for observe the deserialized (if end_object() call toBlock()
    // Java class can has only on base class
    AnsonAst ast = opts.asts->at(anson.anclass);
    if (opts.asts->find(ast.base) != opts.asts->end()) {
        auto base_fields = opts.asts->at(ast.base).fields;
        serialize_fields(os, base_fields, anson, opts);
    }

    auto fields = opts.asts->at(anson.anclass).fields;

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
    return serialize_fields(os, fields, anson, opts);
}

inline static ostream& serialize_envelope(ostream &os, Anson& anson, const JsonOpt &opts) {
    os << R"({"type": ")" + anson.type + '"';
    serialize_kvs(os, anson, opts);
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
    map<string, std::any> data;
};

struct ParseNode {
    meta_any instance;
    string astid;
    bool is_list = false;
    bool is_map = false;
    bool resolve_map2fields = false;
    map<string, string> shadow_fields;
    vector<string> shadow_list;
    id_type activekey = 0;
    string map_key;
};

template<typename T>
class EnTTSaxParser : public nlohmann::json_sax<nlohmann::json> {
private:
    const JsonOpt *contxt;

    // std::vector<ParseNode> stack;

    id_type active_key{0};

    // Helper to set values on the current object in the stack
    template<typename V>
    void set_value(V&& val) {
        if (stack.empty()){
            anerror("setting val to empty object");
            return;
        }

        auto& top = stack.back();

        if (top.is_list) {
            andebug(string_view(std::format("setting string in list: {}", top.map_key)));
            auto view = forward_as_meta(top.shadow_list).as_sequence_container();
            if (view) {
                view.insert(view.end(), std::forward<V>(val));
                andebug(string_view("List size: " + std::to_string(view.size())));
            }
            // top.shadow_list.push_back(std::any_cast<string_t&>(val));
            return;
        }// not the branch of is_map?
        else if (active_key != 0) {
            auto& top = stack.back();

            if (contxt->primtypes.contains(top.astid)) {
                if (!top.is_map)
                    anerror(string_view("Why here\n\n??????????\n\n"));

                andebug(string_view("set_value(): set to supposed map"));
                auto view = top.instance.as_associative_container();
                if (view) {
                    view.insert(top.map_key, std::forward<V>(val));
                }
                else
                    anerror(string_view("Why cannot set value to map?"));

                andebug(string_view("Map size after insert: " + std::to_string(view.size())));
                return;
            }

            auto data = find_field_recursive(top.instance.type(), active_key);
            if (data) {
                andebug(string_view(std::format("set_value(): setting {}, active_key: {}",
                                data.name(), active_key)));

                if (!contxt->asts->contains(top.astid)) {
                    anerror(string_view("set_value(): Cannot find AST "s + top.astid));
                    return;
                }

                AnsonAst ast = contxt->asts->at(top.astid);
                if (ast.isJavaEnum) {
                    // TODO test with a Port instance, not AST
                    auto v = data.type().construct(val);
                    data.set(top.instance, v);
                }
                else {
                    bool res = data.set(top.instance, std::forward<V>(val));
                    if (!res)
                        anerror(string_view("failed to set"s + data.name()));

                    // auto* ptr = stack.front().instance.try_cast<anson::PeerSettings>();
                    // andebug(string_view(std::format("Modifying object at address: {}", (void*)ptr)));
                }
            }
            else {
                // Debug Notes: to avoid error: SEH exception, don't call top.instance.type().name()
                anerror(string_view(std::format(
                    "Cannot find field by key-id: {}", active_key)));
            }
        }
    }

public:
    std::vector<ParseNode> stack;

    EnTTSaxParser(T& obj, const JsonOpt *opts) : contxt(opts) {
        push(obj);
        active_key = 0;
    }

    bool start_object(std::size_t size) override {
        bool k0 = active_key != 0;
        bool es = !stack.empty();

        if (active_key != 0 && !stack.empty()) {
            ParseNode top = stack.back();
            meta_type type = top.instance.type();
            auto data = find_field_recursive(type, active_key);
            if (data) {
                std::string fieldname = data.name();
                andebug(string_view("Starting object, field name: "s + fieldname));

                if (contxt->asts->contains(top.astid)) {
                    // jsonable
                    if (!contxt->asts->contains(top.astid))
                        anerror(string_view("Context has no ast " + top.astid));
                    AnsonAst ast = contxt->asts->at(top.astid);

                    std::string fd_astid;
                    // Notes 26 Mar 2026:
                    // Fields is the definition of an AST, and must be merged back to a being loading AST.
                    // The loading of AST according to AST stop the recursive traversal here.
                    bool resolving_map2Fields = false;
                    if ("fields" == fieldname && true) { // How do I know I am loading an AST of an AST?
                        // fd_astid = "map<string, " + AnsonField_type;
                        fd_astid = "map<string, map<string, string";
                        resolving_map2Fields = true;
                    }

                    else if (!ast.fields.contains(fieldname))
                        // AST {anclass: io.odysz.anson.AnsonBodyAst, datatype: io.odysz.anson.AnsonBodyAst} has no field fields.
                        anerror(string_view(std::format(
                            "AST {{anclass: {}, datatype: {}}} has no field {}.",
                            ast.anclass, ast.dataAnclass, fieldname)));

                    else
                        fd_astid = ast.fields.at(fieldname).dataAnclass;

                    if (contxt->asts->contains(fd_astid))
                        stack.push_back({.instance = data.get(stack.back()),
                                     .astid=fd_astid});
                    else { // e.g. map<string, string
                        meta_any inst = data.get(stack.back().instance);
                        push_map(inst, active_key, fd_astid, resolving_map2Fields);
                    }
                }

            } else {
                anerror(string_view(std::format("Starting object, cannot find object field, key: {}",
                                                std::to_string(active_key))));
                anerror(type.info().name());
                return true;
            }
        } else if (stack.empty()) {
            // This is the root object.
            anerror("Starting object with empty stack.");
            return true;
        }
        return true;
    }

    bool key(string_t& val) override {
        active_key = hashed_string{val.c_str()};
        andebug(string_view(std::format("deserializing key {}, key-id: {}", val, active_key)));

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
                    else {
                        // top.fields is a map<string, map<string, string
                        meta_type anfieldtype = entt::resolve<AnsonField>();
                        map<std::string, AnsonField> anfields;
                        for (auto [fn, f] : top.shadow_fields) {
                            map<std::string, std::string> fieldprops = any_cast<map<std::string, std::string>>(f);
                            AnsonField field{};
                            for (auto [k, v] : fieldprops) {
                                id_type p = hashed_string{k.c_str()};
                                auto fd = anfieldtype.data(p);
                                fd.set(field, v);
                            }
                            anfields[fn] = field;
                        }
                        data.set(stack.back().instance, anfields);
                    }
                }
            }
            active_key = key0;
        }
        // else: needing a resetting for re-enter.
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
        andebug(string_view(
            std::format("string: {}, top.is_list: {}, top.is_map: {}, map_key: {}",
                        val, stack.back().is_list, stack.back().is_map, stack.back().map_key)));

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

        Anson* stack_ptr = stack.front().instance.try_cast<Anson>();
        andebug(string_view(std::format("start0 addr: {:P}", (void*)stack_ptr)));

        if (active_key != 0 && !stack.empty()) {
            // auto data = stack.back().instance.type().data(active_key);
            auto data = find_field_recursive(stack.back().instance.type(), active_key);
            if (data) {
                andebug(string_view(std::format("Starting array, field key {}, name {}", active_key, data.name())));
                // auto sequence_instance = data.get(stack.back().instance);
                // push_list(sequence_instance.as_ref(), active_key);

                push_list(stack.back().instance, active_key);

                stack_ptr = stack.front().instance.try_cast<Anson>();
                andebug(string_view(std::format("start1 addr: {:P}", (void*)stack_ptr)));
                // active_key = 0;
            }
        }
        stack_ptr = stack.front().instance.try_cast<Anson>();
        andebug(string_view(std::format("start2 addr: {:P}", (void*)stack_ptr)));

        return true;
    }

    bool end_array() override {

        Anson* stack_ptr = stack.front().instance.try_cast<Anson>();
        andebug(string_view(std::format("Stack back addr: {:P}", (void*)stack_ptr)));

        if (!stack.empty() && stack.back().is_list) {
            auto finished_list = stack.back().shadow_list;
            id_type key_to_update = stack.back().activekey;
            std::string field_to_update = stack.back().map_key;
            id_type field_id = hashed_string(field_to_update.c_str());

            stack.pop_back();

            if (!stack.empty() && key_to_update != 0) {
                auto data = find_field_recursive(stack.back().instance.type(), key_to_update);
                if (data) {
                    andebug(string_view(std::format("Setting back list {} : {}, size: {}", data.name(), key_to_update, finished_list.size())));
                    bool res = data.set(stack.back().instance, finished_list);

                    Anson* stack_ptr = stack.front().instance.try_cast<Anson>();
                    andebug(string_view(std::format("Stack back addr: {:P}", (void*)stack_ptr)));

                    // if (vector<std::string>* vec = finished_list.try_cast<std::vector<std::string>>()) {
                    //     andebug(string_view(std::format("end_array(): Cast success. List size: {}", vec->size())));
                    //     if (vec->size() > 0) andebug(string_view(vec->at(0)));
                    // }
                    // else
                    //     anwarn(string_view("end_array(): finished_list is NOT a vector<string>. Check registration."));
                    // bool res = data.set(stack.back().instance, finished_list);

                    if (res) {
                        andebug(string_view(std::format("The reference of this list is found. Copied? field: {}, key-id: {}",
                                                        data.name(), key_to_update)));
                    }
                    else
                        anerror(string_view("Failed to set back (copy) "s + data.name()));

                    if (Anson* v = stack.back().instance.try_cast<anson::Anson>())
                        andebug(string_view(v->toBlock(*IJsonable::contxt_ptr)));
                }
            }
            // active_key = key_to_update;  // redundant?
        }
        return true;
    }


    /////////////////////////////////////////////////////////////////
    bool parse_error(std::size_t, const std::string&, const nlohmann::detail::exception&) override { return false; }

    void push(T &obj) {
        stack.push_back({.instance = forward_as_meta(obj), .astid=obj.anclass, .is_list=false, .is_map=false, .activekey=0});
    }

    void push_list(meta_any ref, id_type active_key) {
        stack.push_back({.instance = ref, .astid="string", .is_list=true, .is_map=false, .activekey=active_key});
    }

    void push_map(meta_any &map_inst, const id_type active_key, const std::string & map_type, bool resolve_map2fields) {

        std::string val_anclass = Regex::parseMapValtype(map_type);
        andebug(string_view("Map value data class: "s + val_anclass));

        stack.push_back({.instance = map_inst, .astid=val_anclass,
                         .is_list=false, .is_map=true,
                         .resolve_map2fields=resolve_map2fields,
                         .activekey=active_key});
    }
};

inline static bool parse(const string& json, IJsonable & an, const JsonOpt * opts) {
    EnTTSaxParser handler{an, opts};
    return nlohmann::json::sax_parse(json, &handler);
}
}

