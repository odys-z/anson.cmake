#pragma once

#include "io/odysz/anson.h"
#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"

namespace anson{

inline static void load_echoAst(AstMap &asts, string ast_path) {
    load_msg_specialAst<EchoReq>(asts, "ast/echo.ast.json",
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

            anerror("get_field_instance(): Failed to get entt instance (meta_any)");
            return {};
        };
    });
}
// inline static void load_echoAst(AstMap &asts, string ast_path) {
//     hashed_string enttype{EchoReq::_type_.c_str()};
//     entt::meta_factory<anson::EchoReq>()
//         .type(EchoReq::_type_.c_str())
//         .base<AnsonBody>()
//         .ctor<>()
//         .ctor<string>()
//         .data<&anson::EchoReq::echo>("echo"_hs, "echo")
//         ;

//     AnsonBodyAst *echoAst = new AnsonBodyAst{};
//     echoAst->dataAnclass = EchoReq::_type_;
//     EnTTSaxParser handler(*echoAst, IJsonable::contxt_ptr);

//     std::ifstream ifstream(ast_path);
//     if (!ifstream.is_open()) {
//         anerror(string_view(std::format("Could not open the file {}! ", ast_path)));
//     }

//     bool result = nlohmann::json::sax_parse(ifstream, &handler);
//     if (result) {
//         string anclass = echoAst->dataAnclass;
//         hashed_string enttype = hashed_string{anclass.c_str()};

//         // meta_type portype =
//         entt::meta_factory<anson::EchoReq>()
//             .type(enttype)
//             .base<AnsonBody>()
//             .ctor<>()
//             .ctor<string>()
//             .data<&EchoReq::echo>("echo")
//             .func<+[](const EchoReq &inst) -> std::shared_ptr<anson::EchoReq> {
//                 andebug("EchoReq.func<create_ptr>(const inst)");
//                 return std::make_shared<anson::EchoReq>(inst);
//             }>("create_ptr")
//             ;

//         // for field in portAst.fields
//         //    type.data<field.name().for_class>();

//         echoAst->enttypeid = enttype;
//         echoAst->dataBaseAst = AnsonBodyAst::_type_;
//         asts[anclass] = unique_ptr<AnsonBodyAst>(echoAst);

//         specialize_req<EchoReq>(asts, echoAst);
//     }
//     else
//         anerror(string_view(std::format("Could not load AST from {}!", ast_path)));
// }

inline static vector<string> to_aststring(const AstMap &asts) {
    vector<string> sv;
    sv.push_back("astid: ast.dataAnclass -> ast.dataBaseAst");
    for (auto& [k, v] : asts) {
        // if (!v) {
        //     sv.push_back("Cannot convert string of "s + k);
        //     continue;
        // }
        // string fields;
        // for (auto& [fn, f] : v->fields)
        //     fields += " " + fn;
        // sv.push_back(std::format("{}: {} -> {} [{}]", k, v->dataAnclass, v->dataBaseAst, fields));
        cout << k << endl;
        cout << "  " << v << endl;
        cout << "  " << v->dataAnclass << endl;
        cout << "  " << v->dataBaseAst << endl;
        sv.push_back(std::format("{}: {} -> {}", k, v->dataAnclass, v->dataBaseAst));
    }
    return sv;
}
}
