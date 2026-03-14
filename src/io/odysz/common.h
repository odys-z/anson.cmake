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

#include "utils.h"

namespace anson {
using namespace std;

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
     * @return string_views to the str. IMPORTANT this is a NRV (Named Return Value), so
     * don't return it further beyond will str exists.
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

        return result;
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

    // template <typename Range>
    // inline static string join(const Range& range, const string& sep = ",") {
    //     return LangExt::join(range, sep, "", "");
    // }

    // template <typename Range>
    // inline static string join(const Range& range) {
    //     return join(range, ",", "", "");
    // }

    inline static bool isblank(const string s) {
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

    /**
     * @brief ifnull
     * @return string_view for hard copying
     */
    inline static string_view ifnull(string_view s, string_view null) {
        if (s.length() == 0)
            return null;
        else return s;
    }
};

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

    /**
     * Checks if the scheme is https.
     * Boost.URL handles case-insensitivity automatically.
     */
    inline static bool isHttps(const std::string& s) {
        // auto r = urls::parse_uri(s);
        // if (!r) return false;
        // return r->scheme_id() == urls::scheme::https;
        return regex_search(s, httpsregx);
    }

    /**
     * Checks if the scheme is http or https.
     * Boost.URL handles case-insensitivity automatically.
     */
    inline static bool isHttp(const std::string& s) {
        // auto r = urls::parse_uri(s);
        // if (!r) return false;
        // return r->scheme_id() == urls::scheme::http ||
        //    r->scheme_id() == urls::scheme::https;
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

        // ArrayList<String> grps = reg3986.findGroups(url);
        smatch grps;
        regex_match(url, grps, reg3986);
        if (LangExt::isblank(grps[4]))
            return parts;

        // Utils::print(grps, {.head="\nGroups:\n", .sep="\n", .pre_item = [](int x){return format("[{}]", x);}});
        aninfo(grps, {.head="\nGroups:\n", .sep="\n", .pre_item = [](int x){return format("[{}]", x);}});

        bool https = "https" == grps[2];
        bool http  = "http" == grps[2] || LangExt::isblank(grps[2]);
        int port = https ? 443 : http? 80 : 0;

        string host = grps[4];
        smatch iportss;
        regex_match(host, iportss, reg_hostportv6);

        // Utils::print(iportss, {.head="\nIP-port:\n", .sep="\n", .pre_item = [](int x){return format("[{}]", x);}});
        aninfo(iportss, {.head="\nIP-port:\n", .sep="\n", .pre_item = [](int x){return format("[{}]", x);}});

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
        // new Object[] { https, "[" + host.replaceAll("(^\\[)|(\\]$)", "") + "]", port,
        //         isblank(grps.get(4), "/+") ? null : grps.get(4).replaceAll("^/*", "").split("/"),
        //         grps.get(6), grps.get(8)};
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

        // Utils::print(grps, {.head="\nGroups:\n", .sep="\n", .pre_item = [](int x){return format("[{}]", x);}});
        aninfo(grps, {.head="\nGroups:\n", .sep="\n", .pre_item = [](int x){return format("[{}]", x);}});

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

        // return new Object[] { https, host, port,
        //         isblank(grps.get(4), "/+") ? null : grps.get(4).replaceAll("^/*", "").split("/"),
        //         grps.get(6), grps.get(8)};

        parts.https = https;
        parts.host = host;
        parts.port = port;
        if (!LangExt::isblank(grps[5], "/"))
            parts.Paths(LangExt::split(regex_replace(string(grps[5]), reg_start_slash, ""), '/'));
        parts.query = grps[7];
        parts.fragment = grps[9];

        return parts;
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
 *  This is a short cut to the java Regex.asJserv(...).
inline static string asJserv(const string& url) {
    string s = url.starts_with("//") ? "http://" + url.substr(2) : url;
    cout << s << endl;
    return url.starts_with("//") ? "http://" + url.substr(2) : url;
}

inline static bool getHttpParts(string const& input, HttpParts& parts) {
    namespace urls = boost::urls;

    auto result = urls::parse_uri(input);   // or parse_uri_reference if relative URIs allowed

    if (!result) {
        std::cerr << "Invalid URI: " << result.error().message() << ' ' << input << "\n";
        return false;
    }

    urls::url_view u = *result;

    // Basic components
    parts.scheme = u.scheme();
    parts.https = u.scheme() == "https";
    parts.host   = u.host();
    parts.port   = u.port_number();

    if (parts.port == 0)
        parts.port = default_port(u.scheme());

    for (auto seg : u.segments())
        parts.paths.push_back(std::string(seg));

    return true;
}
 */

/**
 * Validates if the port is within an allowed list/range.
 */
inline static bool validUrlPort(int port, const std::vector<int>& range) {
    if (range.empty()) return port > 0;
    // TODO delete: A relic of LLM Programming (The java logic is actually given)
    // return std::find(range.begin(), range.end(), port) != range.end();
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

}
