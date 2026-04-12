#pragma once

#include "io/odysz/anson.h"
#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"

namespace anson{

inline static void load_usereqAst(AstMap &asts, const string &ast_path) {
    load_msg_specialAst<UserReq>(asts, ast_path,
      [](meta_factory<UserReq> &entf, AnsonBodyAst *ast) {

        entf.data<&UserReq::data>("data");

        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const UserReq&>(ans);
            if ("data" == fieldname)
            return entt::forward_as_meta(concrete.data);
        }

        if (IJsonable::contxt_ptr->has_ast(ast->dataBaseAst)) {
            // AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->dataBaseAst);
            AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->dataBaseAst);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<UserReq>(): Failed to get entt instance (meta_any)");
        return {};
        };
    });
}

inline static void load_echoAst(AstMap &asts, const string &ast_path) {
    load_msg_specialAst<EchoReq>(asts, ast_path, // "ast/echo.ast.json",
      [](meta_factory<EchoReq> &entf, AnsonBodyAst *ast) {

        entf.data<&EchoReq::echo>("echo");

        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const EchoReq&>(ans);
                if ("echo" == fieldname)
                    return entt::forward_as_meta(concrete.echo);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->dataBaseAst)) {
                // AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->dataBaseAst);
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->dataBaseAst);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<EchoReq>(): Failed to get entt instance (meta_any)");
            return {};
        };
    });
}
}
