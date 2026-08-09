#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>
#include <io/odysz/module/rs.h>



namespace anson {

class AnclientSettings : public anson::Anson {
public:
    inline static const std::string _type_ = "io.odysz.jclient.AnclientSettings";
    string sysuri;
    string synuri;
    string jserv;
    string org;
    string domain;
    string device;
    string admin;
    string domain_token;
    string regiserv;
    string centralPswd;
    string temp_dir;

    AnclientSettings() {
        Type(_type_);
    }
};

inline static void register_anclientsettingsAst(AstMap & asts) {

    AnsonAst * ast = createAST <AnclientSettings, AnsonAst> (
        asts, Anson::_type_, map <string, AnsonField> {
        {"sysuri", {.dataAnclass="string"} },
        {"synuri", {.dataAnclass="string"} },
        {"jserv", {.dataAnclass="string"} },
        {"org", {.dataAnclass="string"} },
        {"domain", {.dataAnclass="string"} },
        {"device", {.dataAnclass="string"} },
        {"admin", {.dataAnclass="string"} },
        {"domain_token", {.dataAnclass="string"} },
        {"regiserv", {.dataAnclass="string"} },
        {"centralPswd", {.dataAnclass="string"} },
        {"temp_dir", {.dataAnclass="string"} },
       });

    entt::meta_factory <anson::AnclientSettings> ()
        .type(ast->enttypeid)
        .base<Anson>()
        .ctor<>()

        .data<&anson::AnclientSettings::sysuri>("sysuri")
        .data<&anson::AnclientSettings::synuri>("synuri")
        .data<&anson::AnclientSettings::jserv>("jserv")
        .data<&anson::AnclientSettings::org>("org")
        .data<&anson::AnclientSettings::domain>("domain")
        .data<&anson::AnclientSettings::device>("device")
        .data<&anson::AnclientSettings::admin>("admin")
        .data<&anson::AnclientSettings::domain_token>("domain_token")
        .data<&anson::AnclientSettings::regiserv>("regiserv")
        .data<&anson::AnclientSettings::centralPswd>("centralPswd")
        .data<&anson::AnclientSettings::temp_dir>("temp_dir")
        ;

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const AnclientSettings&>(ans);
                if ("sysuri" == fieldname)
                    return entt::forward_as_meta(concrete.sysuri);
                if ("synuri" == fieldname)
                    return entt::forward_as_meta(concrete.synuri);
                if ("jserv" == fieldname)
                    return entt::forward_as_meta(concrete.jserv);
                if ("org" == fieldname)
                    return entt::forward_as_meta(concrete.org);
                if ("domain" == fieldname)
                    return entt::forward_as_meta(concrete.domain);
                if ("device" == fieldname)
                    return entt::forward_as_meta(concrete.device);
                if ("admin" == fieldname)
                    return entt::forward_as_meta(concrete.admin);
                if ("domain_token" == fieldname)
                    return entt::forward_as_meta(concrete.domain_token);
                if ("regiserv" == fieldname)
                    return entt::forward_as_meta(concrete.regiserv);
                if ("centralPswd" == fieldname)
                    return entt::forward_as_meta(concrete.centralPswd);
                if ("temp_dir" == fieldname)
                    return entt::forward_as_meta(concrete.temp_dir);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<AnclientSettings>(): Failed to get entt instance (meta_any)");
            return { };
        };
}

class StringValues : public anson::Anson {
public:
    inline static const std::string _type_ = "io.odysz.anson.StringValues";
    string lang;
    map<string, string> en;
    map<string, string> local;

    StringValues() : Anson() {
        Type(_type_);
    }
};

inline static void register_stringvaluesAst(AstMap & asts) {

    AnsonAst * ast = createAST <StringValues, AnsonAst> (
        asts, Anson::_type_, map <string, AnsonField> {
        {"lang", {.dataAnclass="string"} },
        {"en", {.dataAnclass="map<string, string"} },
        {"local", {.dataAnclass="map<string, string"} },
       });

    entt::meta_factory <anson::StringValues> ()
        .type(ast->enttypeid)
        .base<Anson>()

        .data<&anson::StringValues::lang>("lang")
        .data<&anson::StringValues::en>("en")
        .data<&anson::StringValues::local>("local")
        ;

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const StringValues&>(ans);
                if ("lang" == fieldname)
                    return entt::forward_as_meta(concrete.lang);
                if ("en" == fieldname)
                    return entt::forward_as_meta(concrete.en);
                if ("local" == fieldname)
                    return entt::forward_as_meta(concrete.local);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<StringValues>(): Failed to get entt instance (meta_any)");
            return { };
        };
}

inline static void register_anclient_cmake(AstMap &asts, const string &ast_folder) {
    filesystem::path folder_path{ast_folder};
    register_anclientsettingsAst(asts);
    register_stringvaluesAst(asts);
}

}
