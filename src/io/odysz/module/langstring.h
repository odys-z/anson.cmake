#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>



// actually this is auto generated, wit modification of global <langs> and operator""_ans.
namespace anson {

class LangString : public Anson {
public:
    inline static const std::string _type_ = "io.odysz.module.LangString";
    string lang;
    map<string, string> en;
    map<string, string> local;

    LangString() : Anson() {
        Type(_type_);
    }

    inline static void loadLangs(string str_res_json, const JsonOpt* ctx); // { Anson::from_file(str_res_json, langs); }
};

inline static void register_langstringAst(const JsonOpt* ctx) {

    AnsonAst * ast = createAST <LangString, AnsonAst> (
        *ctx->asts, Anson::_type_, map <string, AnsonField> {
        {"lang", {.dataAnclass="string"} },
        {"en", {.dataAnclass="map<string, string"} },
        {"local", {.dataAnclass="map<string, string"} },
       });

    entt::meta_factory <anson::LangString> ()
        .type(ast->enttypeid)
        .base<Anson>()

        .data<&anson::LangString::lang>("lang")
        .data<&anson::LangString::en>("en")
        .data<&anson::LangString::local>("local")
        ;

        //
        ast->get_field_instance = [ast, ctx](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const LangString&>(ans);
                if ("lang" == fieldname)
                    return entt::forward_as_meta(concrete.lang);
                if ("en" == fieldname)
                    return entt::forward_as_meta(concrete.en);
                if ("local" == fieldname)
                    return entt::forward_as_meta(concrete.local);
            }

            if (ctx->has_ast(ast->baseAnclass)) {
                AnsonAst *bast = ctx->ast<AnsonAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<StringValues>(): Failed to get entt instance (meta_any)");
            return { };
        };
}

inline static LangString langs;

inline void LangString::loadLangs(string str_res_json, const JsonOpt* ctx) {
    Anson::from_file(str_res_json, langs, ctx);
}

inline std::string operator""_ans(const char* str, std::size_t len) {
    if (langs.en.size() == 0 && langs.local.size() == 0) {
        anerror("String values of languages are not loaded yet.");
        return str;
    }

    std::string key(str, len);
    if (langs.local.contains(key)) {
        auto it = langs.local.find(key);
        return it->second;
    }

    auto it = langs.en.find(key);
    return it != langs.en.end() ? it->second : key;
}

}
