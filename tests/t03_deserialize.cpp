#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <io/odysz/jprotocol.h>
#include <io/odysz/json.h>
#include <io/odysz/anserializer.h>


using namespace std;
using json = nlohmann::json;
using namespace anson;
using namespace entt;

map<string, map<string, int>*> enums;

TEST(Anson, Base) {
    register_meta(enums);

    auto an_type = entt::resolve("Anson"_hs);
    meta_any anptr = an_type.construct();
    Anson& anobj = anptr.cast<anson::Anson&>();
    cout << "Actual Type Name: " << anobj.type << endl;
    ASSERT_EQ("", anobj.type);

    anptr = an_type.construct(Anson::_type_);
    Anson& anobj2 = anptr.cast<anson::Anson&>();
    ASSERT_EQ(Anson::_type_, anobj2.type) << "1. anobj2.type";

    std::string json_input = R"({"type": "input"})";
    EnTTSaxParser<Anson>  handler(anobj2);

    cout << "[2] " << json_input << endl;
    bool result = nlohmann::json::sax_parse(json_input, &handler);

    ASSERT_TRUE(result);
    ASSERT_EQ("input", anobj2.type) << "Errors on parssing {type: input}.";
}

TEST(Anson, AnsonBody) {

    auto b = entt::resolve("AnsonBody"_hs);
    auto v = b.construct(std::string("r/ds"));
    AnsonBody* anb = v.try_cast<AnsonBody>();
    cout << "[1] anb.type: " << anb->type << endl;
    ASSERT_EQ(AnsonBody::_type_, anb->type);
    ASSERT_EQ("r/ds", anb->a) << "[1.0]";

    std::string json_input = R"({"type": "input", "a": "r/query"})";
    EnTTSaxParser<AnsonBody> handler(*anb);

    cout << "[2] " << json_input << endl;
    bool result = nlohmann::json::sax_parse(json_input, &handler);
    cout << "[3] ok: " << result << ", type: " << anb->type << ", a: " << anb->a << endl;
    ASSERT_TRUE(result);
    ASSERT_EQ("input", anb->type) << "[3.1]";
    ASSERT_EQ("r/query", anb->a) << "[3.2]";

    AnsonBody anc;
    result = Anson::from_json<AnsonBody>(json_input, anc);
    ASSERT_TRUE(result);
    ASSERT_EQ("input", anc.type) << "[3.3]";
    ASSERT_EQ("r/query", anc.a) << "[3.4]";
}

TEST(Anson, AnsonMsg) {
    using Req = AnsonMsg<EchoReq>;

    auto mt = entt::resolve("AnsonMsgEchoReq"_hs);

    auto mv = mt.construct(Port(Port::echo));
    AnsonMsg<EchoReq>* msg = mv.try_cast<AnsonMsg<EchoReq>>();
    EchoReq body{"Hello"};
    msg->Body(body);

    cout << "[1] msg.port: " << msg->port << endl;
    ASSERT_EQ("io.odysz.jprotocol.AnsonMsg", msg->type);
    ASSERT_EQ("echo", msg->port);
    ASSERT_EQ("r/query", msg->body.at(0)->a);
    ASSERT_EQ("Hello", msg->body.at(0)->echo);

    std::string json_input = R"({"type": "input", "port": "query", "body": []})";

    EnTTSaxParser<AnsonMsg<EchoReq>> handler(*msg);

    cout << "[2] " << json_input << endl;
    bool result = nlohmann::json::sax_parse(json_input, &handler);
    cout << "[3] ok: " << result << ", type: " << msg->type << ", port: " << msg->port << endl;

    ASSERT_TRUE(result);
    ASSERT_EQ("input", msg->type);
    EXPECT_EQ("query", msg->port) << "TODO: ever breaked at operator overloads?";

    EchoReq& reqbd = msg->Body();

    cout << "[4] body: " << msg->body.size() << ", type: " << reqbd.type << ", a: " << reqbd.a << endl;
    EXPECT_EQ("io.odysz.jprotocol.EchoReq", reqbd.type) << "TODO: No way to check type?";
    EXPECT_EQ("r/query", reqbd.a) << "[4: a = r/query]";
}

TEST(Anson, Servialize_Msg) {

    using Req = AnsonMsg<EchoReq>;
    auto msg = std::make_shared<Req>(Port::query);
    msg->body.push_back(std::make_shared<EchoReq>("Hello World"));

    std::ostringstream oss;

    serialize_recursive(msg, enums, oss);

    // Why move()? Gemini: instead of copying, stringstream is allowed to transfer
    // ownership of its internal memory directly to the returned std::string.
    std::string json_result = std::move(oss).str();

    std::cout << "Serialized JSON: " << json_result << std::endl;
    ASSERT_EQ(R"({"type": "io.odysz.jprotocol.AnsonMsg", "port": "query", )"
              R"("body": [{"a": "r/query", "echo": "Hello World"}]})",
              json_result);
}
