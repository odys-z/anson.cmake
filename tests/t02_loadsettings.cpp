#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

#include <io/odysz/semantier.h>
#include "io/odysz/jprotocol.h"

using json = nlohmann::json;
using namespace anson;

void register_settings(map<string, AnsonAst> &asts, map<string, meta_type> &enttypes) {
    hashed_string enttype;
    //
    enttype = hashed_string{Anson::_type_.c_str()};
    entt::meta_factory<anson::Anson>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        ;

    AnsonAst ast = asts[Anson::_type_] = AnsonAst{Anson::_type_, false};
    ast.base = "io.odysz.anson.IJsonable";
    ast.enttypeid = enttype;

    //
    enttype = hashed_string{PeerSettings::_type_.c_str()};
    entt::meta_factory<anson::PeerSettings>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .data<&anson::PeerSettings::ansons>("ansons"_hs, "ansons")
        .data<&anson::PeerSettings::scopeEnums>("scopeEnums"_hs, "scopeEnums")
        .data<&anson::PeerSettings::javaEnums>("javaEnums"_hs, "javaEnums")
        .data<&anson::PeerSettings::ansonMsg>("ansonMsg"_hs, "ansonMsg")
        .data<&anson::PeerSettings::ansonBody>("ansonBody"_hs, "ansonBody")
        .data<&anson::PeerSettings::anRequests>("anRequests"_hs, "anRequests");
        ;

    ast = asts[PeerSettings::_type_] = AnsonAst{AnsonAst().anclass, false};
    ast.antype = PeerSettings::_type_;
    ast.base = Anson::_type_;
    ast.enttypeid = enttype;
}

TEST(PeerSettings, Load) {
    aninfo(string_view(filesystem::current_path().string()));

    map<string, AnsonAst> asts;
    map<string, meta_type> enttypes;
    register_settings(asts, enttypes);
    JsonOpt contxt{&asts, &enttypes};


    PeerSettings settings;
    std::string json_input = std::format(R"({{"type": "{}"}})", PeerSettings::_type_);
    EnTTSaxParser handler(settings, contxt);

    cout << "[0] " << json_input << endl;
    bool result = nlohmann::json::sax_parse(json_input, &handler);

    ASSERT_TRUE(result);
    ASSERT_EQ(PeerSettings::_type_, settings.anclass) << "Errors on parssing {type: input}.";


    string t02_json = "t02-settings.json";
    std::ifstream ifstream(t02_json);
    if (!ifstream.is_open()) {
        FAIL() << "Could not open the file! " << t02_json << endl;
    }
    result = nlohmann::json::sax_parse(ifstream, &handler);
    ASSERT_TRUE(result);
    ASSERT_EQ(PeerSettings::_type_, settings.anclass) << "Errors on parssing settings.json.";
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, settings.ansonMsg) << "expecting " << AnsonMsg<EchoReq>::_type_;
    ASSERT_EQ(AnsonBody::_type_, settings.ansonBody) << "expecting " << AnsonBody::_type_;
}
