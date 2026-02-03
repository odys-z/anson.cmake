#pragma once
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>

using namespace  std ;

namespace anson {


class JsonOpt;// : public Anson { };

class IJsonable {

public:
    virtual IJsonable* toBlock(ostream& os, JsonOpt& opts);

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
    virtual IJsonable* toJson(string& buf);
};

/**
 * @brief The Anson class
 * java type: io.odysz.anson.Anson
 */
class Anson {
public:
    std::string type;

    Anson() { cout << "defalut contructor" << endl ; }
    Anson(string t) : type(t) { cout << "override contructor, type = " << t << endl ; }

};

class SemanticObject : public Anson {

};
}

