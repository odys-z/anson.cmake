/**
 * The eqivalent of gen.antlr.json + JSONAnsonListener
 */
#pragma once

#include <nlohmann/json.hpp>
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>
#include <entt/meta/container.hpp>
#include <string>
#include "json.h"
#include "semantier.h"

namespace anson {
using namespace entt;
using namespace entt::literals;

inline static void setup_jserv_crud_deprecated(AstMap &asts) {
    setup_msg_specialAst<AnQueryReq, AnsonBody>(asts,
        std::format(R"({{"type": "{}")", AnsonBodyAst::_type_) +
        std::format(R"("base": "{}")", AnsonAst::_type_) +
        R"("A": {"echo":  "echo", "inet":  "inet"},)" +
        std::format(R"("dataAnclass": "{}",)", AnQueryReq::_type_) +
        std::format(R"("dataBaseAst": "{}")", AnsonBody::_type_) +
        R"("fields" : {{ "echo": {"dataAnclass": "string"}} }})"
        R"(}})",

        [](meta_factory<AnQueryReq> &entf, AnsonBodyAst *ast) {

          entf.data<&AnQueryReq::exprs, as_ref_t>("exprs");

          ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const AnQueryReq&>(ans);
                if ("exprs" == fieldname)
                    return entt::forward_as_meta(concrete.exprs);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<AnQueryReq>(): Failed to get entt instance (meta_any)");
            return {};
          };
        });
}

inline static void setup_jserv_crud(AstMap &asts) {
    AnsonBodyAst *bdast = createAST<AnQueryReq, AnsonBodyAst>(asts, AnsonBody::_type_,
        map<string, AnsonField>{
            /**Main table */
            {"mtabl", {.dataAnclass="string"}},
            /**Main table alias*/
            {"mAlias", {.dataAnclass="string"}},

            /**
             * <pre>
             * joins: [join-obj],
             * join-obj: [{t: "j/R/l", tabl: "table-1", as: "t_alais", on: conds}]
             * conds: [cond-obj]
             * cond-obj: {(main-table | alais.)left-col-val op (table-1 | alias2 .)right-col-val}
             * op: '=' | '&lt;=' | '&gt;=' ...</pre>
             */
            {"joins", {.dataAnclass="list<list<string"}},

            /**
             * exprs: [expr-obj],
             * expr-obj: {tabl: "b_articles/t_alais", alais: "recId", expr: "recId"}
             */
            {"exprs", {.dataAnclass="list<list<string"}},

            /**where: [cond-obj], see {@link #joins}for cond-obj.*/
            {"where", {.dataAnclass="list<list<string"}},

            /**
             * orders: [order-obj],
             * - order-obj: {tabl: "b_articles", field: "pubDate", asc: "true"}
             */
            {"orders", {.dataAnclass="list<list<string"}},

            /**
             * group: [group-obj]
             * - group-obj: {tabl: "b_articles/t_alais", expr: "recId" }
             */
            {"groups", {.dataAnclass="list<string"}},

            {"page", {.dataAnclass="int"}},
            {"pgsize", {.dataAnclass="int"}},
            {"limt", {.dataAnclass="list<string"}},
            {"havings", {.dataAnclass="list<list<string"}}
        });

    body_specialize_msg<AnQueryReq, AnsonBody>(asts, bdast,
      [](meta_factory<AnQueryReq> &entf, AnsonBodyAst *ast) {
        entf.data<&AnQueryReq::mtabl>("mtabl")
            .data<&AnQueryReq::mAlias>("mAlias")
            .data<&AnQueryReq::joins>("joins")
            .data<&AnQueryReq::exprs>("exprs")
            .data<&AnQueryReq::where>("where")
            .data<&AnQueryReq::orders>("orders")
            .data<&AnQueryReq::groups>("groups")
            .data<&AnQueryReq::page>("page")
            .data<&AnQueryReq::pgsize>("pgsize")
            .data<&AnQueryReq::limt>("limt")
            .data<&AnQueryReq::havings>("havings")
            ;

        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const AnQueryReq&>(ans);
                if ("mtabl" == fieldname)
                    return entt::forward_as_meta(concrete.mtabl);
                if ("mAlias" == fieldname)
                    return entt::forward_as_meta(concrete.mAlias);
                if ("joins" == fieldname)
                    return entt::forward_as_meta(concrete.joins);
                if ("exprs" == fieldname)
                    return entt::forward_as_meta(concrete.exprs);
                if ("where" == fieldname)
                    return entt::forward_as_meta(concrete.where);
                if ("orders" == fieldname)
                    return entt::forward_as_meta(concrete.orders);
                if ("groups" == fieldname)
                    return entt::forward_as_meta(concrete.groups);
                if ("page" == fieldname)
                    return entt::forward_as_meta(concrete.page);
                if ("pgsize" == fieldname)
                    return entt::forward_as_meta(concrete.pgsize);
                if ("limt" == fieldname)
                    return entt::forward_as_meta(concrete.limt);
                if ("havings" == fieldname)
                    return entt::forward_as_meta(concrete.havings);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<UserReq>(): Failed to get entt instance (meta_any)");
            return {};
        };
    });

    //
    bdast = createAST<AnUpdateReq, AnsonBodyAst>(
        asts, AnsonBody::_type_,
        map<string, AnsonField>{
            /**Main table */
            {"mtabl", {.dataAnclass="string"}},

            /**
             * nvs: [nv-obj],
             * nv-obj: {n: "roleName", v: "admin"}
             * ArrayList<Object[]> nvs;
             */
            {"nvs", {.dataAnclass="list<list<VarType"}},

            /**
             * inserting values, used for "I". 3d array [[[n, v], ...]]
             * protected ArrayList<ArrayList<Object[]>> nvss;
             */
            {"nvss", {.dataAnclass="list<list<list<VarType"}},

            /**
             * Inserting columns, used for "I".
             * Here a col shouldn't be an expression - so not Object[], unlike that of query.
             * protected String[] cols;
             */
            {"cols", {.dataAnclass="list<list<string"}},

            /**
             * where: [cond-obj], see {@link #joins}for cond-obj.
             * ArrayList<Object[]> where;
             */
            {"where", {.dataAnclass="list<list<VarTypae>"}},
            {"limt", {.dataAnclass="string"}},

            // TODO ISSUE: currently no polymorphism can be supported while deserialization
            {"postUpds", {.dataAnclass="list<"s + AnUpdateReq::_type_}},
            {"attacheds", {.dataAnclass="list<list<VarType"}}
    });

    body_specialize_msg<AnUpdateReq, AnsonBody>(asts, bdast,
      [](meta_factory<AnUpdateReq> &entf, AnsonBodyAst *ast) {
        entf.data<&AnUpdateReq::mtabl>("mtabl")
            .data<&AnUpdateReq::nvs>("nvs")
            .data<&AnUpdateReq::nvss>("nvss")
            .data<&AnUpdateReq::cols>("cols")
            .data<&AnUpdateReq::where>("where")
            .data<&AnUpdateReq::limt>("limt")
            .data<&AnUpdateReq::postUpds>("postUpds")
            .data<&AnUpdateReq::attacheds>("attacheds")
            ;

        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const AnUpdateReq&>(ans);
                if ("mtabl" == fieldname)
                    return entt::forward_as_meta(concrete.mtabl);
                if ("nvs" == fieldname)
                    return entt::forward_as_meta(concrete.nvs);
                if ("nvss" == fieldname)
                    return entt::forward_as_meta(concrete.nvss);
                if ("cols" == fieldname)
                    return entt::forward_as_meta(concrete.cols);
                if ("where" == fieldname)
                    return entt::forward_as_meta(concrete.where);
                if ("limt" == fieldname)
                    return entt::forward_as_meta(concrete.limt);
                if ("postUpds" == fieldname)
                    return entt::forward_as_meta(concrete.postUpds);
                if ("attacheds" == fieldname)
                    return entt::forward_as_meta(concrete.attacheds);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<UserReq>(): Failed to get entt instance (meta_any)");
            return {};
        };
    });

    bdast = createAST<AnInsertReq, AnsonBodyAst>(
        asts, AnUpdateReq::_type_,
        map<string, AnsonField>{});

    body_specialize_msg<AnInsertReq, AnUpdateReq>(asts, bdast,
      [](meta_factory<AnInsertReq> &entf, AnsonBodyAst *ast) {

        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<UserReq>(): Failed to get entt instance (meta_any)");
            return {};
        };
    });
}

inline static void register_peersettings(AstMap &asts) {
    //
    AnsonAst *ast = createAST<PeerSettings, AnsonAst>(
        asts, Anson::_type_, map<string, AnsonField>{
        {"ansonMsg",  {.dataAnclass = "string"}},
        {"ansons",    {.dataAnclass = "list<string"}},
        {"scopeEnums",{.dataAnclass = "list<string"}},
        {"javaEnums", {.dataAnclass = "list<string"}},
        {"ansonBody", {.dataAnclass = "string"}},
        {"anRequests",{.dataAnclass = "list<string"}},
        {"cpp_gen",   {.dataAnclass = "string"}}
    });

    entt::meta_factory<anson::PeerSettings>()
        .type(ast->enttypeid)
        .base<Anson>()
        .ctor<>()
        .data<&anson::PeerSettings::ansons>("ansons")
        .data<&anson::PeerSettings::scopeEnums>("scopeEnums")
        .data<&anson::PeerSettings::javaEnums>("javaEnums")
        .data<&anson::PeerSettings::ansonMsg>("ansonMsg")
        .data<&anson::PeerSettings::ansonBody>("ansonBody")
        .data<&anson::PeerSettings::anRequests>("anRequests")
        .data<&anson::PeerSettings::cpp_gen>("cpp_gen")
        ;
}

inline static void register_jserv(AstMap &asts, JsonOpt &ctx_opt) {
    IJsonable::contxt_ptr = &ctx_opt;

    register_varctors();
    register_asts(asts);
    register_msgs(asts);
    register_enums<MsgCode>(asts);
    register_port(asts);
    specialize_respmsg(asts);
    setup_jserv_crud(asts);
    load_usereqAst_ext(asts);
    load_echoAst_ext(asts);
    register_peersettings(asts);
}

}

