#pragma once

#include <exception>
#include <algorithm>
#include <string>
#include <regex>
#include <map>
#include <unordered_set>
#include <boost/url/parse.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include <format>
#include <variant>
#include <optional>
#include <string>
#include <vector>
#include <chrono>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/sha.h>

#include "utils.h"

namespace anson {
using namespace std;

inline static map<string, string> primtypes_c20 {
    {"String", "string"}, {"string", "string"}, {"java.lang.String", "string"},
    {"int", "int"}, {"Integer", "int"}, {"java.lang.Integer", "int"},
    {"short", "int"}, {"Short", "int"}, {"java.lang.Short", "int"},
    {"long", "long"}, {"Long", "long"}, {"java.lang.Long", "long"},
    {"float", "float"}, {"Float", "float"}, {"java.lang.Float", "float"},
    {"double", "double"}, {"Double", "double"}, {"java.lang.Double", "double"},
    {"boolean", "boolean"}, {"Boolean", "boolean"}, {"java.lang.Boolean", "boolean"},
    {"VarType", "VarType"}, {"LangExt::VarType", "VarType"}, {"anson::LangExt::VarType", "VarType"},
};

#define JavaConstr string

template<typename T>
class JavaConsts {
public:
    T v;
    map<string, T> nmp;

    JavaConsts(T v) : nmp(), v(v) {}
    /**
     * Returns the string representation of this share flag,
     * mimicking Java's Enum.name()
     */
    virtual const T& name() { return nmp[v]; }
};

class LangExt {

public:
    /**
     * @brief split
     * @param str
     * @param delim
     * @return string_views to the str.
     *
     * IMPORTANT
     *
     * this is a NRV (Named Return Value), so
     * don't return it further beyond where the str exists.
     *
     * auto bad() {
     *
     *   std::string temp = "hello,world";
     *
     *   return split_view(temp, ',');          // ← DANGER! dangling views!
     *
     * }
     *
     * but, e.g.
     *
     * long_live.vec = vector<string>(ssview.begin(), ssview.end());
     *
     */
    inline static vector<string_view> split(string_view str, char delim = ' ') {
        vector<std::string_view> result;

        size_t start = 0;
        while (start < str.size()) {
            auto end = str.find(delim, start);
            if (end == std::string_view::npos) {
                result.push_back(str.substr(start));
                break;
            }
            if (end > start) {
                result.push_back(str.substr(start, end - start));
            }
            start = end + 1;
        }
        if (result.size() == 0)
            result.push_back(str);

        return result;
    }

    inline static std::string trim(std::string_view sv) {
        // Skip leading whitespace
        size_t start = 0;
        while (start < sv.size() && std::isspace(static_cast<unsigned char>(sv[start]))) {
            ++start;
        }

        // Skip trailing whitespace
        size_t end = sv.size();
        while (end > start && std::isspace(static_cast<unsigned char>(sv[end - 1]))) {
            --end;
        }

        return std::string(sv.substr(start, end - start));
    }

    template <typename Range>
    inline static string join(const Range& range, const string& sep = ",", const string& front = "", const string& back = "") {
        std::ostringstream oss;
        auto it = std::begin(range);
        auto end = std::end(range);

        oss << front;

        if (it != end) {
            oss << *it++;
            while (it != end) {
                oss << sep << *it++;
            }
        }
        oss << back;
        return oss.str();
    }

    inline static string join(const vector<string_view> from, int begin, int until = -1, const string& sep = ",", const string& front = "", const string& back = "") {
        std::ostringstream oss;

        oss << front;

        bool first = true;
        for (int i = begin; until < 0 && i < from.size() || i < until; i++) {
            if (first) first = false;
            else oss << sep;
            oss << from[i];
        }

        oss << back;
        return oss.str();
    }

    inline static bool isblank(const string s) {
        int l = s.length();
        bool b = l == 0;
        return s.length() == 0;
    }

    inline static bool isblank(const string s, const string &blank) {
        return s.length() == 0 || s == blank;
    }

    inline static bool isblank(const string s, const string &blank0, const string &blank1) {
        return s.length() == 0 || s == blank0 || s == blank1;
    }

    inline static bool isblank(const string s, const vector<string> &asblank) {
        if (s.length() == 0)
            return true;
        else for (string as : asblank)
                if (s == as) return true;
        return false;
    }

    inline static string upper_case(const string s) {
        std::string result = s; // Make a copy
        for (auto &c : result) {
            c = std::toupper(static_cast<unsigned char>(c));
        }
        return result;
    }

    /**
     * @brief ifnull
     * @return string_view for hard copying
     */
    inline static string_view ifnull(string_view s, string_view null) {
        if (s.length() == 0)
            return null;
        else return s;
    }

    using VarType = std::variant<std::monostate,
                                 int, double, float,
                                 std::string,
                                 std::chrono::system_clock::time_point>;

    inline static std::optional<std::string> var_str(VarType v) {
        return std::visit([](auto&& arg) -> std::optional<std::string> {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::monostate>) return std::nullopt;

            if constexpr (std::is_same_v<T, std::string>) return arg;

            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, double>)
                return std::to_string(arg);

            else if constexpr (std::is_same_v<T, std::chrono::system_clock::time_point>)
                return std::format("{:%Y-%m-%d %H:%M:%S}", arg);

            return std::nullopt;
        }, v);
    }

    inline static std::optional<int> var_int(VarType v) {
        if (auto ptr = std::get_if<int>(&v)) {
            return *ptr;
        }
        return std::nullopt;
    }

    inline static std::optional<double> var_double(VarType v) {
        if (auto ptr = std::get_if<double>(&v)) {
            return *ptr;
        }
        return std::nullopt;
    }

    inline static std::optional<chrono::system_clock::time_point> var_time(VarType v) {
        if (auto ptr = std::get_if<chrono::system_clock::time_point>(&v)) {
            return *ptr;
        }
        return std::nullopt;
    }

    inline static VarType any2var(entt::meta_any v) {
        using namespace entt::literals;

        if (!v) return std::monostate{};

        auto type = v.type();
        if (type == entt::resolve<int>()) return v.cast<int>();
        if (type == entt::resolve<double>()) return v.cast<double>();

        if (type == entt::resolve<std::string>()) return v.cast<std::string>();
        if (type == entt::resolve<const std::string>()) return v.cast<std::string>();
        if (type == entt::resolve<char*>()) return v.cast<char*>();
        if (type == entt::resolve<const char*>()) return v.cast<const char*>();

        // if (type == entt::resolve<float>()) return v.cast<float>();
        if (auto* f = v.try_cast<float>()) return *f;

        if (type == entt::resolve<std::chrono::system_clock::time_point>())
            return v.cast<std::chrono::system_clock::time_point>();

        return std::monostate{};
    }

    inline static ostream& serialize_var(ostream& os, const entt::meta_any & v) {
        using namespace entt::literals;

        if (!v) return os << "null";

        // Gemini: When wrap std::variant in a meta_any, EnTT sees the variant type, not the type currently held inside the variant.
        if (auto* var = v.try_cast<LangExt::VarType>()) {
            return std::visit([&](auto&& arg) -> ostream& {
                return serialize_var(os, entt::meta_any{arg});
            }, *var);
        }

        auto type = v.type();
        // if (auto* i = v.try_cast<int>())    return os << *i;
        // if (auto* d = v.try_cast<double>()) return os << *d;
        // if (auto* f = v.try_cast<float>()) return os << *f;

        if (type == entt::resolve<int>()) return os << v.cast<int>();
        if (type == entt::resolve<double>() || type == entt::resolve<float>()) {
            std::string s = std::to_string(v.cast<double>());
            // Remove unnecessary trailing zeros, but keep the ".0"
            s.erase(s.find_last_not_of('0') + 1, std::string::npos);
            if (s.back() == '.') s += '0';
            return os << s;
        }

        // if (type == entt::resolve<float>()) return os << v.cast<float>();

        if (auto* s = v.try_cast<std::string>()) return os << '"' << *s << '"';
        if (auto* s = v.try_cast<const std::string>()) return os << '"' << *s << '"';
        if (auto* s = v.try_cast<char*>()) return os << '"' << *s << '"';
        if (auto* s = v.try_cast<const char*>()) return os << '"' << *s << '"';

        if (auto* tp = v.try_cast<std::chrono::system_clock::time_point>())
            return os << format("{:%Y-%m-%d %H:%M:%S}", floor<std::chrono::seconds>(*tp));

        return os << "null";

    }

    inline static map<string, std::function<entt::meta_any()>*> var_ctors;

    inline static bool has_ctor(const string & vartyp) {
        return var_ctors.contains(vartyp);
    }

    /**
     * @brief call_ctor
     * @param tp
     * @return Gemini: will create different instance, e.g. vector for different callers.
     */
    inline static entt::meta_any call_ctor(const string &tp) {
        if (has_ctor(tp)) {
            auto *f = var_ctors.at(tp);
            return (*f)();
        }
        else
            return {};
    }

    inline static void register_ctor(const string & tp, std::function<entt::meta_any()> *c) {
        var_ctors[tp] = c;
    }
};

inline std::chrono::system_clock::time_point operator ""_t(const char* str, std::size_t len) {
    std::string s(str, len);
    std::istringstream ss{s};
    std::chrono::system_clock::time_point tp;

    if (len == 4) {
        ss >> std::chrono::parse("%Y", tp);
    } else if (len == 10) {
        ss >> std::chrono::parse("%F", tp);
    } else {
        ss >> std::chrono::parse("%F %T", tp);
    }
    return tp;
}

// inline LangExt::VarType operator ""_d(long double v) {
//     return static_cast<double>(v);
// }

// inline LangExt::VarType operator ""_l(unsigned long long v) {
//     return static_cast<long>(v);
// }

/**
 * Structure to hold decomposed URL parts
 */
struct HttpParts {
    bool https;
    int port;
    string scheme;
    string host;
    vector<string> paths;
    string query;
    string fragment;

    HttpParts& Paths(vector<string_view> ssview) {
        paths = vector<string>(ssview.begin(), ssview.end());
        return *this;
    }
};

class Regex {
public:
    inline static const std::regex isenvl{R"('?\{\s*["']type["']:)"};

    inline static std::regex httpregx{R"(^https?://)"};

    inline static std::regex httpsregx{R"(^https://)"};

    inline static std::regex is_begin_star{R"(^\s*\*\s*)"};

    /**
     * Checks if the scheme is https.
     * Boost.URL handles case-insensitivity automatically.
     */
    inline static bool isHttps(const std::string& s) {
        return regex_search(s, httpsregx);
    }

    /**
     * Checks if the scheme is http or https.
     * Boost.URL handles case-insensitivity automatically.
     */
    inline static bool isHttp(const std::string& s) {
        return regex_search(s, httpregx);
    }

    inline static bool startEnvelope(const string& envl) {
        return regex_search(envl, isenvl);
    }

    inline static std::regex reg_isIPv6{R"(^(([^:/?#]+):)?(//)?\[(::[0-9A-Fa-f]{1,4})|([0-9A-Fa-f]{1,4}(:[0-9A-Fa-f]{1,4}){7})\](:\d{1,8})?([/?#])?)"};

    inline static std::regex reg_hostportv6{R"(\[((::[0-9A-Fa-f]{1,4})|([0-9A-Fa-f]{1,4}(:[0-9A-Fa-f]{1,4}){7}))\](:(\d+))?)"};

    /**
     * Is the IP include a valid IP v6 address?
     *
     * @param ip
     * @return true if a valid ip
     */
    inline static bool isIPv6(string ip) {
        // smatch matches;
        return regex_search(ip, reg_isIPv6);
    }

    /**
     * groups[3]: ip[:port]
     * groups[4]: path
     * https://www.rfc-editor.org/rfc/rfc3986#appendix-B
     */
    inline static std::regex reg3986{R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)"};

    /**
     * Regex for RFC3986 Schema
     */
    inline static std::regex schemePrefix{R"(^(\w+:)?//)"};

    inline static std::regex reg_host_v6_lr{R"((^\[)|(\]$))"};

    inline static std::regex reg_start_slash{R"(^/*)"};

    /**
     * @param semiJserv
     * @return nomalized jserv url (all url parts, in RFC3986, are present)
     */
    inline static string asJserv(string semiJserv) {
        HttpParts parts;
        getHttpParts(semiJserv, parts);
        return format("{}://{}{}{}{}{}", // schema, host, :port, /sub-paths, ?query, #fragment
                 parts.https ? "https" : "http",
                 parts.host,
                 80 == parts.port || 443 == parts.port ? "" : ":" + to_string(parts.port),
                 "/" + LangExt::join(parts.paths, "/"),
                 LangExt::isblank(parts.query) ? "" : "?" + parts.query,
                 LangExt::isblank(parts.fragment) ? "" : "#" + parts.fragment);
    }

    /**
     *
     * @param url
     * @return<pre>
     * [0] is ip v6 address
     * [1] scheme true: https, false: possibly http
     * [2] authority host
     * [3] authority port
     * [4] sub-paths, String[]
     * [5] query
     * [6] fragment</pre>
     * @since 0.9.130
     */
    inline static HttpParts& getHttpParts(string url, HttpParts &parts) {

        if (Regex::isIPv6(url)) {
            getHttpsPartsv6(url, parts);
        }
        else getHttpsPartsv4(url, parts);

        return parts;
    }

    /**
     * Parse parts in a URl with valid IP v6 address.
     * Return of invalid IP v6 address is unspecified.
     * @param url
     * @return<pre>
     * [0] scheme true: https, false: possibly http
     * [1] authority host
     * [2] authority port
     * [3] sub-paths, String[]
     * [4] query
     * [5] fragment</pre>
     * @since 0.9.130
     */
    static HttpParts& getHttpsPartsv6(string url, HttpParts& parts) {
        if (!regex_search(url, schemePrefix))
            url = "http://" + url;

        smatch grps;
        regex_match(url, grps, reg3986);
        if (LangExt::isblank(grps[4]))
            return parts;

        bool https = "https" == grps[2];
        bool http  = "http" == grps[2] || LangExt::isblank(grps[2]);
        int port = https ? 443 : http? 80 : 0;

        string host = grps[4];
        smatch iportss;
        regex_match(host, iportss, reg_hostportv6);

        if (iportss.size() == 7) {
            // Debug Notes: Don't change host before iportss matchs are no-longer accessed.
            // The matched groups are references to the original string.
            string p = iportss[6];
            host = iportss[1];
            try { port = stoi(p); }
            catch (exception& e) {
                cerr << p << ' ' << e.what();
            }
        }

        parts.https = https;
        parts.host = "[" + regex_replace(host, reg_host_v6_lr, "") + "]";
        parts.port = port;
        if (!LangExt::isblank(grps[5], "/+"))
            parts.Paths(LangExt::split(regex_replace(string(grps[5]), reg_start_slash, ""), '/'));
        parts.query = grps[7];
        parts.fragment = grps[9];

        return parts;
    }

    /**
     * Get URL parts.
     * See <a href='https://www.rfc-editor.org/rfc/rfc3986#section-3'>Section 3, RFC 3986</a>.
     * <pre>
     * foo://example.com:8042/over/there?name=ferret#nose
     * \_/   \______________/\_________/ \_________/ \__/
     * |           |            |            |        |
     * scheme     authority       path        query   fragment
     * </pre>
     * @param url
     * @return<pre>
     * [0] scheme true: https, false: possibly http
     * [1] authority host
     * [2] authority port
     * [3] sub-paths, String[]
     * [4] query
     * [5] fragment</pre>
     */
    static HttpParts& getHttpsPartsv4(string url, HttpParts& parts) {
        if (!regex_search(url, schemePrefix))
            url = "http://" + url;

        // ArrayList<String> grps = reg3986.findGroups(url);
        smatch grps;
        regex_match(url, grps, reg3986);

        if (LangExt::isblank(grps[4]))
            return parts;

        // aninfo(grps, {.head="\nGroups:\n", .sep="\n", .pre_item = [](int x){return format("[{}]", x);}});

        bool https = "https" == grps[2];
        std::string_view vhttp(grps[2].first, grps[2].second); // Only a MSVC problem?
        bool http  = "http" == LangExt::ifnull(vhttp, "http");
        int port = https ? 443 : http? 80 : 0;

        string host = grps[4];
        vector<string_view> iportss = LangExt::split(host, ':'); // host.split(":");

        if (iportss.size() == 2) {
            host = iportss[0];
            try { port = stoi(string(iportss[1])); }
            catch (exception e) {}
        }

        parts.https = https;
        parts.host = host;
        parts.port = port;
        if (!LangExt::isblank(grps[5], "/"))
            parts.Paths(LangExt::split(regex_replace(string(grps[5]), reg_start_slash, ""), '/'));
        parts.query = grps[7];
        parts.fragment = grps[9];

        return parts;
    }

    inline static bool is_pointer_type(const string & s) {
        return regex_search(s, is_begin_star);
    }

    inline static vector<string> parse_val_type(const string & val_type) {
        // string | int | *Anson | shared_ptr<val_type | ...
        if (is_pointer_type(val_type)) {
            string r = std::regex_replace(string{val_type}, is_begin_star, "");
            return {r, "true"};
        }

        vector<string_view> tss = LangExt::split(val_type, '<');

        if (tss.at(0) != "shared_ptr") {
            return {LangExt::trim(val_type), "false"};
        }

        if (tss.size() > 1)
            return {LangExt::trim(LangExt::join(tss, 1, -1, "<")), "true"};

        return {LangExt::trim(val_type), "false"};
    }

    /**
     * @brief parseMapValtype
     * @param map_type
     * @return [list-element-type, is-shared_ptr]
     */
    inline static vector<string> parseMapValtype(const string &map_type) {
        // map<string, val_type
        vector<string_view> tss = LangExt::split(map_type, '<');
        if (tss.size() < 1)
            return {map_type, "false"};

        string kvtype = LangExt::join(tss, 1, -1, "<");
        vector<string_view> kvss = LangExt::split(kvtype, ',');
        if (tss.at(0) != "map" || kvss.at(0) != "string" || kvss.size() <= 1) {
            anwarn(string_view("Not a regular map type: "s + map_type));
            return {LangExt::trim(map_type), "false"};
        }

        string valtype = LangExt::trim(LangExt::join(kvss, 1, -1, ","));
        return parse_val_type(valtype);

    }

    inline static vector<string> parseListValtype(const string &list_type) {
        // list<val_type
        vector<string_view> tss = LangExt::split(list_type, '<');
        if (tss.at(0) != "list") {
            anwarn(string_view("Not a regular list type: "s + list_type));
            return {LangExt::trim(list_type), "false"};
        }

        return parse_val_type(LangExt::join(tss, 1, -1, "<"));
    }
};

inline static int default_port(const string &scheme) {
    return scheme == "https" ? 443
        : scheme == "http" ? 80
        : scheme == "ws" ? 80
        : scheme == "wss" ? 443
        : scheme == "ftp" ? 21
        : scheme == "ftps" ? 90
        : scheme == "ssh" ? 22
        : scheme == "sftp" ? 22
        : scheme == "smtp" ? 25
        : scheme == "pop3" ? 110
        : scheme == "imap" ? 143
        : scheme == "ldaps" ? 636
        : 0
        ;
}

/**
 * Validates if the port is within an allowed list/range.
 */
inline static bool validUrlPort(int port, const std::vector<int>& range) {
    if (range.empty()) return port > 0;
    return (range[0] < 0 || port >= range[0])
           && (range.size() < 2 || range[1] < 0 || port <= range[1]);
}

class UrlValidator {
public:
    static constexpr uint64_t ALLOW_ALL_SCHEMES = 1 << 0;
    static constexpr uint64_t ALLOW_2_SLASHES   = 1 << 1;
    static constexpr uint64_t NO_FRAGMENTS      = 1 << 2;
    static constexpr uint64_t ALLOW_LOCAL_URLS  = 1 << 3;

    explicit UrlValidator(uint64_t options = 0) : options_(options) {
        allowed_schemes_ = {"http", "https"};
    }

    bool isValid(std::string_view url) const {
        if (url.empty()) return false;

        try {
            boost::urls::url u(url);
            // std::cout << u << std::endl;
            string scheme = u.scheme();
            return isValidScheme(scheme);
        } catch (const boost::system::system_error& e) {
            std::cerr << "Invalid URI: " << e.what() << std::endl;
            return false;
        }

        return true;
    }

    inline bool isValidScheme(string scheme) const {
        return std::find(allowed_schemes_.begin(), allowed_schemes_.end(), scheme)
                != allowed_schemes_.end();
    }

private:
    uint64_t options_;
    std::unordered_set<std::string> allowed_schemes_;
};

class AESHelper2 {
public:
    inline static std::vector<unsigned char> base64_decode(const std::string& base64) {
        BIO *bio, *b64;
        std::vector<unsigned char> buffer(base64.length());
        bio = BIO_new_mem_buf(base64.c_str(), -1);
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_push(b64, bio);
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Important: No newlines
        int length = BIO_read(bio, buffer.data(), base64.length());
        buffer.resize(length);
        BIO_free_all(bio);
        return buffer;
    }

    inline static std::vector<unsigned char> hash_key_sha256(const std::string& raw_key) {
        std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
        SHA256(reinterpret_cast<const unsigned char*>(raw_key.data()), raw_key.size(), hash.data());
        return hash;
    }
};
}
