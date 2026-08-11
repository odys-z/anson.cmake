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

class Semantics : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.Semantics";

    Semantics() : Anson(_type_) {}
};

class SemanExpr : public Semantics {
public:
    inline static const string _type_ = "io.odysz.anson.SemanExpr";
    /** Semantype { "()", "", "ini" } */
    string stype;
    vector<string> args;
    vector<SemanExpr> semantics;
    string expect_result;

    SemanExpr() : Semantics() { Type(_type_); }
};

class AnCtor : public Semantics {
public:
    inline static const string _type_ = "io.odysz.anson.AnCtor";
    SemanExpr base;
    vector<SemanExpr> args;
    vector<SemanExpr> body;
    string expect_result;

    AnCtor() { Type(_type_); }
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
    /** @deprecated */
    vector<vector<vector<string>>> ctors;
    vector<AnCtor> ctorsemantics;

    map<string, int> enums;

    /** Only one static string value is allowed in semantic-* ? */
    string static_val;

    hashed_string enttypeid = hashed_string{_type_.c_str()};

    AnsonAst(string anclass, bool isEnum = false) : Anson(anclass),
        isInt(false), isDouble(false), isEnum(isEnum), isList(false),
        isMap(false), istring(false), isJsonable(true), isPortEnum(false) { }

    AnsonAst() : AnsonAst(_type_) {}

    std::function<meta_any(IJsonable&, id_type)> get_entt_instance
        = [](IJsonable& j, id_type t) -> meta_any { return {}; };

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
    else if (!LangExt::isblank(baseAnclass) && asts->contains(baseAnclass)) {
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
            // TODO 0.1.1 Can sentinels be serialized as null?
            anerror("The <funcion>name_of must be overriden by auto-generated code.");
            return "null";
        };

    template <typename C>
    static inline string name(const JsonOpt* contxt_ptr, C e) {
        if (contxt_ptr && contxt_ptr->has_ast(C::_type_)) {
            AnsonJavaEnumAst * jeast = contxt_ptr->ast<AnsonJavaEnumAst>(C::_type_);
            return jeast->name_of({e.valeur});
        }
        return "null";
    }
};

class AnsonBodyAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonBodyAst";

    // ISSUE not adding uri to fields?
    string uri;
    map<string, string> A;

    AnsonBodyAst() : AnsonAst(_type_) { }

    AnsonBodyAst(string anclass, bool isEnum = false) : AnsonAst(anclass) { }
};

class AnsonMsgAst : public AnsonAst {
public:
    inline static const string _type_ = "io.odysz.anson.AnsonMsgAst";

    string bodyAnclass;
    string bodyAst;
    string portAnclass;
    string portAst;

    AnsonMsgAst() : AnsonAst(_type_) { }

    AnsonMsgAst(string anclass, bool isEnum = false) : AnsonAst(anclass) { }
};

inline bool JavaEnum::operator==(const std::string& s) const {
    return ast && ((AnsonJavaEnumAst*)ast)->encode[s] == enm;
}

inline JavaEnum::JavaEnum(const AnsonJavaEnumAst* ast, const string &anclass, const string &e_v)
        : IJsonable(), enm(e_v) {
    Anclass(anclass);
    this->anclass = anclass;

    if (!ast)
        anerror("============= c++ Anson Reflect Enforcement ===============\n["s +
                anclass + "] cannot be created with a null ast prointer.");

    if (ast->dataAnclass != anclass)
        anerror("============= c++ Anson Reflect Enforcement ===============\n["s +
                anclass + " != " + anclass);

    this->ast = ast;
    map<string, string> encode = ast->encode;
    if (encode.contains(enm))
        enm = encode[enm];
}

inline string JavaEnum::valof() const {
    if (!ast) {
        anerror("============= c++ Anson Reflect Enforcement ===============\n["s +
                anclass + "] cannot be created with a null ast prointer.");
        return enm;
    }
    if (ast->decode.contains(enm))
        return ast->decode.at(enm);
    return enm;
}

template<typename AST>
inline AST* JsonOpt::ast(const string &astId) const {
    string astid = polymorph(astId);

    auto it = asts->find(astid);
    if (it != asts->end()) {
        return dynamic_cast<AST*>(it->second.get());
    }
    if (!primtypes.contains(astid))
        if (!LangExt::has_ctor(astid))
        anerror(std::format("JsonOpt.ast(): cannot find ast with id: '{}'.", astid));
    return nullptr;
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
