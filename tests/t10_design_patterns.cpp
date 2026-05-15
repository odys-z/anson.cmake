#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/module/rs.h>
#include "io/odysz/anson.h"
#include "io/odysz/semantier.h"

using namespace anson;
static AstMap asts;
static JsonOpt contxt{&asts};

TEST(DESGIN, FORCE_TYPE) {
    IJsonable::contxt_ptr = &contxt;

    AnResultset rs{};
    ASSERT_EQ(AnResultset::_type_, rs.type);

    Port p{};
    ASSERT_EQ(Port::_type_, p.anclass);

    AnsonBody bd{};
    ASSERT_EQ(AnsonBody::_type_, bd.type);

    EchoReq echo{};
    ASSERT_EQ(EchoReq::_type_, echo.type);
    AnsonMsg<EchoReq> msg{};
    ASSERT_EQ(AnsonMsg<EchoReq>().anclass, msg.anclass);
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, msg.type);
    ASSERT_EQ(AnsonMsg<AnQueryReq>::_type_, msg.type);

    UserReq ur{};
    ASSERT_EQ(UserReq::_type_, ur.type);

    SemanticObject so{};
    ASSERT_EQ(SemanticObject::_type_, so.type);

    SessionInf si{};
    ASSERT_EQ(SessionInf::_type_, si.type);

    AnQueryReq qr{};
    ASSERT_EQ(AnQueryReq::_type_, qr.type);
    AnsonMsg<AnQueryReq> qm{};
    ASSERT_EQ(AnsonMsg<AnQueryReq>().anclass, qm.anclass);
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, qm.type);
    ASSERT_EQ(AnsonMsg<AnQueryReq>::_type_, qm.type);

    AnUpdateReq upr{};
    ASSERT_EQ(AnUpdateReq::_type_, upr.type);
    AnsonMsg<AnUpdateReq> um{};
    ASSERT_EQ(AnsonMsg<AnUpdateReq>().anclass, um.anclass);
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, um.type);
    ASSERT_EQ(AnsonMsg<AnUpdateReq>::_type_, um.type);

    AnInsertReq ir{};
    ASSERT_EQ(AnInsertReq::_type_, ir.type);
    AnsonMsg<AnInsertReq> im{};
    ASSERT_EQ(AnsonMsg<AnInsertReq>().anclass, im.anclass);
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, im.type);
    ASSERT_EQ(AnsonMsg<AnInsertReq>::_type_, im.type);

}
