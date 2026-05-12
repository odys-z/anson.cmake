#pragma once

#include <functional>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace anson {

struct PrintFormat {
    string head = "";
    string_view sep  = ",";
    string foot = "\n";
    bool breakline = false;
    function<string(int index)> pre_item = nullptr;
};

class Utils {
public:
     inline static std::string readOutputFile(std::string f) {
        std::ifstream file(f, std::ios::in | std::ios::binary);
        if (!file) return "";

        return std::string((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    }

    inline static std::vector<unsigned char> readBinaryFile(const std::string& f) {
        std::ifstream file(f, std::ios::binary | std::ios::ate); // Open at end to get size
        if (!file) return {};

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(size);
        if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            return buffer;
        }
        return {};
    }

    inline static void writeBinaryFile(std::string f, vector<unsigned char> content) {
        std::ofstream file(f, std::ios::binary);

        if (!file) {
            throw std::runtime_error("Failed to open file: " + f);
        }

        if (!content.empty()) {
            file.write(reinterpret_cast<const char*>(content.data()), content.size());
        }
    }

    template <typename Range>
    inline static ostream& _print(ostream& oss, Range list, const PrintFormat& f = {}) {
        auto it = std::begin(list);
        auto end = std::end(list);

        oss << f.head;

        int x = 0;
        while (it != end) {
            if (f.pre_item != nullptr) oss << f.pre_item(x++);

            oss << *it++;

            if (it != end) oss << f.sep;

            if (f.breakline) oss << endl;
        }
        oss << f.foot;
        return oss;
    }

    template <typename Range>
    inline static ostream& print(ostream& oss, const Range list,
                  const string head = "", const string sep = ",", const string foot = "") {
        return _print(oss, list, {.head = head, .sep = sep, .foot = foot});
    }

    template <typename Range>
    inline static void print(const Range list, string head = "", string sep = ",", string foot = "") {
        print(cout, list, head, sep, foot);
    }

    template <typename Range>
    inline static void print(const Range list, const PrintFormat& f) {
        _print(cout, list, f);
    }

    inline static void print(string_view msg, const PrintFormat& f = {.breakline=true}) {
        _print(cout, vector<string_view>{msg}, f);
    }

    inline static void print(const std::string& msg, const PrintFormat& f = {.breakline=true}) {
        _print(cout, vector<string_view>{msg}, f);
    }

    inline static void print(const char* s, const PrintFormat& f = {.breakline=true}) {
        print(std::string_view(s), f);
    }
};

#define LOG_ERROR   1
#define LOG_WARN    2
#define LOG_INFO    3
#define LOG_LOG     4
#define LOG_DEBUG   5

#ifdef ANSON_VERBOSE
#ifdef anprint
#undef anprint
#endif

#define anprint(v, ...) \
do{ \
    if ((v) <= ANSON_VERBOSE) { \
        std::cout << "<verbose " << v << '/' << ANSON_VERBOSE << ' ' << __FILE__ << ':' << __LINE__ << "> " << endl; \
        anson::Utils::print(__VA_ARGS__); \
  } \
} while(0)

#else
#define anprint(v, ...) ((void)0)
#endif

/**
* @brief
* Log at level 1 (enabled if ANSON_VERBOSE >= 1)
*/
#define anerror(...)   anprint(LOG_ERROR,  __VA_ARGS__)

/**
* @brief
* Log at level 2 (enabled if ANSON_VERBOSE >= 2)
*/
#define anwarn(...)    anprint(LOG_WARN,   __VA_ARGS__)

/**
* @brief
* Log at level 3 (enabled if ANSON_VERBOSE >= 3)
*/
#define aninfo(...)    anprint(LOG_INFO,   __VA_ARGS__)

/**
* @brief
* Log at level 4 (enabled if ANSON_VERBOSE >= 4)
*/
#define anlog(...)     anprint(LOG_LOG,    __VA_ARGS__)

/**
*
* @brief
* Log at level 4 (enabled if ANSON_VERBOSE >= 5)
*/
#define andebug(...)   anprint(LOG_DEBUG,  __VA_ARGS__)

}
