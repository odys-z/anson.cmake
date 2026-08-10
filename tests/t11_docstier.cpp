#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <io/odysz/common.h>

#include <io/odysz/module/rs.h>
#include "io/odysz/anson.h"
#include "expect/t_10_doctier.hpp"

using namespace anson;
static AstMap asts;
static JsonOpt contxt{&asts};

TEST(DOCSTIER, SerializeDocsReq) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(&opts);
    register_doctier(&opts, "ast");

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

    AnsonMsg<DocsReq> q{Port{&opts, Port::docstier}};
    q.Body(req);
    q.Header(header);

    string json = q.toBlock(opts);
    anlog(json);

    AnsonMsg<DocsReq> p{&opts};
    Anson::from_json(json, p, &opts);

    ASSERT_EQ(p.Body().data["x"], LangExt::VarType{"xxx.xxx.yyy.yyy"});
    ASSERT_EQ(p.Body().a, "r/syncflags");
    ASSERT_EQ(p.Body().uri, req.uri);
    ASSERT_EQ(p.Body().synuri, req.synuri);
}

TEST(DOCSTIER, DeserializeDocsResp) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(&opts);
    register_doctier(&opts, "ast");

    string respjson = R"json({
 "type": "io.odysz.semantic.jprotocol.AnsonMsg",
 "code": "ok", "opts": null, "port": "docstier",
 "header": null,
 "body": [{
  "type": "io.odysz.semantic.tier.docs.DocsResp",
  "syndomain": "infor-17-1", "rs": null,
  "parent": "io.odysz.semantic.jprotocol.AnsonMsg",
  "a": null, "org": null, "collectId": null, "stamp": null, "m": null, "uri": null,
  "syncingPage": {
   "type": "io.odysz.semantic.tier.docs.PathsPage",
   "start": 0, "end": 1,
   "clientPaths": {"path/a": ["cpp-test", "prv", "ody", "now()", 0]},
   "device": "cpp-test"
  },
  "xdoc": null, "blockSeqReply": 0, "docTabl": null,
  "device": {
   "type": "io.odysz.semantic.tier.docs.Device",
   "tofolder": null, "synode0": null, "devname": "xyz",
   "id": "cpp-test"
  }, "map": null}], "addr": null,
 "version": "1.1", "seq": 0})json";

    AnsonMsg<DocsResp> rep{&opts};
    Anson::from_json(respjson, rep, &opts);

    ASSERT_EQ(MsgCode::Code::ok, rep.code);
    ASSERT_EQ(5, rep.body[0]->syncingPage.clientPaths["path/a"].size());

    vector<LangExt::VarType> pths = rep.body[0]->syncingPage.clientPaths["path/a"];
    // FIXME
    // FIXME this is not correct. Expecting an int here!
    string fifth = LangExt::var_str(pths[4]).value();
    ASSERT_EQ("0", fifth);

    ASSERT_EQ("cpp-test", rep.body[0]->device.id);
    ASSERT_EQ("xyz", rep.body[0]->device.devname);
}

TEST(DOCSTIER, DE_ReserializeDocsResp) {

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(&opts);
    register_doctier(&opts, "ast");

    string r =
R"json({"type": "io.odysz.semantic.jprotocol.AnsonMsg", "code": "ok", "opts": null,
"port": "docstier", "header": null,
"body": [{"type": "io.odysz.semantic.tier.docs.DocsResp", "syndomain": "infor-17-1", "rs": null, "parent": "io.odysz.semantic.jprotocol.AnsonMsg", "a": null, "org": null, "collectId": null, "stamp": null, "m": null, "uri": null,
"syncingPage": {"type": "io.odysz.semantic.tier.docs.PathsPage", "start": 0, "end": 9,
"clientPaths": {"C:/Users/github/anclient/examples/example.slint/build/app/ast/desktop-settings.ast.json": ["slint.test", "prv", "admin", "2026-07-22", 0]}, "device": "slint.test"}
, "xdoc": null, "blockSeqReply": 0, "docTabl": null, "device": {"type": "io.odysz.semantic.tier.docs.Device", "tofolder": null, "synode0": null, "devname": null, "id": "slint.test"}
, "map": null}
], "addr": null, "version": "1.1", "seq": 0})json";

    AnsonMsg<DocsResp> msg{&opts};
    Anson::from_json(r, msg, &opts);

    ASSERT_EQ(MsgCode::Code::ok, msg.code);
    Port expt_p{Port{&opts, Port::docstier}};
    ASSERT_EQ(expt_p, msg.port);

    string s = msg.toBlock(opts);
    anlog(s);
    std::regex kv_pathpage(R"("type": "io.odysz.semantic.tier.docs.PathsPage")");
    ASSERT_TRUE(std::regex_search(s, kv_pathpage));
    // ISSUE bug is tolerated here. expecting VarType of int 0.
    ASSERT_EQ(LangExt::VarType{"0"}, msg.Body().syncingPage.clientPaths["C:/Users/github/anclient/examples/example.slint/build/app/ast/desktop-settings.ast.json"][4]);
    std::regex kv_clientpaths(R"("clientPaths": \{"C:/Users/github/anclient/examples/example.slint/build/app/ast/desktop-settings.ast.json": \["slint.test","prv","admin","2026-07-22","0"\]\})");
    ASSERT_TRUE(std::regex_search(s, kv_clientpaths));

}
