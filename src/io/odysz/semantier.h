#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>

#include "anson.h"
#include "jprotocol.h"
#include "json.h"

namespace anson {
class EchoReq: public AnsonBody {
public:
    inline static const std::string _type_ = "io.odysz.semantic.jserv.echo.EchoReq";

    struct A {
        inline static const string echo = "echo";
        inline static const string inet = "inet";
    };

    string echo;

    EchoReq() : AnsonBody("na", EchoReq::_type_) {}

    /**
     * Java:
     * @AnsonCtor(initialist="echo(m)", base={"na"})
     *
     * AST.ctors[1]:
     * [["echo", "string", "m"], ["na"]]
     *
     * @brief EchoReq
     * @param m
     */
    EchoReq(string m) : AnsonBody("na", EchoReq::_type_), echo(m) {}
};

inline static void load_usereqAst_ext(AstMap &asts) {
    istringstream ast_is {
        R"({ "type": "io.odysz.anson.AnsonBodyAst",)"
        R"("dataAnclass": "io.odysz.semantic.jserv.echo.UserReq", "baseAnclass": "io.odysz.semantic.jprotocol.AnsonBody",)"
        R"("A": {"test":  "test.only"},)"
        R"("fields" : { "data": {"dataAnclass": "map<string, string"} })"
        R"(})"};

    load_msg_specialAst<UserReq>(asts, ast_is,
      [](meta_factory<UserReq> &entf, AnsonBodyAst *ast) {

        entf.data<&UserReq::data>("data");

        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const UserReq&>(ans);
            if ("data" == fieldname)
            return entt::forward_as_meta(concrete.data);
        }

        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
            AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<UserReq>(): Failed to get entt instance (meta_any)");
        return {};
        };
    });
}

inline static void load_echoAst_ext(AstMap &asts) {
    istringstream ast_is {R"({ "type": "io.odysz.anson.AnsonBodyAst",)"
    R"("dataAnclass": "io.odysz.semantic.jserv.echo.EchoReq", "baseAnclass": "io.odysz.semantic.jprotocol.AnsonBody",)"
    R"("A": {"echo":  "echo", "inet":  "inet"},)"
    R"("fields" : { "echo": {"dataAnclass": "java.lang.String"} })"
    R"(})"};

    load_msg_specialAst<EchoReq>(asts, ast_is,
      [](meta_factory<EchoReq> &entf, AnsonBodyAst *ast) {

        entf.data<&EchoReq::echo>("echo");

        ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
            if (ast->fields.contains(fieldname)) {
                auto& concrete = static_cast<const EchoReq&>(ans);
                if ("echo" == fieldname)
                    return entt::forward_as_meta(concrete.echo);
            }

            if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
                AnsonBodyAst *bast = IJsonable::contxt_ptr->ast<AnsonBodyAst>(ast->baseAnclass);
                return bast->get_field_instance(ans, fieldname);
            }

            anerror("get_field_instance<EchoReq>(): Failed to get entt instance (meta_any)");
            return {};
        };
    });
}
}
