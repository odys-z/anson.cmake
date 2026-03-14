#pragma once

#include <functional>
#include <string>
#include <iostream>

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
    template <typename Range>
    inline static ostream& print(ostream& oss, Range list, string head = "", string sep = ",", string foot = "") {
        return print(oss, list, {.head = head, .sep = sep, .foot = foot});
    }

    template <typename Range>
    inline static ostream& print(ostream& oss, Range list, const PrintFormat& f = {}) {
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
    inline static void print(Range list, string head = "", string sep = ",", string foot = "") {
        print(cout, list, head, sep, foot);
    }

    template <typename Range>
    inline static void print(Range list, const PrintFormat& f) {
        print(cout, list, f);
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
