#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include "io/odysz/anson.h"

using namespace std;
namespace anson {

using AstMap = map<string, unique_ptr<AnsonAst>>;

template <typename E, size_t N>
struct EnEnregistrement {
    using EnumTaper = E;
    static const string _type_;
    static constexpr size_t compter = N;
    static const std::array<std::string_view, N> noms;
};

// 1. Define the actual enum
enum class MsgCodeEnum { ok, exSession, exSemantic, exIo, exTransct, exDA, exGeneral, ext, _sentinel_ };
enum class SampleCodeEnum { ok, restore, exchange, init, exError, _sentinel_ };

// 2. Wrap it in our template structure
using MsgCode = EnEnregistrement<MsgCodeEnum, static_cast<size_t>(MsgCodeEnum::_sentinel_) + 1>;
template<> const string MsgCode::_type_ = "MsgCode.type";

using SampleCode = EnEnregistrement<SampleCodeEnum, static_cast<size_t>(SampleCodeEnum::_sentinel_) + 1>;
template<> const string SampleCode::_type_ = "SampleCode.type";

// 3. Initialize the names (Global or static)
template<>
constexpr std::array<std::string_view, 9> MsgCode::noms = {
    "ok", "exSession", "exSemantic", "exIo", "exTransct", "exDA", "exGeneral", "ext", "_sentinal_"
};

template<>
constexpr std::array<std::string_view, 6> SampleCode::noms = {
    "ok", "restore", "exchange", "init", "exError", "_sentinel_ "};


template<typename C> // C is now an EnumRegistry type
inline static void register_enums(AstMap& asts) {
    using EType = typename C::EnumTaper; // Get the actual enum type (e.g., MsgCodeEnum)

    AnsonJavaEnumAst *ast = new AnsonJavaEnumAst(C::_type_, true);
    // ... same initialization code ...

    ast->get_field_instance = [ast](const IJsonable &j, const string &fieldname) -> meta_any {
        // Access names directly from the template class C
        for (size_t ix = 0; ix < C::compter; ix++) {
            if (C::noms[ix] == fieldname) {
                return { static_cast<EType>(ix) };
            }
        }
        return { static_cast<EType>(C::compter) }; // Return sentinel
    };

    ast->name_of = [ast](const meta_any val_inst) -> string {
        if (auto* val = val_inst.try_cast<EType>()) {
            size_t idx = static_cast<size_t>(*val);
            if (idx < C::compter) {
                return string(C::noms[idx]);
            }
        }
        return "null";
    };

    asts[C::_type_] = unique_ptr<AnsonJavaEnumAst>(ast);
}
}


using namespace anson;
AstMap asts;
JsonOpt opts{&asts};

TEST(TRY, TypedEnum) {
    IJsonable::contxt_ptr = &opts;
    register_enums<MsgCode>(asts);
    register_enums<SampleCode>(asts);

    AnsonJavaEnumAst *msgast = static_cast<AnsonJavaEnumAst*>(asts.at(MsgCode::_type_).get());
    AnsonJavaEnumAst *smpast = static_cast<AnsonJavaEnumAst*>(asts.at(SampleCode::_type_).get());

    meta_any msg_inst = forward_as_meta(MsgCode::EnumTaper::ok);
    meta_any msgok_any = msgast->get_field_instance(Anson(), "ok");
    meta_any msgext_any = msgast->get_field_instance(Anson(), "ext");

    ASSERT_EQ(msg_inst.cast<MsgCodeEnum>(), msgok_any.cast<MsgCodeEnum>());
    ASSERT_EQ(msgast->name_of(msgok_any), "ok");
    ASSERT_EQ(msgast->name_of(msgext_any), "ext");

    MsgCodeEnum* msgok_ptr = msgok_any.try_cast<MsgCodeEnum>();
    ASSERT_EQ(MsgCodeEnum::ok, *msgok_ptr);

    meta_any smp_inst = forward_as_meta(SampleCode::EnumTaper::ok);
    meta_any smpok_any = smpast->get_field_instance(Anson(), "ok");
    meta_any smpexc_any = smpast->get_field_instance(Anson(), "exchange");
    ASSERT_EQ(smp_inst.cast<SampleCodeEnum>(), smpok_any.cast<SampleCodeEnum>());
    ASSERT_EQ(smpast->name_of(smpok_any), "ok");
    ASSERT_EQ(smpast->name_of(smpexc_any), "exchange");

    SampleCodeEnum* smpok_ptr = smpok_any.try_cast<SampleCodeEnum>();
    ASSERT_EQ(SampleCode::EnumTaper::ok, *smpok_ptr);
}
