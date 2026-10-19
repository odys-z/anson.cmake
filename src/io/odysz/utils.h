#pragma once

#include <functional>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <optional>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <format>

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

    inline static std::filesystem::path safeAbsolute(const std::filesystem::path& p);

    inline static std::string timestamp() {
        using namespace std::chrono;

        auto now = system_clock::now();
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        std::time_t t = system_clock::to_time_t(now);
        std::tm tm{};
#if defined(_WIN32)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
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
        std::cout << "[" << anson::Utils::timestamp() << "] <verbose " << v << '/' << ANSON_VERBOSE << ' ' << __FILE__ << ':' << __LINE__ << "> " << endl; \
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

inline std::filesystem::path Utils::safeAbsolute(const std::filesystem::path& p) {
    try {
        return std::filesystem::absolute(p);
    }
    catch (const std::filesystem::filesystem_error& e) {
        anerror("Filesystem error: "s + e.what());
    }
    catch (const std::system_error& e) {
        anerror("System error: "s + e.what());
    }
    catch (const std::exception& e) {
        anerror("Standard exception: "s + e.what());
    }
    catch (...) {
        anerror("Unknown exception while converting to absolute path\n");
    }
    return {}; // fallback: empty path
}

/**
 * The C++ counterpart of io.odysz.common.EnvPath (semantic.transact), with a fixed set of names.
 *
 * Accepted, on Linux and Windows alike:
 *   ~              at the start of a path, alone or before '/' or '\': the user's home;
 *   $HOME          the user's home, same as ~;
 *   $VOLUME_HOME   EnvPath::volume_home(path) if set by the app, else the environment variable VOLUME_HOME.
 *
 * The user's home is USERPROFILE (then HOME) on Windows, HOME on Linux.
 * Any other $NAME is not a variable here and is kept as is. A known variable without a value is
 * also kept as is, with a warning, so the unresolved name shows up wherever the path is reported.
 * Values have '\' turned into '/', as EnvPath does with FilenameUtils.winpath2unix().
 * Input and output are UTF-8; on Windows nothing goes through the ANSI code page.
 */
class EnvPath {
public:
    inline static const std::string VOLUME_HOME = "VOLUME_HOME";
    inline static const std::string HOME = "HOME";

    /** Sets $VOLUME_HOME, taking precedence over the environment variable. */
    inline static void volume_home(const std::string& utf8_path) { _volume() = utf8_path; }

    inline static std::optional<std::string> volume_home() {
        if (_volume() && !_volume()->empty()) return _volume();
        return getEnv(VOLUME_HOME.c_str());
    }

    /** The user's home folder, UTF-8. */
    inline static std::optional<std::string> home() {
#ifdef _WIN32
        if (auto p = getEnv("USERPROFILE")) return p;
#endif
        return getEnv("HOME");
    }

    /** An environment variable as UTF-8; nullopt if not set or empty. Names are ASCII. */
    inline static std::optional<std::string> getEnv(const char* name) {
#ifdef _WIN32
        // _wgetenv, not _wdupenv_s: MinGW's msvcrt.dll (vcpkg x64-mingw-*) doesn't export the latter.
        std::wstring wname(name, name + std::char_traits<char>::length(name));
#ifdef _MSC_VER
#pragma warning(suppress: 4996)
#endif
        const wchar_t* buf = _wgetenv(wname.c_str());
        if (buf == nullptr || *buf == L'\0') return std::nullopt;
        std::wstring w(buf);
        std::u8string u = std::filesystem::path(w).u8string();
        return std::string(u.begin(), u.end());
#else
        const char* v = std::getenv(name);
        if (v == nullptr || *v == '\0') return std::nullopt;
        return std::string(v);
#endif
    }

    /**
     * Replaces $HOME, $VOLUME_HOME and a leading ~. See the class comment.
     * @param src UTF-8 path, e.g. "$VOLUME_HOME/syntity.json", "~/.anclient/settings.json"
     * @return UTF-8 path
     */
    inline static std::string replaceEnv(const std::string& src) {
        auto isw = [](char c) { return std::isalnum((unsigned char)c) || c == '_'; };

        std::string out;
        out.reserve(src.size());
        for (size_t i = 0; i < src.size();) {
            if (src[i] != '$') { out += src[i++]; continue; }

            size_t j = i + 1;
            while (j < src.size() && isw(src[j])) j++;
            std::string name = src.substr(i + 1, j - i - 1);

            std::optional<std::string> v;
            if (name == HOME) v = home();
            else if (name == VOLUME_HOME) v = volume_home();
            else { out.append(src, i, j - i); i = j; continue; }    // not a variable here

            if (v) out += unixsep(*v);
            else {
                anwarn(std::format("EnvPath: ${} has no value, kept as is: {}", name, src));
                out.append(src, i, j - i);
            }
            i = j;
        }

        if (!out.empty() && out[0] == '~' && (out.size() == 1 || out[1] == '/' || out[1] == '\\')) {
            if (auto h = home()) {
                std::string hm = unixsep(*h);
                while (hm.size() > 1 && hm.back() == '/') hm.pop_back();
                out = hm + out.substr(1);
            }
            else anwarn(std::format("EnvPath: no home folder for ~, kept as is: {}", src));
        }
        return out;
    }

private:
    inline static std::optional<std::string>& _volume() {
        static std::optional<std::string> v;
        return v;
    }

    inline static std::string unixsep(std::string s) {
        std::replace(s.begin(), s.end(), '\\', '/');
        return s;
    }
};

/**
 * Resolves ~, $HOME and $VOLUME_HOME in a UTF-8 path, see EnvPath.
 * (Moved here from Anclient example.slint, ipcagent_manager.h.)
inline std::string resolveHomePath(const std::string& inputPath) {
    return EnvPath::replaceEnv(inputPath);
}
 */

}
