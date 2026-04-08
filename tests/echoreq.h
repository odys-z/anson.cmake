#pragma once

#include "io/odysz/anson.h"
#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"

namespace anson{

inline static void load_echoAst(AstMap &asts, string ast_path) {
    hashed_string enttype{EchoReq::_type_.c_str()};
    entt::meta_factory<anson::EchoReq>()
        .type(EchoReq::_type_.c_str())
        .base<AnsonBody>()
        .ctor<>()
        .ctor<string>()
        .data<&anson::EchoReq::echo>("echo"_hs, "echo")
        ;

    AnsonBodyAst *echoAst = new AnsonBodyAst{};
    echoAst->dataAnclass = EchoReq::_type_;
    EnTTSaxParser handler(*echoAst, IJsonable::contxt_ptr);

    std::ifstream ifstream(ast_path);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", ast_path)));
    }

    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        string anclass = echoAst->dataAnclass;
        hashed_string enttype = hashed_string{anclass.c_str()};

        // meta_type portype =
        entt::meta_factory<anson::EchoReq>()
            .type(enttype)
            .base<AnsonBody>()
            .ctor<>()
            .ctor<string>()
            // .data<&EchoReq::a>("a")
            .data<&EchoReq::echo>("echo")
            .func<+[](const EchoReq &inst) -> std::shared_ptr<anson::EchoReq> {
                andebug("EchoReq.func<create_ptr>(const inst)");
                return std::make_shared<anson::EchoReq>(inst);
            }>("create_ptr")
            ;

        // for field in portAst.fields
        //    type.data<field.name().for_class>();

        echoAst->enttypeid = enttype;
        echoAst->dataBaseAst = AnsonBodyAst::_type_;
        asts[anclass] = unique_ptr<AnsonBodyAst>(echoAst);

        specialize_req<EchoReq>(asts, echoAst);
    }
    else
        anerror(string_view(std::format("Could not load AST from {}!", ast_path)));
}

template <typename Rq>
inline static void load_msg_specialAst(AstMap &asts, string ast_pth,
                                       std::function<void(meta_factory<Rq>)> registerFields) {
    // hashed_string enttype{Rq::_type_.c_str()};
    // entt::meta_factory<anson::EchoReq>()
    //     .type(EchoReq::_type_.c_str())
    //     .base<AnsonBody>()
    //     .ctor<>()
    //     .ctor<string>()
    //     .data<&anson::EchoReq::echo>("echo"_hs, "echo")
    //     ;

    AnsonBodyAst *bodyAst = new AnsonBodyAst{};
    // bodyAst->dataAnclass = Rq::_type_;
    EnTTSaxParser handler(*bodyAst, IJsonable::contxt_ptr);

    std::ifstream ifstream(ast_pth);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", ast_pth)));
    }

    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        string anclass = bodyAst->dataAnclass;
        hashed_string enttype = hashed_string{anclass.c_str()};

        meta_factory<Rq> protype =
        entt::meta_factory<Rq>()
            .type(enttype)
            .template base<AnsonBody>()
            .template ctor<>()
            .template ctor<string>()
            // .data<&EchoReq::echo>("echo")
            .func<+[](const Rq &inst) -> std::shared_ptr<Rq> {
                andebug(string_view(std::format("{}.func<create_ptr>(const inst)", Rq::_type_)));
                return std::make_shared<Rq>(inst);
            }>("create_ptr")
            ;

        registerFields(protype);
        // for field in portAst.fields
        //    type.data<field.name().for_class>();
        // e. g. protype.data<&EchoReq::echo>("echo")

        bodyAst->enttypeid = enttype;
        bodyAst->dataBaseAst = AnsonBodyAst::_type_;
        asts[anclass] = unique_ptr<AnsonBodyAst>(bodyAst);

        specialize_req<EchoReq>(asts, bodyAst);
    }
    else
        anerror(string_view(std::format("Could not load AST from {}!", ast_pth)));
}
}
