#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"

namespace anson {

using namespace std ;
using namespace entt;

class  AnsonAst;
struct AnsonField;

using AstMap = map<string, unique_ptr<AnsonAst>>;

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

    virtual ~AnsonAst() = default;

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
    bool isPortEnum = false;
    bool isVar = false;
    string base = "io.odysz.anson.Anson";

    string baseAnclass;
    /** The target's anson type or data type, e.g. string */
    string dataAnclass;
    AnsonAst& data_anclass(const string & cls) { dataAnclass = cls; return *this; }

    map<string, AnsonField> fields;
    vector<vector<vector<string>>> ctors;
    map<string, int> enums;

    /** Only one static string value is allowed in semantic-* ? */
    string static_val;

    hashed_string enttypeid = hashed_string{_type_.c_str()};

    AnsonAst() : Anson(_type_),
        isInt(false), isDouble(false), isEnum(false), isList(false), isMap(false), istring(false), isJsonable(true), isPortEnum(false) { }

    AnsonAst(string anclass, bool isEnum = false) : Anson(_type_, anclass),
        isInt(false), isDouble(false), isEnum(isEnum), isList(false), isMap(false), istring(false), isJsonable(true), isPortEnum(false) { }

    AnsonAst(string anclass, string type) : Anson(type, anclass),
        isInt(false), isDouble(false), isEnum(false), isList(false), isMap(false), istring(false), isJsonable(true), isPortEnum(false) { }

    std::function<meta_any(IJsonable&, id_type)> get_entt_instance = [](IJsonable& j, id_type t) -> meta_any {
        return {};
    };

    std::function<meta_any(const IJsonable&, const string& fieldname)> get_field_instance =
        [this] (const IJsonable& ans, string fn) -> meta_any{
        if ("type" == fn) {
            auto& concrete = static_cast<const Anson&>(ans);
            return entt::forward_as_meta(concrete.type);
        }
        anwarn(std::format("[WARN] AnsonAst({}): Requring field '{}[{}]', this is supposed to be overriden by AST {}.",
                            this->dataAnclass, ans.anclass, fn, ans.anclass));
        return meta_any{false};
    };

    inline const string find_field_astid(const AstMap* asts, const string & fieldname) const ;
};

const string AnsonAst::find_field_astid(const AstMap* asts, const string & fieldname) const {
    if (fields.contains(fieldname))
        return fields.at(fieldname).dataAnclass;
    else if (asts->contains(baseAnclass)) {
        const AnsonAst *baseAst = asts->at(baseAnclass).get();
        return baseAst->find_field_astid(asts, fieldname);
    }
    return "";
}

class AnsonJavaEnumAst: public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonJavaEnumAst";

    map<string, string> encode;
    map<string, string> decode;

    AnsonJavaEnumAst() : AnsonJavaEnumAst(_type_) {}

    /**
     * @brief AnsonJavaEnumAst
     * @param anclass
     * @param isEnum This parameter is used only for templatized registration, not used.
     * (AST is not a JavaEnum)
     */
    AnsonJavaEnumAst(string anclass, bool isEnum = false) : AnsonAst(anclass, isEnum) {
    }

    std::function<string(const meta_any& val)> name_of =
        [this] (const meta_any& val) -> string{
            anerror("The <funcion>name_of must be overriden by auto-generated code.");
        return "null";
    };

    template <typename C>
    static inline string name(C e) {
        if (IJsonable::contxt_ptr && IJsonable::contxt_ptr->has_ast(C::_type_)) {
            AnsonJavaEnumAst * jeast = IJsonable::contxt_ptr->ast<AnsonJavaEnumAst>(C::_type_);
            return jeast->name_of({e.valeur});
        }
        return "null";
    }
};

inline bool operator==(const anson::JavaEnum& p, const std::string& s) {
    if (IJsonable::contxt_ptr && IJsonable::contxt_ptr->asts->contains(p.anclass)) {
        AnsonJavaEnumAst * ast = dynamic_cast<AnsonJavaEnumAst*>(IJsonable::contxt_ptr->asts->at(p.anclass).get());
        return p.enm == ast->encode[s];
    }
    return false;
}

inline bool operator==(const std::string& s, const anson::JavaEnum& p) {
    return p == s;
}

inline bool operator==(const anson::JavaEnum& p, const anson::JavaEnum& q) {
    return p.enm == q.enm;
}

class AnsonBodyAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonBodyAst";

    // ISSUE not adding uri to fields?
    string uri;
    map<string, string> A;

    AnsonBodyAst() : AnsonAst(_type_, _type_) { }

    AnsonBodyAst(string anclass, bool isEnum = false) : AnsonAst(anclass, _type_) { }
};

class AnsonMsgAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonMsgAst";

    string bodyAnclass;
    string bodyAst;
    string portAnclass;
    string portAst;

    AnsonMsgAst() : AnsonAst(_type_, _type_) { }

    AnsonMsgAst(string anclass, bool isEnum = false) : AnsonAst(anclass, anclass) { }
};

inline JavaEnum:: JavaEnum(string anclass, string e) : enm(std::move(e)), IJsonable(anclass) {
    this->anclass = anclass;
    if (contxt_ptr->asts->contains(anclass)) {
        map<string, string> encode =
            // ISSUE JavaEnum is depending on AnsonJavaEnumAst, is this a design error?
            dynamic_cast<AnsonJavaEnumAst*>(contxt_ptr->asts->at(anclass).get())->encode;
        if (encode.contains(enm)) {
            enm = encode[enm];
            return;
        }
    }
    andebug("JavaEnum: "s + enm);
}

inline string JavaEnum::valof() const {
    if (contxt_ptr->asts->contains(anclass)) {
        map<string, string> decode =
            // ISSUE JavaEnum is depending on AnsonJavaEnumAst, is this a design error?
            dynamic_cast<AnsonJavaEnumAst*>(contxt_ptr->asts->at(anclass).get())->decode;

        if (decode.contains(enm)) {
            return decode[enm];
        }
    }
    return enm;
}

class PeerSettings : public Anson {
public:
    inline static const string _type_ = "io.odysz.semantier.PeerSettings";

    vector<string> ansons;
    vector<string> scopeEnums;
    vector<string> javaEnums;
    string ansonMsg;
    string ansonBody;
    vector<string> ansonMsgs;
    vector<string> anRequests;

    string cpp_gen;

    PeerSettings() : Anson(_type_), cpp_gen("semantier.gen.h") {}
};


}

