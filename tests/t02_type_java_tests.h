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
        {"mapArr2", {.dataAnclass="map<string, list<VarType"}}
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

        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<AnsMap>(): Failed to get entt instance (meta_any): "s + fieldname);
        return {};
    };

    return ast;
}

class AnsTStrsList : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.AnsTStrsList";

    vector<vector<string>> lst;

    vector<vector<vector<LangExt::VarType>>> lst3d;
    vector<vector<vector<vector<Anson*>>>> dim4;

    AnsTStrsList() : Anson(_type_) {
    }

    AnsTStrsList(string lst_0,
        vector<LangExt::VarType> r0, vector<LangExt::VarType> r1, vector<LangExt::VarType> r2)
        : Anson(_type_), lst({vector<string>{lst_0}}),
          lst3d({vector<vector<LangExt::VarType>>{r0, r1, r2},
                 vector<vector<LangExt::VarType>>{r2, r1, r0}}) {
    }

};

inline static AnsonAst* register_AnsTStrsList(AstMap & asts) {
    AnsonAst *ast = createAST<AnsTStrsList, AnsonAst>(asts, Anson::_type_,
                    map<string, AnsonField>{
                        {"lst", {.dataAnclass="list<list<string"}},
                        {"lst3d", {.dataAnclass="list<list<list<LangExt::VarType"}},
                        {"dim4", {.dataAnclass="list<list<list<list<*" + Anson::_type_}}
                    });

    entt::meta_factory<anson::AnsTStrsList>()
        .type(ast->enttypeid)
        .ctor<>()
        .base<anson::Anson>()
        // .data<&anson::AnsTStrsList::lst, entt::as_ref_t>("lst")
        // .data<&anson::AnsTStrsList::lst3d, as_ref_t>("lst3d")
        // .data<&anson::AnsTStrsList::dim4, as_ref_t>("dim4")
        .data<&anson::AnsTStrsList::lst>("lst")
        .data<&anson::AnsTStrsList::lst3d>("lst3d")
        .data<&anson::AnsTStrsList::dim4>("dim4")
        ;

    ast->get_field_instance
        = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {

        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const AnsTStrsList&>(ans);
            if ("lst" == fieldname)
                return entt::forward_as_meta(concrete.lst);
            else if ("lst3d" == fieldname)
                return entt::forward_as_meta(concrete.lst3d);
            else if ("dim4" == fieldname)
                return entt::forward_as_meta(concrete.dim4);
        }

        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<AnsTStrsList>(): Failed to get entt instance (meta_any): "s + fieldname);
        return {};
    };

    return ast;
}

class T_PhotoCSS : public Anson {
public:
    inline static const string _type_ = "io.oz.album.tier.T_PhotoCSS";

    vector<int> size;

    T_PhotoCSS() : Anson(_type_) {
        size = vector<int> {};
    }

    T_PhotoCSS(int w, int h) : Anson(_type_) {
        size = vector<int> {w, h};
    }
};

// MEMO: There are the .data() callings unable to generalized, but can be handled with callbacks.
//
// template <typename D, typename DBase>
// inline static optional<AnsonAst*> register_ast(AstMap & asts, string ast_pth) {
//     optional<AnsonAst*> _ast = load_ast(asts, ast_pth);
//     if (_ast) {
//         AnsonAst *ast = _ast.value();

//         ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
//             auto& concrete = static_cast<const T_PhotoCSS&>(ans);
//             if ("size" == fieldname)
//                 return entt::forward_as_meta(concrete.size);

//             if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
//                 AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
//                 return bast->get_field_instance(ans, fieldname);
//             }

//             anerror(std::format("get_entt_instance<T_PhotoCSS>(): Failed to get entt field instance: {}",
//                         T_PhotoCSS::_type_, fieldname));
//             return {};
//         };

//         string anclass = ast->dataAnclass;
//         hashed_string enttype = hashed_string{anclass.c_str()};

//         meta_factory<T_PhotoCSS> protype =
//         entt::meta_factory<T_PhotoCSS>()
//             .type(enttype)
//             // .template base<DBase>()
//             // .template ctor<>()
//             // .template ctor<string>()
//             .base<Anson>()
//             .ctor<>()
//             .data<&T_PhotoCSS::size>("size")
//             .func<+[](const T_PhotoCSS &inst) -> std::shared_ptr<T_PhotoCSS> {
//                 andebug(std::format("{}.func<create_ptr>(const inst)", inst.anclass));
//                 return std::make_shared<T_PhotoCSS>(inst);
//             }>("create_ptr")
//         ;

//         ast->enttypeid = enttype;
//     }
//     return _ast;
// }
}
