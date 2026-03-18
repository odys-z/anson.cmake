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

class JsonOpt;

class IJsonable {

public:
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

    IJsonable(string type) : anclass(type) {}

    virtual IJsonable* toBlock(ostream& os, JsonOpt& opts) = 0;

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

    // int tree_sitter_test() { return 0; }
    // char& s_test0;
    // char& s_test1;
    // char** s_test2;
    // char** s_test3;

    virtual ~IJsonable() {}
};

// template<typename Drv>
class JavaEnum : public IJsonable {

public:
    inline static const string _type_ = "io.odysz.anson.JavaEnum";

    // std::function<string(JavaEnum)> decoder;
    string enm;

    /** This is invisible in java. */

    JavaEnum(string e) : enm(std::move(e)), IJsonable(_type_) {
        if (encode.contains(e))
            enm = encode[e];
        else
            enm = e;
    }

    JavaEnum(const JavaEnum&) = default;
    JavaEnum(JavaEnum&&) noexcept = default;

    JavaEnum& operator=(const JavaEnum&) = default;
    JavaEnum& operator=(JavaEnum&&) noexcept = default;

    // string encode() { return enm; }


    /** Registered by json.h */
    inline static map<string, string> decode;
    inline static map<string, string> encode;

    static string url(string enm) {
        return enm; // decode[enm];
    }

    static JavaEnum& valof(string s, JavaEnum& stub) {
        // if (decode.contains(s))
        //     stub.enm = s;
        // else stub.enm = "na";

        return stub;
    }

    IJsonable* toBlock(ostream& stream, JsonOpt& opts) override {
        // stream.put('\"');
        // stream.write(name().c_str(), name().size());
        // stream.put('\"');
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

/**
 * @brief The Anson class
 * java type: io.odysz.anson.Anson
 */
class Anson : public IJsonable {
public:
    inline static const string _type_ = "io.odysz.anson.Anson";

    string type;

    Anson() : IJsonable(_type_), type(_type_) {
        aninfo("defalut contructor");
    }

    Anson(string t) : IJsonable(t), type(t) {
        andebug("override constructor, type = " + t);
    }

    /**
     * @brief Anson
     * @param t
     * @param astid can be different if is a template specialized.
     */
    Anson(string t, string astid) : IJsonable(astid), type(t) {
        andebug("override constructor, astid = " + astid);
    }

    template <typename T>
    static bool from_json(const string& json, T& an) {
        EnTTSaxParser<T> handler{an};
        return nlohmann::json::sax_parse(json, &handler);
    }

    Anson& toBlock() {
        return *this;
    }

    IJsonable* toBlock(ostream& os, JsonOpt& opts) override {
        // ...
        return this;
    }

    IJsonable* toJson(string& buf) override {
        // ...
        return this;
    }
};

class JsonOpt: public Anson {
public:
    bool escape4DB;
    string doubleFormat;
    string indent;
};

class SemanticObject : public Anson {
public:
    map<string, std::any> data;
};

class AnstField {
public:
    string fieldname;
    string datatype;
    meta_type enttype;
    int length;

    /**
     * @brief static_val
     * MsgCode.ok = 0, ...
     */
    string static_val;

    //////////////////////// v 0.2.0
    string astid;
    hashed_string enttypeid;
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
class AnsonAst {
public:
    inline static bool valid_type(const string& typ) {
        return !LangExt::isblank(typ);
    }

    bool isEnum;
    bool isJavaEnum;
    string base;
    string anclass;
    map<string, AnstField> fields;
    map<string, int> enums;
    vector<vector<string>> ctors;

    // meta_type meta_type;
    hashed_string enttypeid;

    AnsonAst() : isEnum(false), anclass("") { }

    AnsonAst(string type, bool isEnum = false) : isEnum(isEnum), anclass(type) { }

};

// inline bool operator!(const IJsonable& an) {
//     return AnsonAst::valid_type("an.type ???");
// }


template <typename E>
concept JsonEnum = std::is_enum_v<E>;

// inline ostream& serialize_enum(const JsonEnum auto &e,
                                // AnsonAst &ast, std::ostream &os) {

    // return os << static_cast<std::underlying_type_t<decltype(e)>>(e);
    // return  os << '\"' << javaenum.enm << '\"';
// }
inline ostream& serialize_enum(const meta_any &instance, meta_type &type,
                                AnsonAst &ast, std::ostream &os) {
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

inline ostream& serialize_recursive(const meta_any &instance, AnsonAst &ast, map<string, AnsonAst> &asts, std::ostream &os);

inline ostream& serialize_kvs(const meta_any &instance,
                map<string, AnsonAst> &asts, std::ostream &os, bool &first) {

    meta_type type = instance.type();
    const AnsonAst &ast = asts.at(type.name());

    for (auto [id, feild] : ast.fields) {
        if (!first) {
            first = false;
            os << ", ";
        }

        os << "\"" << feild.fieldname << "\": ";

        // if (ast.isEnum) {
        //     // serialize_enum(data, ast, os);
        //     // return os << entt::any_cast<int>(instance.as_ref());
        //     // return os << static_cast<std::underlying_type_t<decltype(instance)>>(instance);
        //     if (auto value = instance.try_cast<int>(); value) {
        //         os << *value;
        //     } else if (auto u_value = instance.try_cast<unsigned int>(); u_value) {
        //         os << *u_value;
        //     } else {
        //         // Fallback: try to convert via the meta system to a type we can print
        //         // This works if the enum is registered as a meta type
        //         os << instance.cast<int>();
        //     }
        // }
        // else {
        //     // meta_type type = instance.type();
        //     // auto id = entt::hashed_string{type.name()}.value();
        //     serialize_recursive(instance, asts, os);
        // }

        serialize_recursive(instance, asts.at(feild.astid), asts, os);
    }
    return os;
}

inline ostream& serialize_obj(const meta_type &type, const meta_any &instance,
                              map<string, AnsonAst> &asts, std::ostream &os) {
    // 1. First, handle base classes (Recursive)
    bool first{true};
    os << "{";

    // meta_type type = instance.type();

    for (auto [id, base] : type.base())
        serialize_kvs(instance, asts, os, first);

    serialize_kvs(instance, asts, os, first);

    os << "}";
    return os;
}

inline ostream& serialize_recursive(const meta_any &instance, AnsonAst &ast,
                                    map<string, AnsonAst> &asts, std::ostream &os) {
    // if (!instance) return os;
    // meta_type type = instance.type();
    meta_type type = resolve(ast.enttypeid);

    string type_name = type.name();
    // AnsonAst &ast = asts.at(type.name());

    // 1. Dereference pointers (shared_ptr<EchoReq> -> EchoReq)
    if (type.is_pointer() || type.is_pointer_like()) {
        auto deref = *instance;
        serialize_recursive(deref, ast, asts, os);
        return os;
    }

    // 2. Strings (Specific check before sequence)
    if (auto s = instance.try_cast<std::string>()) {
        os << "\"" << *s << "\"";
        return os;
    }

    // 2.1. Enums
    if (ast.isEnum) {
        // if (auto p = instance.try_cast<anson::Port>()) os << *p;
        return serialize_enum(instance, type, ast, os);
    }

    // 2.2. Port
    if (ast.isJavaEnum) {
        // return os << "\"" << instance.try_cast<JavaEnum>()->enm << "\"";
        // return os << instance.try_cast<JavaEnum>();

        // if (auto* ptr = instance.try_cast<JavaEnum>()) {
        //     return os << *ptr;
        // }

        return os << instance.try_cast<JavaEnum>();
    }

    // 3. Sequence Containers (std::vector, etc.)
    // We use the meta_type to get the sequence view
    if (type.is_sequence_container()) {
        auto view = instance.as_sequence_container();
        // If as_sequence() still fails here, use the explicit version:
        // auto view = meta_sequence_view{instance};
        os << "[";
        bool first = true;
        for (auto element : view) {
            if (!first) os << ", ";
            serialize_recursive(element, ast, asts, os);
            first = false;
        }
        os << "]";
        return os;
    }

    // 4. Associative Containers (std::map)
    if (type.is_associative_container()) {
        auto view = instance.as_associative_container();
        os << "{";
        bool first = true;
        for (auto [key, value] : view) {
            if (!first) os << ", ";
            serialize_recursive(key, ast, asts, os);
            os << ": ";
            serialize_recursive(value, ast, asts, os);
            first = false;
        }
        os << "}";
        return os;
    }

    // 5. Handling std::any for UserReq::data
    if (auto a = instance.try_cast<std::any>()) {
        // Check for shared_ptr<Anson> as requested
        if (a->has_value() && a->type() == typeid(std::shared_ptr<anson::Anson>)) {
            serialize_obj(type, instance, asts, os);
        }
        return os;
    }

    // 6. General Objects (Reflection)
    return serialize_obj(type, instance, asts, os);
}

inline string serialize_json(const IJsonable &jsonable,
                             map<string, AnsonAst> &asts) {
    AnsonAst ast = asts.at(jsonable.anclass);
    const meta_type &type = resolve(ast.enttypeid);

    entt::meta_any instance{entt::forward_as_any(jsonable)};
    if (!instance)  return string(nullptr);

    std::stringstream ss;
    serialize_recursive(instance, ast, asts, ss);
    return std::move(ss).str();
}

// inline bool is_javaenum(const meta_type &type) {
//     auto enum_base = entt::resolve<anson::JavaEnum>();

//     auto typname = type.name();
//     if (!typname) return false;
//     else if (string_view(typname) == "JavaEnum") return true;

//     for (auto&& [id, base] : type.base()) {
//         if (base == enum_base) {
//             return true;
//         }
//     }
//     return false;
// }

inline bool is_javaenum(const meta_type &field_type) {
    auto enum_base = entt::resolve<anson::JavaEnum>();
    return field_type.can_cast(enum_base);
}

template<typename T>
class EnTTSaxParser : public nlohmann::json_sax<nlohmann::json> {
private:
    std::vector<meta_any> stack;
    id_type active_key{0};

    // Helper to set values on the current object in the stack
    template<typename V>
    void set_value(V&& val) {
        if (!stack.empty() && active_key != 0) {
            auto data = stack.back().type().data(active_key);
            if (data) {
                if (is_javaenum(data.type())) {
                    auto v = data.type().construct(val);
                    data.set(stack.back(), v);
                }
                else
                    data.set(stack.back(), std::forward<V>(val));
            }
        }
    }

public:
    EnTTSaxParser(T& obj) {
        // set_top(obj);
        stack.push_back(forward_as_meta(obj));
    }

    bool start_object(std::size_t size) override {
        if (active_key != 0 && !stack.empty()) {
            auto data = stack.back().type().data(active_key);
            if (data) {
                stack.push_back(data.get(stack.back()));
            }
        } else if (stack.empty()) {
            // This is the root object.
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
        // set_value(val);
        return true;
    }

    bool binary(binary_t&) override { return true; }
    bool start_array(std::size_t) override { return true; }
    bool end_array() override { return true; }
    bool parse_error(std::size_t, const std::string&, const nlohmann::detail::exception&) override { return false; }

    // Root Management
    // void set_top(meta_any instance) { stack.push_back(instance); }
};
}

