#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <io/odysz/common.h>
#include <optional>

#include "io/odysz/common.h"
#include "io/odysz/anson.h"
#include "io/odysz/jprotocol.h"
#include "io/odysz/json.h"

using namespace std;
namespace anson {
enum class SampleCodeEnum { ok, restore, exchange, init, exError, _sentinel_ };

using SampleCode = EnEnregistrement<SampleCodeEnum, static_cast<size_t>(SampleCodeEnum::_sentinel_) + 1>;
template<> const string SampleCode::_type_ = "SampleCode.type";

template<>
constexpr std::array<std::string_view, 6> SampleCode::noms = {
    "ok", "restore", "exchange", "init", "exError", "_sentinel_ "};
}

using namespace anson;
static AstMap asts;
static JsonOpt opts{&asts};

TEST(VarAnyTest, VarType) {
    entt::meta_any any_int{42};
    auto result = LangExt::var_any(any_int);

    ASSERT_TRUE(std::holds_alternative<int>(result));
    EXPECT_EQ(std::get<int>(result), 42);

    // double
    entt::meta_any any_double{3.14};
    auto res_d = LangExt::var_any(any_double);
    EXPECT_EQ(std::get<double>(res_d), 3.14);

    // Float
    entt::meta_any any_float{2.5f};
    auto res_f = LangExt::var_any(any_float);
    ASSERT_TRUE(std::holds_alternative<float>(res_f));
    EXPECT_DOUBLE_EQ(std::get<float>(res_f), 2.5);

    // string
    entt::meta_any any_str{std::string("hello")};
    auto res_s = LangExt::var_any(any_str);
    EXPECT_EQ(std::get<std::string>(res_s), "hello");

    std::ostringstream oss;
    LangExt::serialize_var(oss, any_str);
    std::string json_result = std::move(oss).str();
    ASSERT_EQ("hello", json_result);

    // C-style string
    entt::meta_any any_cs{"world"};
    auto res_cs = LangExt::var_any(any_cs);
    ASSERT_TRUE(std::holds_alternative<std::string>(res_cs));
    EXPECT_EQ(std::get<std::string>(res_cs), "world");

    LangExt::serialize_var(oss, any_cs);
    json_result = std::move(oss).str();
    ASSERT_EQ("world", json_result);

    LangExt::serialize_var(oss, res_cs);
    json_result = std::move(oss).str();
    ASSERT_EQ("world", json_result);

    // Time Point
    auto tp = "2026-01-01 10:10:10"_t;

    result = LangExt::var_any(tp);
    LangExt::serialize_var(cout, result) << endl;
    json_result = std::move(oss).str();
    ASSERT_EQ("2026-01-01 10:10:10", json_result);

    ASSERT_TRUE(std::holds_alternative<std::chrono::system_clock::time_point>(result));
    EXPECT_EQ(std::get<std::chrono::system_clock::time_point>(result), tp);

    // Empty/Null State
    entt::meta_any empty_any{};
    result = LangExt::var_any(empty_any);

    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST(JAVA_CPP, TypedEnum) {
    IJsonable::contxt_ptr = &opts;
    register_enums<MsgCode>(asts);
    register_enums<SampleCode>(asts);

    AnsonJavaEnumAst *msgast = static_cast<AnsonJavaEnumAst*>(asts.at(MsgCode::_type_).get());
    AnsonJavaEnumAst *smpast = static_cast<AnsonJavaEnumAst*>(asts.at(SampleCode::_type_).get());

    // msgcode
    meta_any msg_inst = forward_as_meta(MsgCode::Code::ok);
    meta_any msgok_any = msgast->get_field_instance(Anson(), "ok");
    meta_any msgext_any = msgast->get_field_instance(Anson(), "ext");

    ASSERT_EQ(msg_inst.cast<MsgCodeEnum>(), msgok_any.cast<MsgCodeEnum>());
    ASSERT_EQ(msgast->name_of(msgok_any), "ok");
    ASSERT_EQ(msgast->name_of(msgext_any), "ext");

    MsgCodeEnum* msgok_ptr = msgok_any.try_cast<MsgCodeEnum>();
    ASSERT_EQ(MsgCodeEnum::ok, *msgok_ptr);

    EXPECT_EQ("ok", msgast->name_of(*msgok_ptr)) << "string == msg-cod3";

    // sample code
    meta_any smp_inst = forward_as_meta(SampleCode::Code::ok);
    meta_any smpok_any = smpast->get_field_instance(Anson(), "ok");
    meta_any smpexc_any = smpast->get_field_instance(Anson(), "exchange");
    ASSERT_EQ(smp_inst.cast<SampleCodeEnum>(), smpok_any.cast<SampleCodeEnum>());
    ASSERT_EQ(smpast->name_of(smpok_any), "ok");
    ASSERT_EQ(smpast->name_of(smpexc_any), "exchange");

    SampleCodeEnum* smpok_ptr = smpok_any.try_cast<SampleCodeEnum>();
    ASSERT_EQ(SampleCode::Code::ok, *smpok_ptr) << "SampleCode::Code::ok == *smpok_ptr";

    EXPECT_EQ("ok", smpast->name_of(*smpok_ptr)) << "string == sample";
}

TEST(JAVA_CPP, JsonVariant) {
    IJsonable::contxt_ptr = &opts;

    std::optional<string> one = LangExt::var_str("1");
    ASSERT_EQ("1", one) << "'1' == one";

    LangExt::VarType i_two{2};
    ASSERT_EQ(2, std::get<int>(i_two)) << "2 == get<int>(i_two)";
    ASSERT_EQ(3, LangExt::var_int(3)) << "2 == two";

    meta_any var_int = forward_as_meta(LangExt::VarType{1});
    meta_any var_str = forward_as_meta(LangExt::VarType{"bla"});

    vector<LangExt::VarType> varr{LangExt::VarType{1}, LangExt::VarType{"bla"}};
    vector<LangExt::VarType> vbrr{{1}, {"bla"}};
    ASSERT_EQ((vbrr), (varr)) << "varr";
}
