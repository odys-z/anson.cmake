/** 9bbe3a552ef8b154454af904eeaa1f1af8f60fc3 */
#pragma once


#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>

#include "common.h"

using namespace std ;
using namespace entt;

namespace anson {


class AnsonAst;
class AnstField;

class JsonOpt {
public:
    bool escape4DB;
    string doubleFormat;
    string indent;


    const map<string, AnsonAst> *asts;
    const map<string, meta_type> *types;
    JsonOpt(const map<string, AnsonAst> *asts, const map<string, meta_type> *types) : asts(asts), types(types) { }
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

    JavaEnum(string antype, string e);

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

    /** The target's anson type or data type, e.g. string */
    string antype;
    AnsonAst& anstype(const string & t) { antype = t; return *this; }

    map<string, AnstField> fields;
    map<string, int> enums;

    // vector<vector<string>> ctors;
    // map<string, string> encode;
    // map<string, string> decode;

    // meta_type meta_type;
    hashed_string enttypeid = hashed_string{_type_.c_str()};

    AnsonAst() : isEnum(false), Anson(_type_) { }

    AnsonAst(string anclass, bool isEnum = false) : isEnum(isEnum), Anson(_type_, anclass) { }

    AnsonAst(string anclass, string type) : isEnum(false), Anson(type, anclass) { }
};

class AnsonJavaEnumAst: public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonJavaEnumAst";

    map<string, string> encode;
    map<string, string> decode;

    AnsonJavaEnumAst() : AnsonJavaEnumAst(_type_) {}

    AnsonJavaEnumAst(string anclass) : AnsonAst(anclass, anclass) {
        isJavaEnum = true;
    }
};

class AnsonBodyAst : public AnsonAst {
 public:
    inline static const string _type_ = "io.odysz.anson.AnsonBodyAst";
    map<string, string> A;

    AnsonBodyAst() : AnsonAst(_type_, _type_) { }

    AnsonBodyAst(string anclass) : AnsonAst(anclass, _type_) { }
};

class AnsonMsgAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonMsgAst";

    // map<string, string> A;

    string bodyType;
    string portType;

    /**
     * AnsonBody's Ast.
     * E.g.
     * body.io.odysz.semantic.jprotocol.AnsonBody
     * body.antype = "io.odysz.sematnic.jserv.echo.EchoReq"
    AnsonAst body;
     */

    /** E.g. io.odysz.semantic.jprotocol.AnsonMsg */
    string antype;

    AnsonMsgAst() : AnsonAst() { }

    AnsonMsgAst(string anclass) : AnsonAst(anclass, _type_) { }
};

class AnstField : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnstField";

    // string datatype; // i.e. antype
    string fieldname;
    // meta_type enttype;
    int length;

    /**
     * @brief static_val
     * MsgCode.ok = 0, ...
     */
    string staticVal;
    AnstField & static_val(string v) {
        staticVal = std::move(v);
        return *this;
    }

    //////////////////////// v 0.2.0
    /** "" for string, int, double or null */
    string astid;
    // AnstField& ast_id(hashed_string &id) {
    //     astid = std::move(id);
    //     return *this;
    // }

    /** ""_hs for string, int, double or null */
    hashed_string enttypeid;
    AnstField& enttype_id(hashed_string &id) {
        enttypeid = std::move(id);
        return *this;
    }

    AnstField(string fieldname) : AnsonAst(_type_), fieldname(fieldname) {}
    AnstField(string fieldname, string type) : AnsonAst(_type_, type), fieldname(fieldname) {}

    bool operator==(const AnstField& other) const {
        return fieldname == other.fieldname && type == other.type
            // && datatype == other.datatype && length == other.length
            && staticVal == other.staticVal && antype == other.antype
            && anclass == other.anclass && isList == other.isList && isMap == other.isMap
            && isEnum == other.isEnum && isJavaEnum == other.isJavaEnum;
    }
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

inline static ostream& serialize_list(ostream& os, const meta_any &val, const AnsonAst &fd_ast,
                                      const meta_type &fd_type) {
    return os;
}

inline static ostream& serialize_map(ostream& os, const meta_any &val, const string &antype) {
    // const AnsonAst &fd_map_ast,
    if ("map<string, string" != antype)
        anerror(string_view("TODO: map of " + antype));

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

    if (fd_ast.isList)
        serialize_list(os, val, fd_ast, fd_type);
    else if (fd_ast.isMap)
        // serialize_map(os, val, fd_ast, fd_type, opts);
        serialize_map(os, val, fd_ast.antype);
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

inline static ostream& serialize_fields(ostream &os, auto fields, IJsonable& anson, const JsonOpt &opts) {
     if (fields.size() > 0) {
        meta_type enttype = opts.types->at(anson.anclass);
        entt::meta_any instance{entt::forward_as_any(anson)};

        bool first = true;
        for (auto[fn, f] : fields) {
        if (first) first = false;
        else os << ",";

        os << '\"' << fn << R"(": ")";

        AnsonAst fd_ast = opts.asts->at(f.astid);
        meta_type fd_type = opts.types->at(fn);

        serialize_field(os, anson, fd_ast, fd_type, opts);
        }
    }
    return os;
}

inline static ostream& serialize_kvs(ostream &os, IJsonable& anson, const JsonOpt &opts) {

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
    os << "type: " + anson.type;

    serialize_kvs(os, anson, opts);
    // auto fields = opts.asts->at(anson->anclass).fields;

    // if (fields.size() > 0) {
    //     meta_type enttype = opts.types->at(anson->anclass);
    //     entt::meta_any instance{entt::forward_as_any(anson)};

    //     bool first = true;
    //     for (auto[fn, f] : fields) {
    //     if (first) first = false;
    //     else os << ",";

    //     os << '\"' << fn << R"(": ")";

    //     AnsonAst fd_ast = opts.asts->at(f.astid);
    //     meta_type fd_type = opts.types->at(fn);

    //     auto data = enttype.data(hashed_string(fn.c_str()));
    //     meta_any val = data.get(instance);

    //     serialize_instance(os, val, fd_ast, fd_type, opts);
    //     }
    // }
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
    bool is_list;
    bool is_map;
    id_type activekey;
};

template<typename T>
class EnTTSaxParser : public nlohmann::json_sax<nlohmann::json> {
private:
    const JsonOpt *contxt;

    std::vector<ParseNode> stack;

    id_type active_key{0};

    entt::meta_data find_field_recursive(entt::meta_type type, id_type key) {
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

    // Helper to set values on the current object in the stack
    template<typename V>
    void set_value(V&& val) {
        if (stack.empty()){
            anerror("setting val to empty object");
            return;
        }

        auto& top = stack.back();

        if (top.is_list) {
            auto view = top.instance.as_sequence_container();
            if (view) {
                view.insert(view.end(), std::forward<V>(val));
            }
        }
        // else if (!stack.empty() && active_key != 0) {
        else if (active_key != 0) {
            // auto data = stack.back().instance.type().data(active_key);
            auto data = find_field_recursive(stack.back().instance.type(), active_key);

            if (!contxt->asts->contains(stack.back().astid)) {
                anerror(string_view("Cannot find AST "s + stack.back().astid));
                return;
            }
            AnsonAst ast = contxt->asts->at(stack.back().astid);

            if (data) {
                aninfo(string_view("set_value(): setting "s + data.name()));
                if (ast.isJavaEnum) {
                    auto v = data.type().construct(val);
                    data.set(stack.back().instance, v);
                }
                else
                    data.set(stack.back().instance, std::forward<V>(val));
            }
        }
    }

public:
    EnTTSaxParser(T& obj, const JsonOpt *opts) : contxt(opts) {
        push(obj);
    }

    bool start_object(std::size_t size) override {
        if (active_key != 0 && !stack.empty()) {
            meta_type type = stack.back().instance.type();
            // auto data = type.data(active_key);
            auto data = find_field_recursive(type, active_key);
            if (data) {
                andebug(string_view("Starting object, name: "s + data.name()));
                // AnsonAst ast = contxt->asts->at(stack.back().astid);
                // std::string fd_astid = ast.fields.at(data.name()).astid;
                // stack.push_back({.instance = data.get(stack.back()), .astid=fd_astid});

                if (contxt->asts->contains(stack.back().astid)) {
                    // jsonable
                    AnsonAst ast = contxt->asts->at(stack.back().astid);
                    std::string fd_astid = ast.fields.at(data.name()).astid;


                    if (contxt->asts->contains(fd_astid))
                        stack.push_back({.instance = data.get(stack.back()),
                                     .astid=fd_astid});
                    else // map
                        stack.push_back({.instance = data.get(stack.back()),
                                     .astid="map<string, string"});
                }

            } else {
                anerror(string_view("Starting object, cannot find object field, key: " + std::to_string(active_key)));
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
        return true;
    }

    bool end_object() override {
        if (stack.size() > 1) stack.pop_back();
        active_key = 0;
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
        andebug(string_view(val));
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
        if (active_key != 0 && !stack.empty()) {
            auto data = stack.back().instance.type().data(active_key);
            if (data) {
                auto sequence_instance = data.get(stack.back().instance);
                push_list(sequence_instance, active_key);
                // Reset active_key so elements inside the array don't try to use it
                active_key = 0;
            }
        }
        return true;
    }
    bool end_array() override {
        if (!stack.empty() && stack.back().is_list) {
            auto finished_list = stack.back().instance;
            id_type key_to_update = stack.back().activekey;

            stack.pop_back(); // Remove the list from stack

            // Now, WRITE the finished list back to the parent object
            if (!stack.empty() && key_to_update != 0) {
                auto data = stack.back().instance.type().data(key_to_update);
                if (data) {
                    data.set(stack.back().instance, finished_list);
                }
            }
        }
        active_key = 0;
        return true;
    }


    /////////////////////////////////////////////////////////////////
    bool parse_error(std::size_t, const std::string&, const nlohmann::detail::exception&) override { return false; }

    void push(T &obj) {
        stack.push_back({.instance = forward_as_meta(obj), .astid=obj.anclass, .is_list=false, .is_map=false, .activekey=0});
    }

    void push_list(auto &instance, id_type active_key) {
        stack.push_back({.instance = instance, .astid="LIST", .is_list=true, .is_map=false, .activekey=active_key});
    }

    void push_map(auto &instance, id_type active_key) {
        stack.push_back({.instance = instance, .astid="MAP", .is_list=false, .is_map=true, .activekey=active_key});
    }
};

inline static bool parse(const string& json, IJsonable & an, const JsonOpt * opts) {
    EnTTSaxParser handler{an, opts};
    return nlohmann::json::sax_parse(json, &handler);
}
}

