#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>

#include <io/odysz/module/rs.h>
#include "io/odysz/anson.h"
#include "io/odysz/semantier.h"
#include "expect/t_10_doctier.hpp"

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

void DocsReq::format(const IFileDescriptor& p) {}

void DocsReq::format(const IFileDescriptor & p, const string uri) {}

void ExpSyncDoc::format(const AnResultset& rs) {}

TEST(Generator, Enforce_Design) {
    IJsonable::contxt_ptr = &contxt;

    PageInf pi{};
    ASSERT_EQ(PageInf::_type_, pi.type);

    Device dev{};
    ASSERT_EQ(Device::_type_, dev.type);

    SynEntity se{};
    ASSERT_EQ(SynEntity::_type_, se.type);

    SynEntity se2{SynEntityMeta{}};
    ASSERT_EQ(SynEntity::_type_, se2.type);

    PathsPage pp{};
    ASSERT_EQ(PathsPage::_type_, pp.type);

    PathsPage pp2{"device", 1, 11};
    ASSERT_EQ(PathsPage::_type_, pp2.type);

    ExpSyncDoc sd{};
    ASSERT_EQ(ExpSyncDoc::_type_, sd.type);

    DocsReq dr{"fake", {"test-docsreq.file"}};
    ASSERT_EQ(DocsReq::_type_, dr.type);
    AnsonMsg<DocsReq> dm{};
    andebug(dm.anclass);
    ASSERT_EQ(AnsonMsg<DocsReq>().anclass, dm.anclass);
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, dm.type);
    ASSERT_EQ(AnsonMsg<DocsReq>::_type_, dm.type);

    DocsReq dr2{"h_photos", sd, "uri"};
    ASSERT_EQ(DocsReq::_type_, dr2.type);
    AnsonMsg<DocsReq> dm2{};
    andebug(dm2.anclass);
    ASSERT_EQ(AnsonMsg<DocsReq>().anclass, dm2.anclass);
    ASSERT_EQ(AnsonMsg<EchoReq>::_type_, dm2.type);
    ASSERT_EQ(dm.type, dm2.type);

    DocsResp drp{};
    ASSERT_EQ(DocsResp::_type_, drp.type);
    AnsonMsg<DocsResp> rpm{};
    andebug(rpm.anclass);
    ASSERT_EQ(AnsonMsg<DocsResp>().anclass, rpm.anclass);
    ASSERT_EQ(AnsonMsg<AnsonResp>::_type_, rpm.type);
    ASSERT_EQ(dm.type , rpm.type);
    ASSERT_EQ(dm2.type, rpm.type);
}

TEST(Generator, Gen_Doctier) {
    AstMap asts;
    JsonOpt opts{&asts};
    register_jserv(asts, opts);
    register_doctier(asts, ".");

    PageInf pi{};
    bool res = Anson::from_json(
        R"({"type": ")" + PageInf::_type_ + R"(",")"
      + R"("page": 1, "size": 20, "total": 42, "arrCondts": [["n0": "v0"], ["n1": v1"]]})",
        pi);
}
