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

TEST(ESCAPE, Invalid_payload) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, "ast");

    map<string, vector<LangExt::VarType>> fileselection;
    fileselection.emplace(R"(C:\Users\github\anclient\examples\example.slint\build\app\libboost_url-gcc16-mt-d-x64-1_91.dll)",
                          vector<LangExt::VarType>{"syncing"});
    fileselection.emplace(R"(C:\Users\github\anclient\examples\example.slint\build\app\libcpr.dll)",
                          vector<LangExt::VarType>{"syncing"});

    PathsPage syncingpage;
    syncingpage.clientPaths = fileselection;
    syncingpage.start = 0;
    syncingpage.end = fileselection.size();

    DocsReq uploadreq{"h_photos", {}};
    uploadreq.syncingPage = syncingpage;
    uploadreq.a = DocsReq::A::requestSyn;

    uploadreq.pageInf.page = 0;
    uploadreq.pageInf.size = 2;
    uploadreq.pageInf.total = 2;
    uploadreq.blockSeq = -1;
    uploadreq.doc.size = 0;
    uploadreq.limit = 0;
    uploadreq.reset = true;

    AnsonMsg<DocsReq> msg(Port{Port::docstier}, std::move(uploadreq));
    msg.seq = 0;

    string invalid_json = R"json({"type": "io.odysz.semantic.jprotocol.AnsonMsg",)json"
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
      R"json("clientPaths": {"C:\Users\github\anclient\examples\example.slint\build\app\libboost_url-gcc16-mt-d-x64-1_91.dll": ["syncing"],)json"
      R"json("C:\Users\github\anclient\examples\example.slint\build\app\libcpr.dll": ["syncing"]},)json"
    R"json("device": "","end": 2,"start": 0},"synuri": ""}],)json"
    R"json("code": "_sentinel_","header": {"type": "io.odysz.semantic.jprotocol.AnsonHeader","iv64": "","ssToken": "","ssid": "","uid": "","usrAct": []},)json"
    R"json("port": "docstier","seq": 0,"version": ""})json";

    ASSERT_EQ(invalid_json, msg.toBlock());

    AnsonMsg<DocsReq> msg2;
    Anson::from_json(invalid_json, msg2);
    ASSERT_EQ(0, msg2.body.size()) << "Expecting invalid escaping check failed.";

}

TEST(ESCAPE, Valid_payload) {

    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, "ast");

    map<string, vector<LangExt::VarType>> fileselection;
    fileselection.emplace(IJsonable::escape(R"(C:\Users\github\anclient\examples\example.slint\build\app\libboost_url-gcc16-mt-d-x64-1_91.dll)", opts),
                          vector<LangExt::VarType>{"syncing"});
    fileselection.emplace(IJsonable::escape(R"(C:\Users\github\anclient\examples\example.slint\build\app\libcpr.dll)", opts),
                          vector<LangExt::VarType>{"syncing"});

    PathsPage syncingpage;
    syncingpage.clientPaths = fileselection;
    syncingpage.start = 0;
    syncingpage.end = fileselection.size();

    DocsReq uploadreq{"h_photos", {}};
    uploadreq.syncingPage = syncingpage;
    uploadreq.a = DocsReq::A::requestSyn;

    uploadreq.pageInf.page = 0;
    uploadreq.pageInf.size = 2;
    uploadreq.pageInf.total = 2;
    uploadreq.blockSeq = -1;
    uploadreq.doc.size = 0;
    uploadreq.limit = 0;
    uploadreq.reset = true;

    AnsonMsg<DocsReq> msg(Port{Port::docstier}, std::move(uploadreq));
    msg.seq = 0;

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
    R"json("code": "_sentinel_","header": {"type": "io.odysz.semantic.jprotocol.AnsonHeader","iv64": "","ssToken": "","ssid": "","uid": "","usrAct": []},)json"
    R"json("port": "docstier","seq": 0,"version": ""})json";

    ASSERT_EQ(expjson, msg.toBlock());
}

TEST(ESCAPE, POSIX_path) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, "ast");

    map<string, vector<LangExt::VarType>> fileselection;
    fileselection.emplace(Anson::posix_path(R"(C:\Users\github\anclient\examples\example.slint\build\app\libboost_url-gcc16-mt-d-x64-1_91.dll)"),
                          vector<LangExt::VarType>{"syncing"});
    fileselection.emplace(Anson::posix_path(R"(C:\Users\github\anclient\examples\example.slint\build\app\libcpr.dll)"),
                          vector<LangExt::VarType>{"syncing"});

    PathsPage syncingpage;
    syncingpage.clientPaths = fileselection;
    syncingpage.start = 0;
    syncingpage.end = fileselection.size();

    DocsReq uploadreq{"h_photos", {}};
    uploadreq.syncingPage = syncingpage;
    uploadreq.a = DocsReq::A::requestSyn;

    uploadreq.pageInf.page = 0;
    uploadreq.pageInf.size = 2;
    uploadreq.pageInf.total = 2;
    uploadreq.blockSeq = -1;
    uploadreq.doc.size = 0;
    uploadreq.limit = 0;
    uploadreq.reset = true;

    AnsonMsg<DocsReq> msg(Port{Port::docstier}, std::move(uploadreq));
    msg.seq = 0;

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
      R"json("clientPaths": {"C:/Users/github/anclient/examples/example.slint/build/app/libboost_url-gcc16-mt-d-x64-1_91.dll": ["syncing"],)json"
      R"json("C:/Users/github/anclient/examples/example.slint/build/app/libcpr.dll": ["syncing"]},)json"
    R"json("device": "","end": 2,"start": 0},"synuri": ""}],)json"
    R"json("code": "_sentinel_","header": {"type": "io.odysz.semantic.jprotocol.AnsonHeader","iv64": "","ssToken": "","ssid": "","uid": "","usrAct": []},)json"
    R"json("port": "docstier","seq": 0,"version": ""})json";

    ASSERT_EQ(expjson, msg.toBlock());
}
