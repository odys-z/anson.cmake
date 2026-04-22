#pragma once

#include <map>

#include "io/odysz/common.h"
#include "io/odysz/anson.h"
#include "io/odysz/json.h"

namespace anson {

using namespace std;

class AnsTMap : public Anson {

public:
    inline static const string _type_ = "io.odysz.anson.AnsTMap";

    map<string, string> _map;

    // @AnsonField(valType="[Ljava.lang.Object;")
    map<string, vector<LangExt::VarType>> mapArr;
    map<string, vector<LangExt::VarType>> mapArr2;

    AnsTMap() : Anson(_type_) {
    }

    AnsTMap(map<string, string> mstr, map<string, vector<LangExt::VarType>> marr,
            map<string, vector<LangExt::VarType>> marr2 = {})
        : Anson(_type_), _map(mstr), mapArr(marr), mapArr2(marr2) {
    }
};

inline static AnsonAst* register_AnsTMap(AstMap & asts) {
    AnsonAst *ast = createAST<AnsTMap, AnsonAst>(asts, Anson::_type_, map<string, AnsonField>{
        {"_map", {.dataAnclass="map<string, string"}},
        {"mapArr", {.dataAnclass="map<string, list<LangExt::VarType"}},
                    // .nest_val_ctor=[]() -> meta_any{ return meta_any{vector<LangExt::VarType>{}};
        {"mapArr2", {.dataAnclass="map<string, list<VarType"}}
                    // .nest_val_ctor=[]() -> meta_any{ return meta_any{vector<LangExt::VarType>{}};
    });

    entt::meta_factory<anson::AnsTMap>()
        .type(ast->enttypeid)
        .ctor<>()
        .base<anson::Anson>()
        .data<&anson::AnsTMap::_map>("_map")
        .data<&anson::AnsTMap::mapArr>("mapArr")
        .data<&anson::AnsTMap::mapArr2>("mapArr2")
        ;

    ast->get_field_instance
        = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {

        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const AnsTMap&>(ans);
            if ("_map" == fieldname)
                return entt::forward_as_meta(concrete._map);
            else if ("mapArr" == fieldname)
                return entt::forward_as_meta(concrete.mapArr);
            else if ("mapArr2" == fieldname)
                return entt::forward_as_meta(concrete.mapArr2);
        }

        if (IJsonable::contxt_ptr->has_ast(ast->dataBaseAst)) {
            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->dataBaseAst);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<AnsMap>(): Failed to get entt instance (meta_any): "s + fieldname);
        return {};
    };

    return ast;
}
}
