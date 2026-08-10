#pragma once

#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>

#include <io/odysz/anson.h>
#include <io/odysz/jprotocol.h>
#include <io/odysz/entt_jserv.h>
#include <io/odysz/module/rs.h>



namespace anson {

class HeartBeat : public anson::AnsonBody {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jsession.HeartBeat";

    struct A {
    };
    string ssid;
    string uid;

    HeartBeat(string clienturi, string ssid, string uid) : AnsonBody(Port::heartbeat), ssid(ssid), uid(uid) {
        Type(_type_);
        uri = clienturi;
    }

    // No default ctor is not found. Force a compile error here: io.odysz.semantic.jsession.HeartBeat HeartBeat ()
    HeartBeat() : AnsonBody() { Type(_type_); }
};

inline static void load_heartbeatAst(JsonOpt* ctx, const string &ast_path) {
    specialize_msg_astpth<HeartBeat, AnsonBody>(ctx, ast_path,
      [ctx](meta_factory<HeartBeat> &entf, AnsonBodyAst *ast) {
        entf.data<&HeartBeat::ssid>("ssid");
        entf.data<&HeartBeat::uid>("uid");

        //
        ast->get_field_instance = [ast, ctx](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const HeartBeat&>(ans);
                if ("ssid" == fieldname)
                    return entt::forward_as_meta(concrete.ssid);
                if ("uid" == fieldname)
                    return entt::forward_as_meta(concrete.uid);
            }

            if (ctx->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = ctx->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<HeartBeat>(): Failed to get entt instance (meta_any)");
            return { };
        };
    });
}

class AnSessionReq : public anson::AnsonBody {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jsession.AnSessionReq";

    struct A {
        inline static const string login = "login";
        inline static const string logout = "logout";
        inline static const string pswd = "pswd";
        inline static const string init = "init";
        inline static const string touch = "touch";
        inline static const string ping = "ping";
    };
    string uid;
    string token;
    string iv;
    string deviceId;

    AnSessionReq() : AnsonBody() {
        Type(_type_);
    }
};

inline static void load_ansessionreqAst(JsonOpt* ctx, const string &ast_path) {
    specialize_msg_astpth<AnSessionReq, AnsonBody>(ctx, ast_path,
      [ctx](meta_factory<AnSessionReq> &entf, AnsonBodyAst *ast) {
        entf.data<&AnSessionReq::uid>("uid");
        entf.data<&AnSessionReq::token>("token");
        entf.data<&AnSessionReq::iv>("iv");
        entf.data<&AnSessionReq::deviceId>("deviceId");
        entf.ctor<>();

        //
        ast->get_field_instance = [ast, ctx](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const AnSessionReq&>(ans);
                if ("uid" == fieldname)
                    return entt::forward_as_meta(concrete.uid);
                if ("token" == fieldname)
                    return entt::forward_as_meta(concrete.token);
                if ("iv" == fieldname)
                    return entt::forward_as_meta(concrete.iv);
                if ("deviceId" == fieldname)
                    return entt::forward_as_meta(concrete.deviceId);
            }

            if (ctx->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = ctx->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<AnSessionReq>(): Failed to get entt instance (meta_any)");
            return { };
        };
    });
}

class AnSessionResp : public anson::AnsonResp {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jsession.AnSessionResp";

    struct A {
    };
    SessionInf ssInf;
    Anson profile;

    AnSessionResp(string ssid, string uid, string roleId) : AnsonResp() {
        Type(_type_);
        ssInf.ssid = ssid;
        ssInf.uid = uid;
        ssInf.roleId = roleId;
    }

    AnSessionResp(SessionInf ss_inf) : ssInf(ss_inf) {
        Type(_type_);
    }

    AnSessionResp() : AnSessionResp("", "", "") {
    }
};

inline static void load_ansessionrespAst(JsonOpt* ctx, const string &ast_path) {
    specialize_msg_astpth<AnSessionResp, AnsonResp>(ctx, ast_path,
      [ctx](meta_factory<AnSessionResp> &entf, AnsonBodyAst *ast) {
        entf.data<&AnSessionResp::ssInf>("ssInf");
        entf.data<&AnSessionResp::profile>("profile");
        entf.ctor<string, string, string>();
        entf.ctor<SessionInf>();
        entf.ctor<>();

        //
        ast->get_field_instance = [ast, ctx](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const AnSessionResp&>(ans);
                if ("ssInf" == fieldname)
                    return entt::forward_as_meta(concrete.ssInf);
                if ("profile" == fieldname)
                    return entt::forward_as_meta(concrete.profile);
            }

            if (ctx->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = ctx->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<AnSessionResp>(): Failed to get entt instance (meta_any)");
            return { };
        };
    });
}

inline static void register_semantier(JsonOpt* ctx, const string &ast_folder) {
    filesystem::path folder_path{ast_folder};
    load_heartbeatAst(ctx, (folder_path / "heartbeat.ast.json").string());
    load_ansessionreqAst(ctx, (folder_path / "session-req.ast.json").string());
    load_ansessionrespAst(ctx, (folder_path / "session-resp.ast.json").string());
}

}
