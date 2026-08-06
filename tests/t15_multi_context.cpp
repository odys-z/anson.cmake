#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <io/odysz/common.h>

#include <io/odysz/module/rs.h>
#include "io/odysz/anson.h"
#include "expect/t_10_doctier.hpp"
#include "expect/registry.hpp"

using namespace anson;
// static AstMap asts;
// static JsonOpt contxt{&asts};

TEST(SWITCH_CONTXT, SerializeReqs) {
    AstMap syn_asts;
    JsonOpt syn_opts{&syn_asts};
    register_jserv(syn_asts, syn_opts);
    register_doctier(syn_asts, "ast");

    AstMap reg_asts;
    JsonOpt reg_opts{&reg_asts};
    register_jserv(reg_asts, reg_opts);
    register_centralclientier(reg_asts, "ast");

    PathsPage pths("device", 0, 1);
    set<string> pathpool;
    pths.clientPaths.insert({"path/a", {}});

    //
    AnsonHeader header{};
    header.Act("synclient.cpp", "query", "r/states", "query sync");

    DocsReq req {"h_photos", {}, ""};

    req.uri = "sys/test";
    req.synuri = "syn/test";
    req.syncingPage = pths;
    req.device = Device{pths.device, "synode anclient.cmake test", "Ody@test"};
    req.a = DocsReq::A::selectSyncs;

    req.data.insert({"x", "xxx.xxx.yyy.yyy"});

    AnsonMsg<DocsReq> q{Port{Port::docstier}};
    q.Body(req);
    q.Header(header);

    string json = q.toBlock(syn_opts);
    anlog(json);

    //
    SynodeConfig diction;
    RegistReq reg{JServUrl{"my-jserv-root", {}}, diction};
    reg.a = RegistReq::A::queryDomConfig;

    AnsonMsg<RegistReq> r{Centralport{Centralport::regist}};
    r.Body(reg);
    string kson = r.toBlock(reg_opts);
    anlog(kson);

    //
    AnsonMsg<DocsReq> p{};
    Anson::from_json(json, p, &syn_opts);

    ASSERT_EQ(Port{Port::docstier}, p.port);
    ASSERT_EQ(p.Body().data["x"], LangExt::VarType{"xxx.xxx.yyy.yyy"});
    ASSERT_EQ(p.Body().a, "r/syncflags");
    ASSERT_EQ(p.Body().uri, req.uri);
    ASSERT_EQ(p.Body().synuri, req.synuri);

    //
    AnsonMsg<RegistReq> s{};
    Anson::from_json(kson, s, &reg_opts);

    ASSERT_EQ(Centralport{Centralport::regist}, s.port);
    ASSERT_EQ(RegistReq::A::queryDomConfig, s.Body().a);
    ASSERT_EQ(s.Body().uri, reg.uri);
}
