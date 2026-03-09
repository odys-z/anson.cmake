#pragma once

#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>

using namespace  std ;

namespace anson {


class JsonOpt;// : public Anson { };

class JavaEnum {

public:
    // std::function<string(JavaEnum)> decoder;

    /** This is invisible in java. */
    string enm;

    JavaEnum(string e) {
        if (encode.contains(e))
            enm = encode[e];
        else
            enm = "na:" + e;
    }

    // string encode() { return enm; }


    /** Registered by json.h */
    inline static map<string, string> decode;
    inline static map<string, string> encode;

    static string url(string enm) {
        return decode[enm];
    }

    static JavaEnum& valof(string s, JavaEnum& stub) {
        if (decode.contains(s))
            stub.enm = s;
        else stub.enm = "na";

        return stub;
    }
};

inline std::ostream& operator<<(std::ostream& os, const JavaEnum& enm) {
    return os << '\"' << enm.enm << '\"';
}

class IJsonable {

public:
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

template<typename T>
class EnTTSaxParser;

/**
 * @brief The Anson class
 * java type: io.odysz.anson.Anson
 */
class Anson : public IJsonable {
public:
    inline static const string _type_ = "io.odysz.anson.Anson";
    std::string type;

    Anson() : IJsonable() { cout << "defalut contructor" << endl ; }
    Anson(string t) : type(t) { cout << "override constructor, type = " << t << endl ; }
    
    template <typename T>
    static bool from_json(const string& json, T& an) {
        EnTTSaxParser<T> handler{an};
        return nlohmann::json::sax_parse(json, &handler);
    }

    void toBlock() {

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

class SemanticObject : public Anson {
public:
    map<string, any> data;
};

using namespace entt;

inline ostream& serialize_enum(const meta_type type, const meta_any instance,
               map<string, map<string, int>*> &enum_vals, std::ostream &os) {
    string name = type.name();
    for (auto [id, data] : type.data()) {
        if (enum_vals.at(name)->at(data.name()) == data.get(instance).cast<int>())
            return os << '\"' << name << '\"';
    }
    return os;
}

inline ostream& serialize_recursive(const meta_any &instance,
                                    map<string, map<string, int>*> &enumtypes, std::ostream &os);

inline ostream& serialize_kvs(const meta_type &type, const meta_any &instance,
                map<string, map<string, int>*> &enumtypes, std::ostream &os, bool &first) {

    // // 1. First, handle base classes (Recursive)
    // for (auto [id, base] : type.base()) {
    //     serialize_object_fields(base.type(), instance, os, first);
    // }

    // 2. Then, handle fields of the current class
    for (auto [id, data] : type.data()) {
        if (!first) os << ", ";

        // Use .name() from your meta_factory registration
        os << "\"" << data.name() << "\": ";

        // Pass the instance to data.get() to extract the value
        serialize_recursive(data.get(instance), enumtypes, os);
        first = false;
    }
    return os;
}

inline ostream& serialize_obj(const meta_type &type, const meta_any &instance,
                              map<string, map<string, int>*> &enumtypes, std::ostream &os) {
    // 1. First, handle base classes (Recursive)
    bool first{true};
    os << "{";

    for (auto [id, base] : type.base())
        serialize_kvs(base, instance, enumtypes, os, first);

    serialize_kvs(type, instance, enumtypes, os, first);

    os << "}";
    return os;
}

inline ostream& serialize_recursive(const meta_any &instance,
                                    map<string, map<string, int>*> &enumtypes, std::ostream &os) {
    if (!instance) return os;

    meta_type type = instance.type();

    // 1. Dereference pointers (shared_ptr<EchoReq> -> EchoReq)
    if (type.is_pointer() || type.is_pointer_like()) {
        auto deref = *instance;
        serialize_recursive(deref, enumtypes, os);
        return os;
    }

    // 1. Strings (Specific check before sequence)
    if (auto s = instance.try_cast<std::string>()) {
        os << "\"" << *s << "\"";
        return os;
    }

    // 2.1. Enums
    if (type.is_enum()) {
        // if (auto p = instance.try_cast<anson::Port>()) os << *p;
        return serialize_enum(type, instance, enumtypes, os);
    }
    // 2.2. Port
    // if (type == entt::resolve<anson::Port>())
    auto portname = type.name();
    if (portname && string_view(portname) == "Port") {
        // return os << "\"" << instance.try_cast<JavaEnum>()->enm << "\"";
        // return os << instance.try_cast<JavaEnum>();
        if (auto* ptr = instance.try_cast<JavaEnum>()) {
            return os << *ptr;
        }
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
            serialize_recursive(element, enumtypes, os);
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
            serialize_recursive(key, enumtypes, os);
            os << ": ";
            serialize_recursive(value, enumtypes, os);
            first = false;
        }
        os << "}";
        return os;
    }

    // 5. Handling std::any for UserReq::data
    if (auto a = instance.try_cast<std::any>()) {
        // Check for shared_ptr<Anson> as requested
        if (a->has_value() && a->type() == typeid(std::shared_ptr<anson::Anson>)) {
            serialize_obj(type, instance, enumtypes, os);
        }
        return os;
    }

    // 6. General Objects (Reflection)
    return serialize_obj(type, instance, enumtypes, os);
}

inline string serialize_json(const meta_any &instance,
                             map<string, map<string, int>*> &enumtypes) {
    if (!instance)  return string(nullptr);

    std::stringstream ss;
    serialize_recursive(instance, enumtypes, ss);
    return ss.str();
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
                data.set(stack.back(), std::forward<V>(val));
            }
        }
    }

public:
    EnTTSaxParser(Anson& obj) {
        set_top(obj);
    }

    bool start_object(std::size_t size) override {
        if (active_key != 0 && !stack.empty()) {
            auto data = stack.back().type().data(active_key);
            if (data) {
                stack.push_back(data.get(stack.back()));
            }
        } else if (stack.empty()) {
            // This is the root object, we assume the caller set it up
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
    void set_top(meta_any instance) { stack.push_back(instance); }
};
}

