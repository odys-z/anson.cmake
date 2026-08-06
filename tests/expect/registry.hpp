#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>
#include <io/odysz/module/rs.h>



namespace anson {

class SynOrg : public anson::Anson {
public:
    inline static const std::string _type_ = "io.oz.syn.registry.SynOrg";
    string orgId;
    string orgName;
    string orgType;
    string parent;
    string sort;
    string fullpath;
    string webroot;
    string homepage;
    string album0;

    SynOrg() : Anson() {
        Type(_type_);
    }
};

inline static void register_synorgAst(AstMap & asts) {

    AnsonAst * ast = createAST <SynOrg, AnsonAst> (
        asts, Anson::_type_, map <string, AnsonField> {
        {"orgId", {.dataAnclass="string"} },
        {"orgName", {.dataAnclass="string"} },
        {"orgType", {.dataAnclass="string"} },
        {"parent", {.dataAnclass="string"} },
        {"sort", {.dataAnclass="string"} },
        {"fullpath", {.dataAnclass="string"} },
        {"webroot", {.dataAnclass="string"} },
        {"homepage", {.dataAnclass="string"} },
        {"album0", {.dataAnclass="string"} },
       });

    entt::meta_factory <anson::SynOrg> ()
        .type(ast->enttypeid)
        .base<Anson>()

        .data<&anson::SynOrg::orgId>("orgId")
        .data<&anson::SynOrg::orgName>("orgName")
        .data<&anson::SynOrg::orgType>("orgType")
        .data<&anson::SynOrg::parent>("parent")
        .data<&anson::SynOrg::sort>("sort")
        .data<&anson::SynOrg::fullpath>("fullpath")
        .data<&anson::SynOrg::webroot>("webroot")
        .data<&anson::SynOrg::homepage>("homepage")
        .data<&anson::SynOrg::album0>("album0")
        ;

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const SynOrg&>(ans);
                if ("orgId" == fieldname)
                    return entt::forward_as_meta(concrete.orgId);
                if ("orgName" == fieldname)
                    return entt::forward_as_meta(concrete.orgName);
                if ("orgType" == fieldname)
                    return entt::forward_as_meta(concrete.orgType);
                if ("parent" == fieldname)
                    return entt::forward_as_meta(concrete.parent);
                if ("sort" == fieldname)
                    return entt::forward_as_meta(concrete.sort);
                if ("fullpath" == fieldname)
                    return entt::forward_as_meta(concrete.fullpath);
                if ("webroot" == fieldname)
                    return entt::forward_as_meta(concrete.webroot);
                if ("homepage" == fieldname)
                    return entt::forward_as_meta(concrete.homepage);
                if ("album0" == fieldname)
                    return entt::forward_as_meta(concrete.album0);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<SynOrg>(): Failed to get entt instance (meta_any)");
            return { };
        };
}

class Synode : public anson::Anson {
public:
    inline static const std::string _type_ = "io.oz.syn.Synode";
    string org;
    string domain;
    string synid;
    string mac;
    string stat;
    string remarks;
    string jserv;
    string optime;
    string oper;
    long nyquence;
    string synuid;

    Synode() : Anson() {
        Type(_type_);
    }

    Synode(const string& synid, const string& synuid, const string& orgid, const string& domain, const string& remark_hub) : Anson(), synid(synid), synuid(synuid), org(orgid), domain(domain), remarks(remark_hub) {
        Type(_type_);
    }
};

inline static void register_synodeAst(AstMap & asts) {

    AnsonAst * ast = createAST <Synode, AnsonAst> (
        asts, Anson::_type_, map <string, AnsonField> {
        {"org", {.dataAnclass="string"} },
        {"domain", {.dataAnclass="string"} },
        {"synid", {.dataAnclass="string"} },
        {"mac", {.dataAnclass="string"} },
        {"stat", {.dataAnclass="string"} },
        {"remarks", {.dataAnclass="string"} },
        {"jserv", {.dataAnclass="string"} },
        {"optime", {.dataAnclass="string"} },
        {"oper", {.dataAnclass="string"} },
        {"nyquence", {.dataAnclass="long"} },
        {"synuid", {.dataAnclass="string"} },
       });

    entt::meta_factory <anson::Synode> ()
        .type(ast->enttypeid)
        .base<Anson>()
        .ctor<>()
        .ctor<const string&, const string&, const string&, const string&, const string&>()

        .data<&anson::Synode::org>("org")
        .data<&anson::Synode::domain>("domain")
        .data<&anson::Synode::synid>("synid")
        .data<&anson::Synode::mac>("mac")
        .data<&anson::Synode::stat>("stat")
        .data<&anson::Synode::remarks>("remarks")
        .data<&anson::Synode::jserv>("jserv")
        .data<&anson::Synode::optime>("optime")
        .data<&anson::Synode::oper>("oper")
        .data<&anson::Synode::nyquence>("nyquence")
        .data<&anson::Synode::synuid>("synuid")
        ;

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const Synode&>(ans);
                if ("org" == fieldname)
                    return entt::forward_as_meta(concrete.org);
                if ("domain" == fieldname)
                    return entt::forward_as_meta(concrete.domain);
                if ("synid" == fieldname)
                    return entt::forward_as_meta(concrete.synid);
                if ("mac" == fieldname)
                    return entt::forward_as_meta(concrete.mac);
                if ("stat" == fieldname)
                    return entt::forward_as_meta(concrete.stat);
                if ("remarks" == fieldname)
                    return entt::forward_as_meta(concrete.remarks);
                if ("jserv" == fieldname)
                    return entt::forward_as_meta(concrete.jserv);
                if ("optime" == fieldname)
                    return entt::forward_as_meta(concrete.optime);
                if ("oper" == fieldname)
                    return entt::forward_as_meta(concrete.oper);
                if ("nyquence" == fieldname)
                    return entt::forward_as_meta(concrete.nyquence);
                if ("synuid" == fieldname)
                    return entt::forward_as_meta(concrete.synuid);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<Synode>(): Failed to get entt instance (meta_any)");
            return { };
        };
}

class SynodeConfig : public anson::Anson {
public:
    inline static const std::string _type_ = "io.oz.syn.registry.SynodeConfig";
    string domain;
    string synid;
    string sysconn;
    string synconn;
    SynOrg org;
    vector<Synode> peers;
    string mode;
    bool https;
    string admin;
    float syncIns;
    int chsize;

    SynodeConfig() : Anson() {
        Type(_type_);
    }

    SynodeConfig(const string& synid, const string& mode) : Anson(), synid(synid), mode(mode) {
        Type(_type_);
    }
};

inline static void register_synodeconfigAst(AstMap & asts) {

    AnsonAst * ast = createAST <SynodeConfig, AnsonAst> (
        asts, Anson::_type_, map <string, AnsonField> {
        {"domain", {.dataAnclass="string"} },
        {"synid", {.dataAnclass="string"} },
        {"sysconn", {.dataAnclass="string"} },
        {"synconn", {.dataAnclass="string"} },
        {"org", {.dataAnclass="io.oz.syn.registry.SynOrg"} },
        {"peers", {.dataAnclass="list<io.oz.syn.Synode"} },
        {"mode", {.dataAnclass="string"} },
        {"https", {.dataAnclass="bool"} },
        {"admin", {.dataAnclass="string"} },
        {"syncIns", {.dataAnclass="float"} },
        {"chsize", {.dataAnclass="int"} },
       });

    entt::meta_factory <anson::SynodeConfig> ()
        .type(ast->enttypeid)
        .base<Anson>()
        .ctor<>()
        .ctor<const string&, const string&>()

        .data<&anson::SynodeConfig::domain>("domain")
        .data<&anson::SynodeConfig::synid>("synid")
        .data<&anson::SynodeConfig::sysconn>("sysconn")
        .data<&anson::SynodeConfig::synconn>("synconn")
        .data<&anson::SynodeConfig::org>("org")
        .data<&anson::SynodeConfig::peers>("peers")
        .data<&anson::SynodeConfig::mode>("mode")
        .data<&anson::SynodeConfig::https>("https")
        .data<&anson::SynodeConfig::admin>("admin")
        .data<&anson::SynodeConfig::syncIns>("syncIns")
        .data<&anson::SynodeConfig::chsize>("chsize")
        ;

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const SynodeConfig&>(ans);
                if ("domain" == fieldname)
                    return entt::forward_as_meta(concrete.domain);
                if ("synid" == fieldname)
                    return entt::forward_as_meta(concrete.synid);
                if ("sysconn" == fieldname)
                    return entt::forward_as_meta(concrete.sysconn);
                if ("synconn" == fieldname)
                    return entt::forward_as_meta(concrete.synconn);
                if ("org" == fieldname)
                    return entt::forward_as_meta(concrete.org);
                if ("peers" == fieldname)
                    return entt::forward_as_meta(concrete.peers);
                if ("mode" == fieldname)
                    return entt::forward_as_meta(concrete.mode);
                if ("https" == fieldname)
                    return entt::forward_as_meta(concrete.https);
                if ("admin" == fieldname)
                    return entt::forward_as_meta(concrete.admin);
                if ("syncIns" == fieldname)
                    return entt::forward_as_meta(concrete.syncIns);
                if ("chsize" == fieldname)
                    return entt::forward_as_meta(concrete.chsize);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<SynodeConfig>(): Failed to get entt instance (meta_any)");
            return { };
        };
}

class Centralport : public anson::JavaEnum {
public:
    inline static const std::string _type_ = "io.oz.syn.registry.Centralport";

    inline static const string heartbeat = "ping.serv";
    inline static const string session = "login.serv";
    inline static const string regist = "regist.serv";
    inline static const string menu = "menu.serv";

    Centralport() : JavaEnum(_type_, "_sentinel_") {
    }

    Centralport(const string& enumval) : JavaEnum(_type_, enumval) {
    }
};


class RegistReq : public anson::UserReq {
public:
    inline static const std::string _type_ = "io.oz.syn.registry.RegistReq";

    struct A {
        inline static const string queryDomx = "r/domx";
        inline static const string queryDomConfig = "r/dom-config";
        inline static const string registDom = "c/domx";
        inline static const string updateDom = "u/domx";
        inline static const string submitSettings = "u/settings";
    };
    string market;
    string protocolPath;
    SynodeConfig diction;
    JServUrl myjserv;
    string mystate;

    RegistReq(const JServUrl& jserv, const SynodeConfig& diction) : UserReq(), myjserv(jserv), diction(diction) {
        Type(_type_);
    }

    // No default ctor is not found. Force a compile error here: io.oz.syn.registry.RegistReq RegistReq ()
    RegistReq() : UserReq() { Type(_type_); }
};

inline static void load_registreqAst(AstMap &asts, const string &ast_path) {
    specialize_msg_astpth<RegistReq, UserReq>(asts, ast_path,
      [](meta_factory<RegistReq> &entf, AnsonBodyAst *ast) {
        entf.data<&RegistReq::market>("market");
        entf.data<&RegistReq::protocolPath>("protocolPath");
        entf.data<&RegistReq::diction>("diction");
        entf.data<&RegistReq::myjserv>("myjserv");
        entf.data<&RegistReq::mystate>("mystate");

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const RegistReq&>(ans);
                if ("market" == fieldname)
                    return entt::forward_as_meta(concrete.market);
                if ("protocolPath" == fieldname)
                    return entt::forward_as_meta(concrete.protocolPath);
                if ("diction" == fieldname)
                    return entt::forward_as_meta(concrete.diction);
                if ("myjserv" == fieldname)
                    return entt::forward_as_meta(concrete.myjserv);
                if ("mystate" == fieldname)
                    return entt::forward_as_meta(concrete.mystate);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<RegistReq>(): Failed to get entt instance (meta_any)");
            return { };
        };
    });
}

class RegistResp : public anson::AnsonResp {
public:
    inline static const std::string _type_ = "io.oz.syn.registry.RegistResp";

    struct A {
    };
    string r;
    SynodeConfig diction;
    vector<string> orgDomains;

    RegistResp() : AnsonResp() {
        Type(_type_);
    }

    RegistResp(const SynodeConfig& diction) : AnsonResp(), diction(diction) {
        Type(_type_);
    }
};

inline static void load_registrespAst(AstMap &asts, const string &ast_path) {
    specialize_msg_astpth<RegistResp, AnsonResp>(asts, ast_path,
      [](meta_factory<RegistResp> &entf, AnsonBodyAst *ast) {
        entf.data<&RegistResp::r>("r");
        entf.data<&RegistResp::diction>("diction");
        entf.data<&RegistResp::orgDomains>("orgDomains");

        //
        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const RegistResp&>(ans);
                if ("r" == fieldname)
                    return entt::forward_as_meta(concrete.r);
                if ("diction" == fieldname)
                    return entt::forward_as_meta(concrete.diction);
                if ("orgDomains" == fieldname)
                    return entt::forward_as_meta(concrete.orgDomains);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<RegistResp>(): Failed to get entt instance (meta_any)");
            return { };
        };
    });
}

inline static void register_centralclientier(AstMap &asts, const string &ast_folder) {
    filesystem::path folder_path{ast_folder};
    register_synorgAst(asts);
    register_synodeAst(asts);
    register_synodeconfigAst(asts);
    register_iport<Centralport>(asts, (folder_path / "centralport.ast.json").string());
    load_registreqAst(asts, (folder_path / "registreq.ast.json").string());
    load_registrespAst(asts, (folder_path / "registresp.ast.json").string());
}

}
