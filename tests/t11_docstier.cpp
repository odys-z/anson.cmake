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

    AnsonMsg<DocsReq> q{Port{Port::docstier}};
    q.Body(req);
    q.Header(header);

    string json = q.toBlock();
    anlog(json);

    AnsonMsg<DocsReq> p{};
    Anson::from_json(json, p);

    ASSERT_EQ(p.Body().data["x"], LangExt::VarType{"xxx.xxx.yyy.yyy"});
    ASSERT_EQ(p.Body().a, "r/syncflags");
    ASSERT_EQ(p.Body().uri, req.uri);
    ASSERT_EQ(p.Body().synuri, req.synuri);
}

TEST(DOCSTIER, DeserializeDocsResp) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, "./");

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

    AnsonMsg<DocsResp> rep{};
    Anson::from_json(respjson, rep);

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

TEST(DOCSTIER, Escape_paths) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, "./");

    map<string, vector<LangExt::VarType>> fileselection;
    fileselection.emplace(R"(C:\Users\github\anclient\examples\example.slint\build\app\libboost_url-gcc16-mt-d-x64-1_91.dll)",
                          vector<LangExt::VarType>{"syncing"});
    fileselection.emplace(R"(C:\Users\github\anclient\examples\example.slint\build\app\libcpr.dll)",
                          vector<LangExt::VarType>{"syncing"});

    PathsPage syncingpage;
    syncingpage.clientPaths = fileselection;
    syncingpage.start = 0;
    syncingpage.end = fileselection.size();

    // in slint: reconnect_ipc();
    // wsclient->place_tasks(syncingpage);
    DocsReq uploadreq{"h_photos", {}};
    // in slint: uploadreq.syncingPage.end = pthpage.clientPaths.size();
    // in slint: uploadreq.syncingPage.start = 0;
    uploadreq.syncingPage = syncingpage;
    uploadreq.a = DocsReq::A::requestSyn;

    uploadreq.pageInf.page = 0;
    uploadreq.pageInf.size = 2; // syncingpage.clientPaths.size();
    uploadreq.pageInf.total = 2; // syncingpage.clientPaths.size();
    uploadreq.blockSeq = -1;
    uploadreq.doc.size = 0;

    AnsonMsg<DocsReq> msg(Port{Port::docstier}, std::move(uploadreq));
    msg.seq = 0;
    // in slint: asynSend(msg);

    string expjson = R"json({"type": "io.odysz.semantic.jprotocol.AnsonMsg",)json"
    R"json("body": [{"type": "io.odysz.semantic.tier.docs.DocsReq",)json"
    R"json("data": {},"a": "u/syn","uri": "","blockSeq": -1,"deletings": [],)json"
    R"json("device": {"type": "io.odysz.semantic.tier.docs.Device","devname": "","id": "","synode0": "","toFolder": ""},)json"
    R"json("doc": {"type": "io.odysz.semantic.tier.docs.ExpSyncDoc",)json"
      R"json("entm": null,"recId": "","synode": "","synoder": "","uids": "","clientpath": "","createDate": "",)json"
      R"json("device": "","folder": "","mime": "","org": "","pname": "","shareMsg": "","shareby": "","sharedate": "",)json"
      R"json("shareflag": "","size": 0,"uri64": ""},)json"
    R"json("docTabl": "","limit": 0,"org": "",)json"
    R"json("pageInf": {"type": "io.odysz.transact.sql.PageInf","arrCondts": [],"mapCondts": {},"page": 0,"size": 2,"total": 2},"reset": true,"stamp": "","syncQueries": [],)json"
    R"json("syncingPage": {"type": "io.odysz.semantic.tier.docs.PathsPage",)json"
      R"json("clientPaths": {"C:\\Users\\github\\anclient\\examples\\example.slint\\build\\app\\libboost_url-gcc16-mt-d-x64-1_91.dll": ["syncing"],)json"
      R"json("C:\\Users\\github\\anclient\\examples\\example.slint\\build\\app\\libcpr.dll": ["syncing"]},)json"
    R"json("device": "","end": 2,"start": 0},"synuri": ""}],)json"
    R"json("code": "null","header": {"type": "io.odysz.semantic.jprotocol.AnsonHeader","iv64": "","ssToken": "","ssid": "","uid": "","usrAct": []},)json"
    R"json("port": "docstier","seq": 0,"version": ""})json";

    try {
        msg.toBlock();
        FAIL() << "Expecting invalid escaping check failed.";
    } catch (AnsonException e) {
        anlog("That's what expected");
    }
}

/*
TEST(DOCSTIER, Escape_paths) {

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, "./");

    map<string, vector<LangExt::VarType>> fileselection;
    fileselection.emplace(R"(C:\Users\github\anclient\examples\example.slint\build\app\libboost_url-gcc16-mt-d-x64-1_91.dll)",
                          vector<LangExt::VarType>{"syncing"});
    fileselection.emplace(R"(C:\Users\github\anclient\examples\example.slint\build\app\libcpr.dll)",
                          vector<LangExt::VarType>{"syncing"});

    PathsPage syncingpage;
    syncingpage.clientPaths = fileselection;
    syncingpage.start = 0;
    syncingpage.end = fileselection.size();

    // in slint: reconnect_ipc();
    // wsclient->place_tasks(syncingpage);
    DocsReq uploadreq{"h_photos", {}};
    // in slint: uploadreq.syncingPage.end = pthpage.clientPaths.size();
    // in slint: uploadreq.syncingPage.start = 0;
    uploadreq.syncingPage = syncingpage;
    uploadreq.a = DocsReq::A::requestSyn;

    uploadreq.pageInf.page = 0;
    uploadreq.pageInf.size = 2; // syncingpage.clientPaths.size();
    uploadreq.pageInf.total = 2; // syncingpage.clientPaths.size();
    uploadreq.blockSeq = -1;
    uploadreq.doc.size = 0;

    AnsonMsg<DocsReq> msg(Port{Port::docstier}, std::move(uploadreq));
    msg.seq = 0;
    // in slint: asynSend(msg);

    string expjson = R"json({"type": "io.odysz.semantic.jprotocol.AnsonMsg",)json"
    R"json("body": [{"type": "io.odysz.semantic.tier.docs.DocsReq",)json"
    R"json("data": {},"a": "u/syn","uri": "","blockSeq": -1,"deletings": [],)json"
    R"json("device": {"type": "io.odysz.semantic.tier.docs.Device","devname": "","id": "","synode0": "","toFolder": ""},)json"
    R"json("doc": {"type": "io.odysz.semantic.tier.docs.ExpSyncDoc",)json"
      R"json("entm": null,"recId": "","synode": "","synoder": "","uids": "","clientpath": "","createDate": "",)json"
      R"json("device": "","folder": "","mime": "","org": "","pname": "","shareMsg": "","shareby": "","sharedate": "",)json"
      R"json("shareflag": "","size": 0,"uri64": ""},)json"
    R"json("docTabl": "","limit": 0,"org": "",)json"
    R"json("pageInf": {"type": "io.odysz.transact.sql.PageInf","arrCondts": [],"mapCondts": {},"page": 0,"size": 2,"total": 2},"reset": true,"stamp": "","syncQueries": [],)json"
    R"json("syncingPage": {"type": "io.odysz.semantic.tier.docs.PathsPage",)json"
      R"json("clientPaths": {"C:\\Users\\github\\anclient\\examples\\example.slint\\build\\app\\libboost_url-gcc16-mt-d-x64-1_91.dll": ["syncing"],)json"
      R"json("C:\\Users\\github\\anclient\\examples\\example.slint\\build\\app\\libcpr.dll": ["syncing"]},)json"
    R"json("device": "","end": 2,"start": 0},"synuri": ""}],)json"
    R"json("code": "null","header": {"type": "io.odysz.semantic.jprotocol.AnsonHeader","iv64": "","ssToken": "","ssid": "","uid": "","usrAct": []},)json"
    R"json("port": "docstier","seq": 0,"version": ""})json";

    ASSERT_EQ(expjson, msg.toBlock());
}
*/
