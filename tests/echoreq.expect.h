#pragma once

#include "io/odysz/anson.h"
#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"
#include "io/odysz/semantier.h"

namespace anson{

/**
 * The test version of load_usereqAst_ext().
 *
 * @brief load_usereqAst_test
 * @param asts
 * @param ast_path
 */
inline static void load_usereqAst_test(JsonOpt* ctx, const string &ast_path) {
    specialize_msg_astpth<UserReq, AnsonBody>(ctx, ast_path,
      [ctx](meta_factory<UserReq> &entf, AnsonBodyAst *ast) {

        entf.data<&UserReq::data>("data");

        ast->get_field_instance = [ast, ctx](const IJsonable& ans, const string& fieldname) -> meta_any {
        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const UserReq&>(ans);
            if ("data" == fieldname)
            return entt::forward_as_meta(concrete.data);
        }

        if (ctx->has_ast(ast->baseAnclass)) {
            AnsonBodyAst *bast = ctx->ast<AnsonBodyAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<UserReq>(): Failed to get entt instance (meta_any)");
        return {};
        };
    });
}

/**
 * Ast configurable version of the load_echoAst_ext().
 *
 * @brief load_echoAst_test
 * @param asts AST Pool
 * @param ast_path ast json path
 */
inline static void load_echoAst_test(JsonOpt* ctx, const string &ast_path) {
    specialize_msg_astpth<EchoReq, AnsonBody>(ctx, ast_path,
      [ctx](meta_factory<EchoReq> &entf, AnsonBodyAst *ast) {

        entf.data<&EchoReq::echo>("echo");

        ast->get_field_instance = [ast, ctx](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const EchoReq&>(ans);
                if ("echo" == fieldname)
                    return entt::forward_as_meta(concrete.echo);
            }

            if (ctx->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = ctx->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<EchoReq>(): Failed to get entt instance (meta_any)");
            return {};
        };
    });
}
}
