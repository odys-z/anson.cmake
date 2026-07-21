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
    auto result = LangExt::any2var(any_int);

    ASSERT_TRUE(std::holds_alternative<int>(result));
    EXPECT_EQ(std::get<int>(result), 42);

    // double
    entt::meta_any any_double{3.14};
    auto res_d = LangExt::any2var(any_double);
    EXPECT_EQ(std::get<double>(res_d), 3.14);

    // Float
    entt::meta_any any_float{2.5f};
    auto res_f = LangExt::any2var(any_float);
    ASSERT_TRUE(std::holds_alternative<float>(res_f));
    EXPECT_DOUBLE_EQ(std::get<float>(res_f), 2.5);

    // string
    entt::meta_any any_str{std::string("hello")};
    auto res_s = LangExt::any2var(any_str);
    EXPECT_EQ(std::get<std::string>(res_s), "hello");

    std::ostringstream oss;
    LangExt::serialize_var(oss, any_str, opts);
    std::string json_result = std::move(oss).str();
    ASSERT_EQ(R"("hello")", json_result);

    // C-style string
    entt::meta_any any_cs{"world"};
    LangExt::VarType var_cs = LangExt::any2var(any_cs);
    ASSERT_TRUE(std::holds_alternative<std::string>(var_cs));
    EXPECT_EQ(std::get<std::string>(var_cs), "world");

    LangExt::serialize_var(oss, any_cs, opts);
    json_result = oss.str();
    ASSERT_EQ(R"("world")", json_result);

    // Time Point
    chrono::system_clock::time_point tp = "2026-01-01 10:10:10"_t;
    LangExt::VarType tp_var = LangExt::any2var(meta_any{tp});
    EXPECT_EQ(std::get<std::chrono::system_clock::time_point>(tp_var), tp);

    std::ostringstream os2;
    LangExt::serialize_var(os2, meta_any{tp}, opts);
    json_result = std::move(os2.str());
    ASSERT_EQ("2026-01-01 10:10:10", json_result);

    ASSERT_TRUE(std::holds_alternative<std::chrono::system_clock::time_point>(tp_var));

    // Empty/Null State
    entt::meta_any empty_any{};
    result = LangExt::any2var(empty_any);

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

    map<string, vector<LangExt::VarType>> maparr {
        {"X", {"00", "01"}},
        {"Y", {1.0, 1.1}},
        {"Z", {20, 21}}
    };

    meta_any anzlst = meta_any{maparr["Z"]};
    std::ostringstream osz;
    if (auto view = anzlst.as_sequence_container(); view) {
        bool first = true;
        for (const auto &e : view) {
            if (first) first = false; else osz << ',';
            if (meta_any _e = *e; _e)
                LangExt::serialize_var(osz, _e, opts);
            else
                LangExt::serialize_var(osz, e, opts);
        }
        string json_result = std::move(osz.str());
        cout << json_result << endl;
        ASSERT_EQ("20,21", json_result);
    }
    else FAIL() << "Cannot create sequence view Z.";

    meta_any anyxlst = meta_any{maparr["X"]};
    std::ostringstream osx;
    if (auto view = anyxlst.as_sequence_container(); view) {
        bool first = true;
        for (const auto &e : view) {
            if (first) first = false; else osx << ',';
            if (meta_any _e = *e; _e)
                LangExt::serialize_var(osx, _e, opts);
            else
                LangExt::serialize_var(osx, e, opts);
        }
        string json_result = std::move(osx.str());
        cout << json_result << endl;
        ASSERT_EQ(R"("00","01")", json_result);
    }
    else FAIL() << "Cannot create sequence view X.";

    meta_any anylst = meta_any{maparr["Y"]};
    std::ostringstream osy;
    if (auto view = anylst.as_sequence_container(); view) {
        bool first = true;
        for (const auto &e : view) {
            if (first) first = false; else osy << ',';
            if (meta_any _e = *e; _e)
                LangExt::serialize_var(osy, _e, opts);
            else
                LangExt::serialize_var(osy, e, opts);
        }
        string json_result = std::move(osy.str());
        cout << json_result << endl;
        ASSERT_EQ("1.0,1.1", json_result);
    }
    else FAIL() << "Cannot create sequence view Y.";

}

TEST(JAVA_CPP, LIST_MODIFY) {
    meta_any anylst = meta_any{vector<LangExt::VarType>{}};

    if (auto view = anylst.as_sequence_container(); view) {
        view.insert(view.end(), meta_any{LangExt::VarType{"A"}});
        view.insert(view.end(), LangExt::VarType{"B"});
        view.insert(view.end(), LangExt::VarType{3.1415926});

        vector<LangExt::VarType> *varlist = anylst.try_cast<vector<LangExt::VarType>>();
        ASSERT_EQ(3, view.size()) << "view.size";
        ASSERT_EQ(3, varlist->size()) << "varlist.size";
        ASSERT_EQ("A", LangExt::var_str((*varlist)[0]));
        ASSERT_EQ("B", LangExt::var_str((*varlist)[1]));
        ASSERT_EQ(3.1415926, LangExt::var_double((*varlist)[2]));
    }
    else FAIL() << "Cannot create sequence view Y.";
}
