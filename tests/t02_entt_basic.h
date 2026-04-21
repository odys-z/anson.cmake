#pragma once

#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include "io/odysz/anson.h"
#include "io/odysz/utils.h"

namespace anson {
using namespace entt;

class T_List : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.T_List";

    vector<string> val;

    string txt;

    T_List() : Anson(_type_), txt("By Ctor") {  anlog(string_view("T_List Constructor")); }
};

class T_List2D : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.T_List2D";

    vector<T_List> vss;

    T_List2D() : Anson(_type_) { anlog(string_view("T_List2D Constructor")); }
};

class T_List2DPtr : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.T_List2DPtr";

    vector<shared_ptr<T_List>> vpp;

    T_List2DPtr() : Anson(_type_) {}
};

inline static void register_T_List(AstMap &asts) {
    hashed_string enttype;
    string anclass;

    enttype = hashed_string{T_List::_type_.c_str()};
    entt::meta_factory<anson::T_List>()
        .type(T_List::_type_.c_str())
        .base<Anson>()
        .ctor<>()
        .func<+[](const T_List &inst) -> std::shared_ptr<anson::T_List> {
            anlog(std::format("T_List.func<create_ptr>(const inst)"));
            return std::make_shared<anson::T_List>(inst);
        }>("create_ptr")
        .data<&anson::T_List::val, as_ref_t>("val")
        .data<&anson::T_List::txt>("txt")
        ;

    anclass = T_List().anclass;
    AnsonAst *ast = new AnsonAst{anclass, false};
    ast->dataAnclass = T_List::_type_;
    ast->base = Anson::_type_;
    ast->enttypeid = enttype;

    // ast.fields is only used for serialization?
    ast->fields = map<string, AnsonField>{
        {"txt",   {.fieldname="txt", .dataAnclass = "string"}},
        {"val",   {.fieldname="val", .dataAnclass = "list<string"}}
    };

    ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        if (!ast->fields.contains(fieldname))
            return meta_any{false};

        auto& concrete = static_cast<const T_List&>(ans);

        if ("txt" == fieldname)
            return entt::forward_as_meta(concrete.txt);
        if ("val" == fieldname)
            return entt::forward_as_meta(concrete.val);

        anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
        return {};
    };

    asts[anclass] = unique_ptr<AnsonAst>(ast);
}

inline static void register_2Dasts(AstMap &asts) {
    //
    entt::meta_factory<anson::T_List2D>()
        .type(T_List2D::_type_.c_str())
        .base<Anson>()
        .ctor<>()
        .data<&anson::T_List2D::vss>("vss")
        ;

    string anclass = T_List2D().anclass;
    AnsonAst *ast = new AnsonAst{anclass, false};
    ast->dataAnclass = T_List2D::_type_;
    ast->dataBaseAst = AnsonAst::_type_;
    ast->base = Anson::_type_;
    ast->enttypeid = hashed_string{T_List2D::_type_.c_str()};

    ast->fields = map<string, AnsonField>{
        {"vss",   {.fieldname="vss", .dataAnclass = "list<"s + T_List::_type_}},
    };

    ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        if (!ast->fields.contains(fieldname))
            return meta_any{false};

        auto& concrete = static_cast<const T_List2D&>(ans);

        if ("vss" == fieldname)
            return entt::forward_as_meta(concrete.vss);

        anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
        return {};
    };

    asts[ast->dataAnclass] = unique_ptr<AnsonAst>(ast);
}

inline static void register_2DPtr_asts_callback(AstMap &asts) {
    string anclass;
    hashed_string enttype;

    entt::meta_factory<std::shared_ptr<anson::T_List>>()
        .type(("shared_ptr<"s + T_List::_type_).c_str());

    entt::meta_factory<std::vector<std::shared_ptr<anson::T_List>>>()
        .type(("vector<shared_ptr<"s + T_List::_type_).c_str());

    entt::meta_factory<anson::T_List2DPtr>()
        .type(T_List2DPtr::_type_.c_str())
        .base<Anson>()
        .ctor<>()
        .data<&anson::T_List2DPtr::vpp>("vpp")
        ;

    anclass = T_List2DPtr().anclass;
    AnsonAst *ast = new AnsonAst{anclass, false};
    ast->dataAnclass = T_List2DPtr::_type_;
    ast->dataBaseAst = Anson::_type_;
    ast->base = Anson::_type_;
    ast->enttypeid = hashed_string{T_List2DPtr::_type_.c_str()};

    ast->fields = map<string, AnsonField>{
                                          {"vpp",   {.fieldname="vpp", .dataAnclass = "list<shared_ptr<"s + T_List::_type_}},
                                          };

    /* Returned something. Depends on entt reflection or Anson semantics generation?
    ast->get_entt_instance = [](IJsonable& inst, id_type key) -> meta_any {
        auto& concrete = static_cast<T_List2DPtr&>(inst);

        auto type = entt::resolve<T_List2DPtr>();

        if (auto data = find_field_recursive(type, key); data) {
            if (!entt::resolve(data.type().id())) {
                anerror("Field type is not registered in EnTT meta system!");
            }
            meta_any d = data.get(concrete);
            if (d)
                return d;
        }
        anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
        return {};
    };
    */

    ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        if (!ast->fields.contains(fieldname))
            return meta_any{false};

        auto& concrete = static_cast<const T_List2DPtr&>(ans);

        if ("vpp" == fieldname)
            return entt::forward_as_meta(concrete.vpp);

        anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
        return {};
    };

    asts[anclass] = unique_ptr<AnsonAst>(ast);
}
}
