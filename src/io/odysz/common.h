#pragma once

#include <algorithm>
#include <string>
#include <regex>
#include <map>
#include <string_view>
#include <unordered_set>
#include <boost/url/parse.hpp>
#include <boost/url/url_view.hpp>
#include <boost/url.hpp>
#include <iostream>

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

namespace urls = boost::urls;

class Regex {
public:
    inline static const std::regex isenvl{R"('?\{\s*["']type["']:)"};

    /**
     * Checks if the scheme is https.
     * Boost.URL handles case-insensitivity automatically.
     */
    inline static bool isHttps(const std::string& s) {
        auto r = urls::parse_uri(s);
        if (!r) return false;
        return r->scheme_id() == urls::scheme::https;
    }

    /**
     * Checks if the scheme is http or https.
     * Boost.URL handles case-insensitivity automatically.
     */
    inline static bool isHttp(const std::string& s) {
        auto r = urls::parse_uri(s);
        if (!r) return false;
        return r->scheme_id() == urls::scheme::http ||
           r->scheme_id() == urls::scheme::https;
    }

    /**
     * Checks if the host part of the URL is a valid IPv6 address.
     */
    inline static bool isIPv6(const std::string& s) {
        auto r = urls::parse_uri(s);
        return r && r->host_type() == urls::host_type::ipv6;
    }

    inline static bool startEnvelope(const string& envl) {
        return regex_search(envl, isenvl);
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
    std::vector<std::string> paths;
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

inline static bool getHttpParts(string const& input, HttpParts& parts) {
    namespace urls = boost::urls;

    auto result = urls::parse_uri(input);   // or parse_uri_reference if relative URIs allowed

    if (!result &&
        // This is a short cut to the java Regex.asJserv(...).
        !getHttpParts("http://" + (input.starts_with("//") ? input.substr(2) : input), parts)) {
        std::cerr << "Invalid URI: " << result.error().message() << input << "\n";
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
