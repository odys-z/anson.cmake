/**
 * The eqivalent of gen.antlr.json + JSONAnsonListener
 */
#pragma once

#include <nlohmann/json.hpp>
#include <entt/meta/meta.hpp>
#include <entt/entt.hpp>
#include <entt/meta/container.hpp>
#include <string>
#include <fstream>
#include "anson.h"
#include "jprotocol.h"

namespace anson {

using namespace entt;
using namespace entt::literals;

template <typename AN, typename AST>
inline static AST* createAST(AstMap &asts, const string &base_ast_id,
                             const map<string, AnsonField> fields) {
    const AN anson{};
    AST *ast = new AST(anson.anclass, false);
    string antype = AN::_type_;
    hashed_string enttype{antype.c_str()};

    ast = new AST(antype);
    ast->dataBaseAst = base_ast_id;
    ast->enttypeid = enttype;
    ast->dataAnclass = anson.anclass;
    ast->fields = fields;

    // e.g. asid of AnsonMsg<EchoReq> is "io..AnsonMsgAst<io..AnsonMsg<io..EchoReq"
    // string astid = AST::_type_ + '<' + AN().anclass;
    string astid = AN().anclass;

    anlog(string_view{std::format("create AST: {}", astid)});
    asts[astid] = unique_ptr<AST>(ast);
    return ast;
}

inline static void register_asts(AstMap &asts) {
    hashed_string enttype;
    string anclass;

    // [abstract class without ctors registered]
    enttype = hashed_string(IJsonable::_anclass_.c_str());
    entt::meta_factory<anson::IJsonable>()
        .type(enttype)
        .data<&anson::IJsonable::anclass>("anclass")
        ;

    anclass = IJsonable::_anclass_;
    AnsonAst *ast = new AnsonAst(anclass, false);
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    asts.insert(make_pair(anclass, ast));

    //
    enttype = hashed_string{AnsonField_type.c_str()};
    entt::meta_factory<anson::AnsonField>()
        .type(enttype)
        .data<&anson::AnsonField::fieldname>("fieldname")
        .data<&anson::AnsonField::dataAnclass>("dataAnclass")
        .data<&anson::AnsonField::valType>("valType")
        ;

    ast = new AnsonAst(AnsonField_type);
    ast->base = "";
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    ast->fields = map<string, AnsonField>{
        {"fieldname", {.fieldname="fieldname", .dataAnclass="string"}},
        {"valType", {.fieldname="valType", .dataAnclass="string"}},
        {"dataAnclass", {.fieldname="dataAnclass", .dataAnclass="string"}}
    };

    asts[AnsonField_type] = unique_ptr<AnsonAst>(ast);

    //
    ast = createAST<Anson, AnsonAst>(asts, IJsonable::_anclass_, map<string, AnsonField>{
        // {"type", {.fieldname="type", .dataAnclass="string"}},
    });
    entt::meta_factory<anson::Anson>()
        .type(ast->enttypeid)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::Anson::type>("type")
        ;

    //
    enttype = hashed_string{AnsonAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonAst>()
        .type(enttype)
        .base<Anson>()
        .ctor<>()
        .ctor<std::string, bool>()
        .ctor<std::string, std::string>()
        .data<&anson::AnsonAst::isInt>("isInt")
        .data<&anson::AnsonAst::isDouble>("isDouble")
        .data<&anson::AnsonAst::isEnum>("isEnum")
        .data<&anson::AnsonAst::isList>("isList")
        .data<&anson::AnsonAst::isMap>("isMap")
        .data<&anson::AnsonAst::istring>("istring")
        .data<&anson::AnsonAst::isJsonable>("isJsonable")
        .data<&anson::AnsonAst::isJavaEnum>("isJavaEnum")
        .data<&anson::AnsonAst::enttypeid>("enttypeid")
        .data<&anson::AnsonAst::dataAnclass>("dataAnclass")
        .data<&anson::AnsonAst::fields>("fields")
        .data<&anson::AnsonAst::enums>("enums")
        .data<&anson::AnsonAst::static_val>("static_val")
        .data<&anson::AnsonAst::dataBaseAst>("dataBaseAst")
        .data<&anson::AnsonAst::dataAnclass>("dataAnclass")
        ;

    //
    createAST<AnsonAst, AnsonAst>(asts, "", map<string, AnsonField>{});
    //
    AnsonJavaEnumAst *jeast = createAST<AnsonJavaEnumAst, AnsonJavaEnumAst>(
        asts, AnsonAst::_type_, map<string, AnsonField>{
            {"encode", {.fieldname="encode", .dataAnclass = "map<string, string"}},
            {"decode", {.fieldname="decode", .dataAnclass = "map<string, string"}},
    });
    entt::meta_factory<anson::AnsonJavaEnumAst>()
        .type(jeast->enttypeid)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonJavaEnumAst::encode>("encode")
        .data<&anson::AnsonJavaEnumAst::decode>("decode")
        ;

    //
    AnsonBodyAst *bdast = createAST<AnsonBodyAst, AnsonBodyAst>(asts, AnsonAst::_type_, map<string, AnsonField>{
        {"A",   {.fieldname="A", .dataAnclass = "map<string, string"}}
    });
    // asts[bdast->dataAnclass] = unique_ptr<AnsonAst>(bdast);

    entt::meta_factory<anson::AnsonBodyAst>()
        .type(bdast->enttypeid)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonBodyAst::uri>("uri"_hs, "uri")
        .data<&anson::AnsonBodyAst::A>("A"_hs, "A")
        ;

    //
    AnsonMsgAst *msgast = createAST<AnsonMsgAst, AnsonMsgAst>(asts, Anson::_type_, map<string, AnsonField>{
        {"bodyAnclass", {.fieldname="bodyAnclass", .dataAnclass = "string"}},
        {"bodyAst", {.fieldname="bodyAst", .dataAnclass = "string"}},
        {"portAnclass", {.fieldname="portAnclass", .dataAnclass = "string"}},
        {"portAst", {.fieldname="portAst", .dataAnclass = "string"}}
    });

    entt::meta_factory<anson::AnsonMsgAst>()
        .type(msgast->enttypeid)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonMsgAst::bodyAnclass>("bodyAnclass")
        .data<&anson::AnsonMsgAst::bodyAst>("bodyAst")
        .data<&anson::AnsonMsgAst::portAnclass>("portAnclass")
        .data<&anson::AnsonMsgAst::portAst>("portAst")
        ;
}

inline static void register_msg(AstMap &asts) {
    //
    // string anclass = SemanticObject().anclass;
    // hashed_string enttype{anclass.c_str()};
    AnsonAst *ast = createAST<SemanticObject, AnsonAst>(asts, Anson::_type_, map<string, AnsonField>{
        {"data", {.fieldname="data", .dataAnclass = "map<string, TODO"}}
    });
    entt::meta_factory<anson::SemanticObject>()
        .type(ast->enttypeid)
        .ctor<>()
        .base<anson::Anson>()
        .data<&anson::SemanticObject::data>("data")
        ;

    //
    ast = createAST<AnsonBody, AnsonBodyAst>(asts, Anson::_type_, map<string, AnsonField>{
        {"uri", {.fieldname="uri", .dataAnclass = "string"}},
        {"a", {.fieldname="a", .dataAnclass = "string"}}
    });

    ast->get_field_instance
        = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {

        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const AnsonBody&>(ans);
            if ("a" == fieldname)
                return entt::forward_as_meta(concrete.a);
            else if ("uri" == fieldname)
                return entt::forward_as_meta(concrete.uri);
        }

        if (IJsonable::contxt_ptr->has_ast(ast->dataBaseAst)) {
            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->dataBaseAst);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<AnsonBody>(): Failed to get entt instance (meta_any)");
        return {};
    };

    entt::meta_factory<anson::AnsonBody>()
        .type(ast->enttypeid)
        .base<anson::Anson>()
        .ctor()
        .ctor<string>()
        .ctor<string, string>()
        .data<&anson::AnsonBody::a>("a")
        ;

    //
    ast = createAST<AnsonResp, AnsonBodyAst>(asts, AnsonBody::_type_, map<string, AnsonField>{
        {"m", {.fieldname="m", .dataAnclass = "string"}},
        {"rs", {.fieldname="rs", .dataAnclass = AnResultset::_anclass_}},
        {"map", {.fieldname="map", .dataAnclass = SemanticObject::_anclass_}}
    });
    entt::meta_factory<anson::AnsonResp>()
        .type(ast->enttypeid)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::AnsonResp::m>("m")
        .data<&anson::AnsonResp::rs>("rs")
        .data<&anson::AnsonResp::map>("map")
        ;
}

inline static void register_port(AstMap &asts, const string &port_ast_pth) {
    std::ifstream ifstream(port_ast_pth);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", port_ast_pth)));
    }

    AnsonJavaEnumAst *portAst = new AnsonJavaEnumAst{};
    portAst->dataAnclass = Port::_type_;
    portAst->isJavaEnum = true;

    // portAst = loadAST("");

    EnTTSaxParser handler(*portAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        string anclass = portAst->dataAnclass; // Port::_type_
        hashed_string enttype = hashed_string{anclass.c_str()};

        // meta_type portype =
        entt::meta_factory<anson::Port>()
            .type(enttype)
            .base<JavaEnum>()
            .ctor<>()
            .ctor<std::string>()
            ;

        // for field in portAst.fields
        //    type.data<Port::type.name()>();

        portAst->enttypeid = enttype;

        asts[anclass] = unique_ptr<AnsonJavaEnumAst>(portAst);
    }
    else
        anerror(string_view(std::format("Could not load AST from {}!", port_ast_pth)));
}

template<typename T>
inline static void specialize_req(AstMap &asts, const AnsonBodyAst *body_ast) {
    AnsonMsg<T> msg_echoreq;
    string anclass = msg_echoreq.anclass;
    hashed_string enttype{anclass.c_str()};

    entt::meta_factory<anson::AnsonMsg<T>>()
        .type(anclass.c_str())
        .template ctor<>()
        .template ctor<anson::Port>()
        .template base<anson::Anson>()
        .template data<&anson::AnsonMsg<T>::port>("port")
        .template data<&anson::AnsonMsg<T>::body>("body");

    AnsonMsgAst *ast = new AnsonMsgAst(anclass);
    ast->dataBaseAst = Anson::_type_;
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;

    ast->fields = map<string, AnsonField>{
        {"port", {.fieldname = "port", .dataAnclass=Port::_type_}},
        {"body", {.fieldname = "body", .dataAnclass="list<shared_ptr<"s + T::_type_}}
    };

    ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        auto& concrete = static_cast<const AnsonMsg<T>&>(ans);
        if ("port" == fieldname)
            return entt::forward_as_meta(concrete.port);
        else if ("body" == fieldname)
            return entt::forward_as_meta(concrete.body);

        anerror(std::format("get_entt_instance<AnsonMsg{}>(): Failed to get entt instance (meta_any)", T::_type_));
        return {};
    };

    entt::meta_factory<std::vector<std::shared_ptr<T>>>()
        .type(hashed_string{(string("list<shared_ptr<") + T::_type_ + ">>").c_str()})
        ;

    /**
     * 1. Cast the base interface to the known concrete type
     * 2. Resolve the meta type for the concrete class
     * 3. Get the field data and return the value from this instance
     */
    // ast->get_entt_instance = [](IJsonable& inst, id_type key) -> meta_any {
    //     auto& concrete = static_cast<anson::AnsonMsg<T>&>(inst);

    //     auto type = entt::resolve<anson::AnsonMsg<T>>();

    //     if (auto data = find_field_recursive(type, key); data) {
    //         meta_any d = data.get(concrete);
    //         if (d)
    //         return d;
    //     }
    //     anerror("get_entt_instance(): Failed to get entt instance (meta_any)");
    //     return {};
    // };

    asts[anclass] = unique_ptr<AnsonMsgAst>(ast);
}

template <typename Rq>
inline static void load_msg_specialAst(AstMap &asts, string ast_pth,
                   std::function<void(meta_factory<Rq>&, AnsonBodyAst *ast)> registerBodyFields) {
    AnsonBodyAst *bodyAst = new AnsonBodyAst{};
    EnTTSaxParser handler(*bodyAst, IJsonable::contxt_ptr);

    std::ifstream ifstream(ast_pth);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", ast_pth)));
    }

    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        // andebug(bodyAst->fields);
        for (auto& [fn, f] : bodyAst->fields)
            if (LangExt::isblank(f.fieldname) || LangExt::isblank(f.dataAnclass)) {
                anwarn(std::format("Error fields configuration in {} : {} (fieldname: {}, dataAnclass: {})",
                                   ast_pth, fn, f.dataAnclass, f.fieldname));
                if (LangExt::isblank(f.fieldname))
                    f.fieldname = fn;
            }

        string anclass = bodyAst->dataAnclass;
        hashed_string enttype = hashed_string{anclass.c_str()};

        meta_factory<Rq> protype =
        entt::meta_factory<Rq>()
            .type(enttype)
            .template base<AnsonBody>()
            .template ctor<>()
            .template ctor<string>()
            .func<+[](const Rq &inst) -> std::shared_ptr<Rq> {
                andebug(string_view(std::format("{}.func<create_ptr>(const inst)", Rq::_type_)));
                return std::make_shared<Rq>(inst);
            }>("create_ptr")
            ;

        bodyAst->enttypeid = enttype;
        registerBodyFields(protype, bodyAst);

        asts[anclass] = unique_ptr<AnsonBodyAst>(bodyAst);

        specialize_req<Rq>(asts, bodyAst);
    }
    else
        anerror(string_view(std::format("Could not load AST from {}!", ast_pth)));
}
}
