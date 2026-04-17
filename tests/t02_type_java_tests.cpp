#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <optional>

#include "io/odysz/common.h"
#include "io/odysz/anson.h"
#include "io/odysz/json.h"
#include "t02_type_java_tests.h"

using namespace anson;
static AstMap asts;
static JsonOpt opts{&asts};

TEST(JUNIT, AnsTMap) {
    IJsonable::contxt_ptr = &opts;
    register_asts(asts);
    register_AnsTMap(asts);
    anlog(to_aststring(asts), PrintFormat{.sep="\n"});

    map<string, vector<LangExt::VarType>> maparr {
        {"X", {"00", "01"}},
        {"Y", {"10", "11"}}
    };

    AnsTMap tmap{{{"u", "v"}}, maparr};

    string json = tmap.toBlock(opts);
    // {"type": "io.odysz.anson.AnsTMap","_map": {"u": "v"},"mapArr": {"X": ,"Y": },"mapArr2": {}}
    anlog("------------------------------------ "s + json);

    AnResultset rs;
    bool result = Anson::from_json(json, rs);
    anlog(std::format("[2] ok: {}, anclass: {}, rows: {}",
                      result, rs.anclass, rs.rowCnt));

    ASSERT_TRUE(result);
    ASSERT_EQ(AnResultset::_type_, rs.anclass) << "[2]rs->anclass";
}

TEST(JUNIT, AnsList3D) {
}
