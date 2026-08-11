#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include "io/odysz/module/langstring.h"
#include "io/odysz/reflect.h"

using namespace anson;
static AstMap asts;
static JsonOpt contxt{&asts};

TEST(T12_Translate, en) {
    register_jserv(&contxt);
    register_langstringAst(&contxt);

    LangString::loadLangs(&contxt, "settings/strings.json");
    ASSERT_EQ("English", langs.en["eng"]);
    ASSERT_EQ("English", "eng"_ans);
    ASSERT_EQ("Chinese", langs.en["chn"]);
    ASSERT_EQ("Chinese", "chn"_ans);
    ASSERT_EQ("jp", "jp"_ans);

    LangString::loadLangs(&contxt, "settings/strings-zh.json");
    ASSERT_EQ("English", langs.en["eng"]);
    ASSERT_EQ("英语", langs.local["eng"]);
    ASSERT_EQ("英语", "eng"_ans);

    ASSERT_EQ("Chinese", langs.en["chn"]);
    ASSERT_EQ("中文", langs.local["chn"]);
    ASSERT_EQ("中文", "chn"_ans);
    ASSERT_EQ("jp", "jp"_ans);
}
