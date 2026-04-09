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

inline static void register_asts(AstMap &asts) {
    hashed_string enttype;
    string anclass;
    //
    enttype = hashed_string{"io.odysz.anson.IJasonable"};
    entt::meta_factory<anson::IJsonable>()
        .type(enttype)
        // .ctor<>()
        // .ctor<const std::string&>()
        .data<&anson::IJsonable::anclass>("anclass")
        ;

    anclass = IJsonable::_anclass_;
    AnsonAst *ast = new AnsonAst(anclass, false);
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    // asts[anclass] = unique_ptr<AnsonAst>(ast);
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
    asts.insert(make_pair(AnsonField_type, ast));

    //
    enttype = hashed_string{Anson::_type_.c_str()};
    entt::meta_factory<anson::Anson>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::Anson::type>("type")
        ;

    anclass = Anson().anclass;
    ast = new AnsonAst{anclass, false};
    ast->base = IJsonable::_anclass_; //"io.odysz.anson.IJsonable";
    ast->dataAnclass = anclass;
    ast->enttypeid = enttype;
    // asts[anclass] = unique_ptr<AnsonAst>(ast);
    asts.insert(make_pair(anclass, ast));


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
        ;

    anclass = AnsonAst().anclass;
    ast = new AnsonAst{anclass};
    ast->dataAnclass = anclass;
    ast->fields = map<std::string, AnsonField>{
        {"isList", {.fieldname="isList", .dataAnclass="boolean"}},
        {"isJavaEnum", {.fieldname="isJavaEnum", .dataAnclass="boolean"}},
        {"dataAnclass", {.fieldname="dataAnclass", .dataAnclass="String"}},
        {"fields", {.fieldname="fields", .dataAnclass="map<string, io.odysz.anson.AnstField"}},
    };
    ast->enttypeid = enttype;
    // asts[anclass] = unique_ptr<AnsonAst>(ast);
    asts.insert(make_pair(anclass, ast));

    //
    string javaenum = AnsonJavaEnumAst().anclass;
    enttype = hashed_string{javaenum.c_str()};
    entt::meta_factory<anson::AnsonJavaEnumAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonJavaEnumAst::dataBaseAst>("dataBaseAst")
        .data<&anson::AnsonJavaEnumAst::dataAnclass>("dataAnclass")
        .data<&anson::AnsonJavaEnumAst::encode>("encode")
        .data<&anson::AnsonJavaEnumAst::decode>("decode")
        ;

    AnsonJavaEnumAst *jeast = new AnsonJavaEnumAst(AnsonJavaEnumAst::_type_);
    jeast->dataAnclass = javaenum;
    jeast->base = JavaEnum::_type_;
    jeast->enttypeid = enttype;

    jeast->fields = map<string, AnsonField>{
        {"encode", {.fieldname="encode", .dataAnclass = "map<string, string"}},
        {"decode", {.fieldname="decode", .dataAnclass = "map<string, string"}},
    };
    asts.insert(make_pair(javaenum, jeast));

    //
    enttype = hashed_string{AnsonBodyAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonBodyAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonBodyAst::uri>("uri"_hs, "uri")
        .data<&anson::AnsonBodyAst::A>("A"_hs, "A")
        ;

    anclass = AnsonBodyAst().anclass;
    AnsonBodyAst *bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->dataAnclass = anclass;
    bdast->enttypeid = enttype;

    bdast->fields = map<string, AnsonField>{
        {"uri", {.fieldname="uri", .dataAnclass = "string"}},
        {"A",   {.fieldname="A", .dataAnclass = "map<string, string"}}
    };
    asts.insert(make_pair(anclass, bdast));

    //
    enttype = hashed_string{AnsonMsgAst::_type_.c_str()};
    entt::meta_factory<anson::AnsonMsgAst>()
        .type(enttype)
        .base<AnsonAst>()
        .ctor<string>()
        .data<&anson::AnsonMsgAst::bodyAnclass>("bodyAnclass")
        .data<&anson::AnsonMsgAst::bodyAst>("bodyAst")
        .data<&anson::AnsonMsgAst::portAnclass>("portAnclass")
        .data<&anson::AnsonMsgAst::portAst>("portAst")
        ;

    anclass = AnsonMsgAst().anclass;
    AnsonMsgAst *msgast = new AnsonMsgAst(anclass);
    msgast->base = AnsonAst::_type_;
    msgast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    msgast->fields = map<string, AnsonField> {
        {"bodyAnclass", {.fieldname="bodyAnclass", .dataAnclass = "string"}},
        {"bodyAst", {.fieldname="bodyAst", .dataAnclass = "string"}},
        {"portAnclass", {.fieldname="portAnclass", .dataAnclass = "string"}},
        {"portAst", {.fieldname="portAst", .dataAnclass = "string"}}
    };
    asts[anclass] = unique_ptr<AnsonMsgAst>(msgast);
}

inline static void register_msg(AstMap &asts) {
    //
    string anclass = SemanticObject().anclass;
    hashed_string enttype{anclass.c_str()};
    entt::meta_factory<anson::SemanticObject>()
        .type(enttype)
        .ctor<>()
        .base<anson::Anson>()
        .data<&anson::SemanticObject::data>("data")
        ;

    AnsonAst *ast = new AnsonAst(anclass);
    ast->base = Anson::_type_;
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;
    ast->fields = map<string, AnsonField> {
        {"data", {.fieldname="data", .dataAnclass = "map<string, TODO"}}
    };
    asts[anclass] = unique_ptr<AnsonAst>(ast);

    //
    enttype = hashed_string{AnsonBody::_type_.c_str()};
    entt::meta_factory<anson::AnsonBody>()
        .type(enttype)
        .base<Anson>()
        .ctor()
        .ctor<string>()
        .ctor<string, string>()
        .data<&anson::AnsonBody::a>("a") // This is what justifying all fields in ast must be regstered.
        ;

    anclass = AnsonBody().anclass;
    AnsonBodyAst *bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->enttypeid = enttype;
    bdast->dataAnclass = anclass;
    bdast->fields = map<string, AnsonField> {
        {"a", {.fieldname="a", .dataAnclass = "string"}}
    };
    asts[anclass] = unique_ptr<AnsonBodyAst>(bdast);

    //
    enttype = hashed_string{UserReq::_type_.c_str()};
    entt::meta_factory<anson::UserReq>()
        .type(enttype)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::UserReq::data>("data");

    anclass = UserReq().anclass;
    bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->enttypeid = enttype;
    bdast->dataAnclass = anclass;
    bdast->fields = map<string, AnsonField> {
        {"data", {.fieldname="data", .dataAnclass = SemanticObject::_anclass_}}
    };
    asts[anclass] = unique_ptr<AnsonBodyAst>(bdast);

    //
    enttype = hashed_string{AnsonResp::_type_.c_str()};
    entt::meta_factory<anson::AnsonResp>()
        .type(enttype)
        .ctor<>()
        .ctor<const std::string&>()
        .base<anson::AnsonBody>()
        .data<&anson::AnsonResp::m>("m")
        .data<&anson::AnsonResp::rs>("rs")
        .data<&anson::AnsonResp::map>("map")
        ;

    anclass = AnsonResp().anclass;
    bdast = new AnsonBodyAst{anclass};
    bdast->base = AnsonAst::_type_;
    bdast->enttypeid = enttype;
    bdast->dataAnclass = anclass;
    bdast->fields = map<string, AnsonField> {
        {"m", {.fieldname="m", .dataAnclass = "string"}},
        {"rs", {.fieldname="rs", .dataAnclass = AnResultset::_anclass_}},
        {"map", {.fieldname="map", .dataAnclass = SemanticObject::_anclass_}}
    };
    asts[anclass] = unique_ptr<AnsonBodyAst>(bdast);
}

inline static void register_port(AstMap &asts, string port_ast) {
    std::ifstream ifstream(port_ast);
    if (!ifstream.is_open()) {
        anerror(string_view(std::format("Could not open the file {}! ", port_ast)));
    }

    AnsonJavaEnumAst *portAst = new AnsonJavaEnumAst{};
    portAst->dataAnclass = Port::_type_;
    portAst->isJavaEnum = true;

    EnTTSaxParser handler(*portAst, IJsonable::contxt_ptr);
    bool result = nlohmann::json::sax_parse(ifstream, &handler);
    if (result) {
        string anclass = portAst->dataAnclass;
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
        anerror(string_view(std::format("Could not load AST from {}!", port_ast)));
}


template<typename T>
inline static void specialize_msg(AstMap &asts, const AnsonBodyAst *body_ast) {
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
    ast->dataBaseAst = AnsonAst::_type_;
    ast->enttypeid = enttype;
    ast->dataAnclass = anclass;

    ast->fields = map<string, AnsonField>{
        {"port", {.fieldname = "port", .dataAnclass=Port::_type_}},
        {"body", {.fieldname = "body", .dataAnclass="list<shared_ptr<"s + T::_type_}}
    };

    // asts[anclass] = unique_ptr<AnsonMsgAst>(ast);
    asts.insert(make_pair(anclass, ast));
}

}
