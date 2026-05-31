#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/module/rs.h>
#include "io/odysz/anson.h"
#include "io/odysz/semantier.h"
#include "expect/t_10_doctier.hpp"

using namespace anson;
static AstMap asts;
static JsonOpt contxt{&asts};

// void DocsReq::format(const IFileDescriptor& p) {}

// void DocsReq::format(const IFileDescriptor & p, const string uri) {}

// void ExpSyncDoc::format(const AnResultset& rs) {}


TEST(DOCSTIER, SerializeDocsReq) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, "./");

    PathsPage pths("device", 0, 1);
    set<string> pathpool;
    pths.clientPaths.insert({"path/a", {}}); // what's the querying flag?

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

    AnsonMsg<DocsReq> q{Port::docstier};
    q.Body(req);
    q.Header(header);

    string json = q.toBlock();
    anlog(json);

    // DocsReq p{"", {}};
    AnsonMsg<DocsReq> p{};
    Anson::from_json(json, p);

    ASSERT_EQ(p.Body().data["x"], LangExt::VarType{"xxx.xxx.yyy.yyy"});
    ASSERT_EQ(p.Body().a, "r/syncflags");
    ASSERT_EQ(p.Body().uri, req.uri);
    ASSERT_EQ(p.Body().synuri, req.synuri);
}
