#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

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

TEST(TRY, TypedEnum) {
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
