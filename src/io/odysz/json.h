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
#include "anserializer.h"

namespace anson {
inline static int cnt = 0;
using namespace entt;
using namespace entt::literals;

inline static optional<AnsonAst*> load_ast(AstMap & asts, string ast_pth) {
    std::ifstream iss(ast_pth);
    if (!iss.is_open()) {
        anerror(string_view(std::format("Can not open the file {}! ", ast_pth)));
        return nullptr;
    }
    AnsonAst *ast = new AnsonAst{};
    EnTTSaxParser handler(*ast, IJsonable::contxt_ptr);

    bool result = nlohmann::json::sax_parse(iss, &handler);

    if (!result) {
        anerror(std::format("AST parsing failed. file{}).", ast_pth));
        return nullptr;
    }
    else if (asts.contains(ast->dataAnclass)) {
        anwarn(std::format("AST {} already exists. Ingore updating for avoiding unique-ptr deletion error (Only happens with heavy template usage?).",
                           ast->dataAnclass));
        return nullptr;
    }
    else {
        for (auto& [fn, f] : ast->fields)
            if (!LangExt::isblank(f.fieldname) && f.fieldname != fn || LangExt::isblank(f.dataAnclass)) {
                anwarn(std::format("Error fields configuration : {} (fieldname: {}, dataAnclass: {})",
                                   fn, f.fieldname, f.dataAnclass));
                if (LangExt::isblank(f.fieldname))
                    f.fieldname = fn;
            }

        andebug(std::format("AST loaded: {}, field size: {}", ast->dataAnclass, ast->fields.size()));
        asts[ast->dataAnclass] = unique_ptr<AnsonAst>(ast);
        return ast;
    }
}

template <typename AN, typename AST>
inline static AST* createAST(AstMap &asts, const string &base_ast_id,
                             map<string, AnsonField> fields) {
    const AN anson{};
    AST *ast = new AST(anson.anclass, false);
    string antype = AN::_type_;
    hashed_string enttype{antype.c_str()};

    ast = new AST(antype);
    ast->baseAnclass = base_ast_id;
    ast->enttypeid = enttype;
    ast->dataAnclass = anson.anclass;

    for (auto& [fn, f] : fields) {
        if (fn != f.fieldname && !LangExt::isblank(f.fieldname))
            anwarn(std::format("Override field {}.{} -> {}", ast->dataAnclass, f.fieldname, fn));
        f.fieldname = string{fn};
    }
    ast->fields = fields;

    string astid = AN().anclass;

    andebug(string_view{std::format("create AST: {}", astid)});
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
    ast = createAST<Anson, AnsonAst>(asts, IJsonable::_anclass_, map<string, AnsonField>{});

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
        .data<&anson::AnsonAst::isPortEnum>("isJavaEnum")
        .data<&anson::AnsonAst::enttypeid>("enttypeid")
        .data<&anson::AnsonAst::dataAnclass>("dataAnclass")
        .data<&anson::AnsonAst::fields>("fields")
        .data<&anson::AnsonAst::enums>("enums")
        .data<&anson::AnsonAst::static_val>("static_val")
        .data<&anson::AnsonAst::baseAnclass>("baseAnclass")
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

template<typename T>
inline static void specialize_req(AstMap &asts, const AnsonBodyAst *body_ast) {
    AnsonMsg<T> msg_req;
    string anclass = msg_req.anclass;
    hashed_string enttype{anclass.c_str()};

    entt::meta_factory<anson::AnsonMsg<T>>()
        .type(anclass.c_str())
        .template ctor<>()
        .template ctor<anson::Port>()
        .template base<anson::Anson>()
        .template data<&anson::AnsonMsg<T>::port>("port")
        .template data<&anson::AnsonMsg<T>::code>("code")
        .template data<&anson::AnsonMsg<T>::body>("body");

    AnsonMsgAst *ast = new AnsonMsgAst(anclass);
    ast->baseAnclass = Anson::_type_;
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;

    ast->fields = map<string, AnsonField>{
        {"port", {.fieldname = "port", .dataAnclass=Port::_type_}},
        {"code", {.fieldname = "code", .dataAnclass=MsgCode::_type_}},
        {"body", {.fieldname = "body", .dataAnclass="list<shared_ptr<"s + T::_type_}}
    };

    ast->get_field_instance = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {
        auto& concrete = static_cast<const AnsonMsg<T>&>(ans);
        if ("port" == fieldname)
            return entt::forward_as_meta(concrete.port);
        else if ("code" == fieldname)
            return entt::forward_as_meta(concrete.code);
        else if ("body" == fieldname)
            return entt::forward_as_meta(concrete.body);

        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror(std::format("get_entt_instance<AnsonMsg{}>(): Failed to get entt field instance: {}",
                            T::_type_, fieldname));
        return {};
    };

    entt::meta_factory<std::vector<std::shared_ptr<T>>>()
        .type(hashed_string{(string("list<shared_ptr<") + T::_type_ + ">>").c_str()})
        ;

    asts[anclass] = unique_ptr<AnsonMsgAst>(ast);
}

inline static void specialize_respmsg(AstMap & asts) {
    //
    AnsonBodyAst *ast = createAST<AnsonResp, AnsonBodyAst>(asts, AnsonBody::_type_,
        map<string, AnsonField>{
            {"m", {.fieldname="m", .dataAnclass = "string"}},
            {"rs", {.fieldname="rs", .dataAnclass = "list<"s + AnResultset::_type_}},
            {"map", {.fieldname="map", .dataAnclass = "map<string,string"}}
        });

    entt::meta_factory<anson::AnsonResp>()
        .type(ast->enttypeid)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::AnsonResp::m>("m")
        .data<&anson::AnsonResp::rs>("rs")
        .data<&anson::AnsonResp::map>("map")
        .func<+[](const AnsonResp &inst) -> std::shared_ptr<AnsonResp> {
            andebug(string_view(std::format("{}.func<create_ptr>(const inst)", AnsonResp::_type_)));
            return std::make_shared<AnsonResp>(inst);
        }>("create_ptr")
        ;

    ast->get_field_instance
        = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {

        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const AnsonResp&>(ans);
            if ("m" == fieldname)
                return entt::forward_as_meta(concrete.m);
            else if ("rs" == fieldname)
                return entt::forward_as_meta(concrete.rs);
            else if ("map" == fieldname)
                return entt::forward_as_meta(concrete.map);
        }

        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
            andebug(std::format("------- {} ------- {} ------ {} ",
                                ++cnt, ast->dataAnclass, fieldname));

            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<AnsonResp>(): Failed to get entt instance (meta_any): "s + fieldname);
        return {};
    };

    specialize_req<AnsonResp>(asts, ast);
}

inline static void register_msgs(AstMap &asts) {
    AnsonAst *ast = createAST<SemanticObject, AnsonAst>(asts, Anson::_type_, map<string, AnsonField>{
        {"data", {.fieldname="data", .dataAnclass = "map<string, string"}}
    });
    entt::meta_factory<anson::SemanticObject>()
        .type(ast->enttypeid)
        .ctor<>()
        .base<anson::Anson>()
        .data<&anson::SemanticObject::data>("data")
        ;

    //
    ast = createAST<AnResultset, AnsonAst>(asts, Anson::_type_, map<string, AnsonField>{
        {"columns", {.dataAnclass = "map<string, list<VarType"}},
        {"rows", {.dataAnclass = "list<list<VarType"}}
    });

    entt::meta_factory<anson::AnResultset>()
        .type(ast->enttypeid)
        .ctor<>()
        .base<anson::Anson>()
        .data<&anson::AnResultset::colnames>("columns")
        .data<&anson::AnResultset::rows>("rows")
        ;

    ast->get_field_instance
        = [ast](const IJsonable& ans, const string& fieldname) -> meta_any {

        if (ast->fields.contains(fieldname)) {
            auto& concrete = static_cast<const AnResultset&>(ans);
            if ("columns" == fieldname)
                return entt::forward_as_meta(concrete.colnames);
            else if ("rows" == fieldname)
                return entt::forward_as_meta(concrete.rows);
        }

        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<AnResultset>(): Failed to get entt instance (meta_any): "s + fieldname);
        return {};
    };

    //
    ast = createAST<AnsonBody, AnsonBodyAst>(asts, Anson::_type_, map<string, AnsonField>{
        {"uri", {.dataAnclass = "string"}},
        {"a",   {.dataAnclass = "string"}}
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

        if (IJsonable::contxt_ptr->has_ast(ast->baseAnclass)) {
            AnsonAst *bast = IJsonable::contxt_ptr->ast<AnsonAst>(ast->baseAnclass);
            return bast->get_field_instance(ans, fieldname);
        }

        anerror("get_field_instance<AnsonBody>(): Failed to get entt instance (meta_any): "s + fieldname);
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
}

template<typename C>
inline static void register_enums(AstMap& asts) {
    string anclass = C::_type_;
    using EType = typename C::Code;

    AnsonJavaEnumAst *ast = new AnsonJavaEnumAst(anclass, true);

    hashed_string enttype{anclass.c_str()};
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    anlog(string_view{std::format("create AST: {}", anclass)});

    ast->get_field_instance = [ast](const IJsonable &j, const string &fieldname) -> meta_any {
        for (size_t ix = 0; ix < C::compter; ix++) {
            if (C::noms[ix] == fieldname) {
                return { static_cast<EType>(ix) };
            }
        }
        return { static_cast<EType>(C::compter) }; // sentinel
    };

    ast->name_of = [ast](const meta_any val_inst) -> string {
        if (auto* val = val_inst.try_cast<EType>()) {
            size_t idx = static_cast<size_t>(*val);
            if (idx < C::compter) {
                return string(C::noms[idx]);
            }
        }
        return "null";
    };

    asts[anclass] = unique_ptr<AnsonJavaEnumAst>(ast);
}

inline static void register_port(AstMap &asts, const string &port_ast_pth) {
    std::ifstream ifstream(port_ast_pth);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", port_ast_pth)));
    }

    AnsonJavaEnumAst *portAst = new AnsonJavaEnumAst{};
    portAst->dataAnclass = Port::_type_;
    portAst->isPortEnum = true;

    EnTTSaxParser handler(*portAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        string anclass = portAst->dataAnclass;
        hashed_string enttype = hashed_string{anclass.c_str()};

        entt::meta_factory<anson::Port>()
            .type(enttype)
            .base<JavaEnum>()
            .ctor<>()
            .ctor<std::string>()
            ;

        portAst->enttypeid = enttype;

        asts[anclass] = unique_ptr<AnsonJavaEnumAst>(portAst);
    }
    else
        anerror(string_view(std::format("Could not load AST from {}!", port_ast_pth)));
}

inline static void register_varctors() {
    function<meta_any()> *f;

    f = new function<meta_any()> {[]()->meta_any{ return meta_any{vector<string>{}}; }};
    LangExt::register_ctor("list<string", f);
    f = new function<meta_any()> {[]()->meta_any{ return meta_any{vector<vector<string>>{}}; }};
    LangExt::register_ctor("list<list<string", f);

    f = new function<meta_any()> {[]()->meta_any{ return meta_any{vector<LangExt::VarType>{}}; }};
    LangExt::register_ctor("list<VarType", f);
    LangExt::register_ctor("list<LangExt::VarType", f);

    f = new function<meta_any()> {[]()->meta_any{ return meta_any{vector<vector<LangExt::VarType>>{}}; }};
    LangExt::register_ctor("list<list<VarType", f);
    LangExt::register_ctor("list<list<LangExt::VarType", f);
}

inline static function<shared_ptr<Anson>(const Anson&)> create_ptr = [](const Anson & inst) {
    return make_shared<Anson>(inst);
};

// template <typename BD>
template <typename BD, typename BD_Base>
inline static void body_specialize_msg(AstMap &asts, AnsonBodyAst* bodyAst,
                   const std::function<void(meta_factory<BD>&, AnsonBodyAst *ast)>& registerBodyFields) {
    for (auto& [fn, f] : bodyAst->fields)
        if (!LangExt::isblank(f.fieldname) && f.fieldname != fn || LangExt::isblank(f.dataAnclass)) {
            anwarn(std::format("Error fields configuration : {} (fieldname: {}, dataAnclass: {})",
                               fn, f.dataAnclass, f.fieldname));
            if (LangExt::isblank(f.fieldname))
                f.fieldname = fn;
        }

    // TODO merge with load_ast()?
    string anclass = bodyAst->dataAnclass;
    hashed_string enttype = hashed_string{anclass.c_str()};

    meta_factory<BD> protype =
        entt::meta_factory<BD>()
            .type(enttype)
            .template base<BD_Base>()
            // .template base<AnsonBody>()
            .template ctor<>()
            .template ctor<string>()
            .func<+[](const BD &inst) -> std::shared_ptr<BD> {
                andebug(std::format("{}.func<create_ptr>(const inst)", inst.anclass));
                return std::make_shared<BD>(inst);
            }>("create_ptr")
        ;

    bodyAst->enttypeid = enttype;
    registerBodyFields(protype, bodyAst);

    if (!asts.contains(anclass))
        asts[anclass] = unique_ptr<AnsonBodyAst>(bodyAst);

    specialize_req<BD>(asts, bodyAst);
}

template <typename Rq>
inline static bool load_msg_specialAst(AstMap &asts, std::istream &iss,
                   const std::function<void(meta_factory<Rq>&, AnsonBodyAst *ast)>& registerBodyFields) {
    AnsonBodyAst *bodyAst = new AnsonBodyAst{};
    EnTTSaxParser handler(*bodyAst, IJsonable::contxt_ptr);

    bool result = nlohmann::json::sax_parse(iss, &handler);
    if (result) {
        body_specialize_msg<Rq, AnsonBody>(asts, bodyAst, registerBodyFields);
    }
    return result;
}

template <typename Rq>
inline static bool setup_msg_specialAst(AstMap &asts, const string &ast_json,
                   const std::function<void(meta_factory<Rq>&, AnsonBodyAst *ast)> registerBodyFields) {
    std::istringstream iss(ast_json);
    return load_msg_specialAst<Rq>(asts, iss, registerBodyFields);
}

template <typename Rq>
inline static void specialize_msg_astpth(AstMap &asts, const string &ast_pth,
                   const std::function<void(meta_factory<Rq>&, AnsonBodyAst *ast)> & registerBodyFields) {
    std::ifstream ifstream(ast_pth);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", ast_pth)));
    }
    else if (!load_msg_specialAst<Rq>(asts, ifstream, registerBodyFields))
        anerror(string_view(std::format("Could not load AST from {}!", ast_pth)));
}

}

