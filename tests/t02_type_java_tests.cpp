#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include "io/odysz/common.h"
#include "io/odysz/anson.h"
#include "io/odysz/json.h"
#include "t02_type_java_tests.h"

using namespace anson;
static AstMap asts;
static JsonOpt opts{&asts};

TEST(JUNIT, AnsTMap) {
    IJsonable::contxt_ptr = &opts;
    register_varctors();
    register_asts(asts);
    register_AnsTMap(asts);
    anlog(to_aststring(asts), PrintFormat{.sep="\n"});

    map<string, vector<LangExt::VarType>> maparr {
        {"X", {"00", "01"}},
        {"Y", {"10", "11"}}
    };

    AnsTMap tmap{{{"u", "v"}}, maparr};

    string json = tmap.toBlock(opts);
    // {"type": "io.odysz.anson.AnsTMap","_map": {"u": "v"},"mapArr": {"X": ["00","01"],"Y": ["10","11"]},"mapArr2": {}}
    anlog("------------------------------------ "s + json);

    AnsTMap res;
    bool result = Anson::from_json(json, res);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsTMap::_type_, res.anclass) << "[2]res->anclass";
    ASSERT_EQ(2, res.mapArr.size());
    ASSERT_EQ(2, res.mapArr["X"].size());
    ASSERT_EQ(2, res.mapArr["Y"].size());
    ASSERT_EQ(LangExt::VarType{"00"}, res.mapArr["X"][0]) << "mapArr 0:0";
    ASSERT_EQ(LangExt::VarType{"01"}, res.mapArr["X"][1]) << "mapArr 0:1";
    ASSERT_EQ(LangExt::VarType{"10"}, res.mapArr["Y"][0]) << "mapArr 1:0";
    ASSERT_EQ(LangExt::VarType{"11"}, res.mapArr["Y"][1]) << "mapArr 1:1";
}

TEST(JUNIT, AnsList3D) {

    register_varctors();
    register_AnsTStrsList(asts);

    vector<LangExt::VarType>
        r0{"x0", "y0"},
        r1{"x1", "y1"},
        r2{"x2", "y2"};

    AnsTStrsList anst("1d-00", r0, r1, r2);
    string json = anst.toBlock(opts);
    // {"type": "io.odysz.anson.AnsTStrsList","dim4": [],"lst": [["1d-00"]],
    //  "lst3d": [[["x0","y0"],["x1","y1"],["x2","y2"]],[["x2","y2"],["x1","y1"],["x0","y0"]]]}
    anlog("------------------------------------ "s + json);

    AnsTStrsList res;
    bool result = Anson::from_json(json, res);
    ASSERT_TRUE(result);
    ASSERT_EQ(AnsTStrsList::_type_, res.anclass) << "AnsTstrsList->anclass";
    ASSERT_EQ(1, res.lst.size());
    ASSERT_EQ(2, res.lst3d.size());
    ASSERT_EQ(3, res.lst3d[0].size());
    ASSERT_EQ(2, res.lst3d[0][0].size());
    ASSERT_EQ((vector<LangExt::VarType>{"x0", "y0"}), res.lst3d[0][0]) << "3d[0:0";
    ASSERT_EQ((vector<LangExt::VarType>{"x1", "y1"}), res.lst3d[0][1]) << "3d[0:1";
    ASSERT_EQ((vector<LangExt::VarType>{"x2", "y2"}), res.lst3d[0][2]) << "3d[0:2";
    ASSERT_EQ((vector<LangExt::VarType>{"x0", "y0"}), res.lst3d[1][2]) << "3d[0:2";
}

TEST(JUNIT, NestedList) {
    register_jserv(asts, opts);

    string jblock = R"({"type":"io.odysz.semantic.jprotocol.test.U.AnInsertReq",)"
                    R"("nvs":[["type""type","type","io.oz.album.tier.PhotoRec"],["css","{\"type\":\"io.odysz.semantic.T_PhotoCSS\", \"size\":[4,3,3,4]}"]],)"
                    R"("nvss": [[["0-0-0.0 0-0-0.1","v0-0-1.0,v0-0-1.1","0-0-2"],["0-1-0"]]])"
                    R"(})";

    AnInsertReq req;
    bool result = Anson::from_json(jblock, req);
    ASSERT_TRUE(result);
    ASSERT_EQ("\"type\"\"type\"", LangExt::var_str(req.nvs[0][0]));
    ASSERT_EQ("\"type\"", LangExt::var_str(req.nvs[0][1]));

    ASSERT_EQ("0-0-0.0 0-0-0.1", LangExt::var_str(req.nvss[0][0][0]));
    ASSERT_EQ("0-0-2", LangExt::var_str(req.nvss[0][0][2]));
    ASSERT_EQ("0-1-0", LangExt::var_str(req.nvss[0][1][0]));
}

TEST(JUNIT, NoSql) {
    string jblock = "{\"type\":\"io.odysz.semantic.jprotocol.test.U.AnInsertReq\","s
                    + "\"nvs\":[[\"type\"\"type\",\"type\",\"io.oz.album.tier.PhotoRec\"],[\"css\",\"{\\\"type\\\":\\\"io.odysz.semantic.T_PhotoCSS\\\", \\\"size\\\":[4,3,3,4]}\"]],"
                    + "\"nvss\": [[[\"0-0-0.0,0-0-0.1\",\"0-0-1\"],[\"0-1-0\"]]]"
                    + "}";

    AnInsertReq req;
    bool result = Anson::from_json(jblock, req);
    ASSERT_TRUE(result);

    T_PhotoCSS css;
    result = Anson::from_json(LangExt::var_str(req.nvs[1][1]).value(), css);
    ASSERT_TRUE(result);
    ASSERT_EQ (4, css.size[0]);
}
