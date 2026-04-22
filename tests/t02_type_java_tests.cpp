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

    // AnResultset rs2;
    // json = R"({"colnames": {"", [0, ""]})";
    // result = Anson::from_json(json, rs2);
    // anlog(std::format("[2] ok: {}, anclass: {}, rows: {}",
    //                   result, rs2.anclass, rs2.rowCnt));

    // ASSERT_TRUE(result);
    // ASSERT_EQ(AnResultset::_type_, rs2.anclass) << "[2]rs->anclass";
    // ASSERT_EQ(res.mapArr, rs2.colnames) << "[2]rs2->names";
    // ASSERT_EQ(res.mapArr, rs2.rows) << "[2]rs2->rows";
}

// TEST(JUNIT, AnsList3D) { }
