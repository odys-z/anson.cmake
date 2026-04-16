#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"
#include <io/odysz/module/rs.h>


using namespace anson;
using namespace std;

static AstMap asts;

static JsonOpt contxt{&asts};

TEST(AnResultset, StringVal) {

    map<string, AnResultset::Column> cols {
      {"X", {0, "x"}},
      {"Y", {1, "y"}}
    };
    vector<string>
        r0{"x0", "y0"},
        r1{"x1", "y1"},
        r2{"x2", "y2"};

    AnResultset rstr{cols, r0, r1, r2};

    ASSERT_EQ(2, rstr.colCnt);
    ASSERT_EQ(3, rstr.rowCnt);

    rstr.beforeFirst().next();
    EXPECT_EQ("x0", rstr.getString("x"));
    EXPECT_EQ("y0", rstr.getString("y"));
    rstr.next();
    EXPECT_EQ("x1", rstr.getString("x"));
    EXPECT_EQ("y1", rstr.getString("y"));
    rstr.next();
    EXPECT_EQ("x2", rstr.getString("x"));
    EXPECT_EQ("y2", rstr.getString("y"));

    ASSERT_FALSE(rstr.next());
    EXPECT_EQ(std::nullopt, rstr.getString("y"));
}

TEST(AnResultset, IntVal) {
    map<string, AnResultset::Column> cols {
      {"M", {0, "m"}},
      {"N", {1, "n"}},
      {"O", {2, "o"}},
      {"P", {3, "p"}}
    };

    vector<int>
        r0{00, 01},
        r1{10, 11},
        r2{20, 21, 23, 24};

    AnResultset intrs{cols, r0, r1, r2};

    intrs.beforeFirst().next();
    EXPECT_EQ(0, intrs.getInt("m"));
    EXPECT_EQ(1, intrs.getInt("n"));

    EXPECT_EQ(std::nullopt, intrs.getString("m"));
    EXPECT_EQ(std::nullopt, intrs.getString("n"));

    while(intrs.next())
        ;

    ASSERT_FALSE(intrs.next());
    EXPECT_EQ(std::nullopt, intrs.getString("n"));
}

TEST(AnResultset, Serialize_Deserialize) {
    register_jserv(asts, contxt);
    anlog(to_aststring(asts), PrintFormat{.sep="\n"});

    map<string, AnResultset::Column> cols {
        {"X", {0, "x"}},
        {"Y", {1, "y"}}
    };

    vector<string>
        r0{"x0", "y0"},
        r1{"x1", "y1"},
        r2{"x2", "y2"};

    AnResultset rstr{cols, r0, r1, r2};

    string json = rstr.toBlock(contxt);
    anlog(json);

    AnResultset rs;
    bool result = Anson::from_json(json, rs);
    anlog(std::format("[2] ok: {}, anclass: {}, rows: {}",
                      result, rs.anclass, rs.rowCnt));

    ASSERT_TRUE(result);
    ASSERT_EQ(AnResultset::_type_, rs.anclass) << "[2]rs->anclass";
}
