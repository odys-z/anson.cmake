#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include "io/odysz/anson.h"
#include "io/odysz/json.h"
#include "t02_entt_basic.h"

TEST(ENTT, EXT_ASTS) {
    using namespace entt::literals;
    using namespace anson;
    AstMap asts;
    JsonOpt opts(&asts);

    ASSERT_TRUE(opts.is_ast(AnsonAst::_type_));
    ASSERT_TRUE(opts.is_ast(AnsonJavaEnumAst::_type_));
    ASSERT_TRUE(opts.is_ast(AnsonBodyAst::_type_));
    ASSERT_TRUE(opts.is_ast(AnsonMsgAst::_type_));
    ASSERT_FALSE(opts.is_ast(Anson::_type_));
}

TEST(ENTT, T_LIST_GENERIC_SEQUENCE) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    anson::register_asts(asts);
    anson::register_T_List(asts);

    auto type = entt::resolve<anson::T_List>();
    entt::meta_any instance = type.construct();
    T_List &tlist = instance.cast<T_List&>();
    auto data_member = type.data("val"_hs);

    entt::meta_any val_any = data_member.get(instance);

    andebug(tlist.val);

    auto view = val_any.as_sequence_container();

    if (view) {
        entt::meta_any newValue = std::string("Generic Insert");
        view.insert(view.end(), std::move(newValue));
        // https://github.com/skypjack/entt/issues/1332#event-23995788785
        andebug(string_view(std::format("#1332 {}, {}", view.size(), tlist.val.size())));
        EXPECT_EQ(1, view.size());
        EXPECT_EQ(1, tlist.val.size());
    }

    // Verification
    auto& final_vec = instance.cast<anson::T_List&>().val;
    EXPECT_EQ(1, view.size());
    EXPECT_EQ(0, final_vec.size());
    vector<string>& val = final_vec;

    vector<string> vec{"New Insert"};
    data_member.set(instance, vec);

    EXPECT_EQ("New Insert", val[0]);

    meta_any inst = forward_as_meta(tlist);
    vec.push_back("A");
    data_member.set(inst, vec);
    EXPECT_EQ("A", tlist.val[1]);

    T_List stub;
    T_List &stub_ref = stub; // works with or without this replacement
    meta_any inst2 = forward_as_meta(stub_ref);
    vec.push_back("A1");
    data_member.set(inst2, vec);
    EXPECT_EQ(3, stub.val.size());
    EXPECT_EQ("A1", stub.val[2]);

    // case 1
    EnTTSaxParser handler(stub_ref, IJsonable::contxt_ptr);
    vec.push_back("B");
    data_member.set(handler.stack.back().instance, vec);
    EXPECT_EQ(4, stub.val.size());
    EXPECT_EQ("B", stub_ref.val[3]);
    EXPECT_EQ("B", stub.val[3]);

    // case 2
    T_List stub2;
    T_List &stub2_ref = stub2;
    EnTTSaxParser handler2_ref(stub2_ref, IJsonable::contxt_ptr);
    vec.push_back("C");
    data_member.set(handler2_ref.stack.back().instance, vec);
    EXPECT_EQ(5, vec.size());
    EXPECT_EQ(5, stub2.val.size());
    EXPECT_EQ("C", stub2_ref.val[4]);

    // Case 3
    T_List stub3;
    EnTTSaxParser handler3(stub3, IJsonable::contxt_ptr);
    vec.push_back("D");
    data_member.set(handler3.stack.back().instance, vec);
    EXPECT_EQ(6, vec.size());
    EXPECT_EQ(6, stub3.val.size());
    EXPECT_EQ("D", stub3.val[5]);
}

TEST(ENTT, T_LIST_PARSE) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts);
    register_T_List(asts);

    T_List inst_list;
    T_List &anlist = inst_list;

    std::string json_input = std::format(R"({{"type": "{}", "txt": "text", "val": ["a", "b"]}})", T_List::_type_);

    EnTTSaxParser handler_parse(anlist, IJsonable::contxt_ptr);

    T_List* to_checkList0 = handler_parse.stack.back().instance.try_cast<anson::T_List>();

    cout << "[0] " << json_input << endl;
    anlist.type = "";
    bool result = nlohmann::json::sax_parse(json_input, &handler_parse);

    ASSERT_TRUE(result);
    ASSERT_EQ(T_List::_type_, anlist.anclass) << "anlist.anclass";
    ASSERT_EQ(T_List::_type_, anlist.type) << "anlist.type";

    T_List* stack_ptr = handler_parse.stack.front().instance.try_cast<T_List>();
    andebug(string_view(std::format("Anlist addr: {:P}, Stack Root addr: {:P} : {:P}",
                                    (void*)&anlist, (void*)to_checkList0, (void*)stack_ptr)));

    T_List* to_checkList1 = handler_parse.stack.back().instance.try_cast<anson::T_List>();

    ASSERT_EQ("text", to_checkList1->txt) << "to_check_list.txt";
    ASSERT_EQ((vector<string>{"a", "b"}), to_checkList1->val) << "to_check_list.val";
    ASSERT_EQ("text", anlist.txt) << "anlist.txt";
    ASSERT_EQ((vector<string>{"a", "b"}), anlist.val) << "anlist.val";
}

TEST(ENTT, T_LIST_2D) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts);
    register_T_List(asts);
    register_2Dasts(asts);

    T_List2D inst_list;
    T_List2D &anlist = inst_list;

    std::string json_input = std::format(R"({{"type": "{}", "vss": [{{"txt": "x", "val": ["0"]}}] }})",
                                         T_List2D::_type_);

    EnTTSaxParser handler_parse(anlist, IJsonable::contxt_ptr);

    cout << "[0] " << json_input << endl;
    anlist.type = "";
    bool result = nlohmann::json::sax_parse(json_input, &handler_parse);

    ASSERT_TRUE(result);
    ASSERT_EQ(T_List2D::_type_, anlist.anclass) << "anlist.anclass";
    ASSERT_EQ(T_List2D::_type_, anlist.type) << "anlist.type";

    T_List2D* to_checkList = handler_parse.stack.back().instance.try_cast<anson::T_List2D>();

    ASSERT_EQ(1, to_checkList->vss.size()) << "to_check_list.vss.size";
    ASSERT_EQ((vector<string>{"0"}), to_checkList->vss[0].val) << "to_check_list[0].val";
    ASSERT_EQ("x", anlist.vss[0].txt) << "vss[0].txt";
    ASSERT_EQ(1, anlist.vss[0].val.size()) << "vss[0].val.size";
    ASSERT_EQ((vector<string>{"0"}), anlist.vss[0].val) << "vss[0].val";
}

TEST(ENTT, T_LIST_2D_Ptr) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts);
    register_T_List(asts);
    register_2DPtr_asts_callback(asts);

    T_List2DPtr inst_list;
    T_List2DPtr &anlist = inst_list;

    std::string json_input = std::format(R"({{"type": "{}", "vpp": [{{"txt": "y", "val": ["cccc ----"]}}] }})",
                                         T_List2DPtr::_type_);

    EnTTSaxParser handler_parse(anlist, IJsonable::contxt_ptr);

    cout << "[0] " << json_input << endl;
    anlist.type = "";
    bool result = nlohmann::json::sax_parse(json_input, &handler_parse);

    ASSERT_TRUE(result);
    ASSERT_EQ(T_List2DPtr::_type_, anlist.anclass) << "anlist.anclass";
    ASSERT_EQ(T_List2DPtr::_type_, anlist.type) << "anlist.type";


    T_List2DPtr* to_checkList = handler_parse.stack.back().instance.try_cast<anson::T_List2DPtr>();

    ASSERT_EQ(1, to_checkList->vpp.size()) << "to_check_list.vss.size";
    ASSERT_EQ((vector<string>{"cccc ----"}), to_checkList->vpp[0]->val) << "to_check_list_pp[0]->val";
    ASSERT_EQ("y", anlist.vpp[0]->txt) << "vpp[0]->txt";
    ASSERT_EQ(1, anlist.vpp[0]->val.size()) << "vpp[0]->val->size";
    ASSERT_EQ((vector<string>{"cccc ----"}), anlist.vpp[0]->val) << "vpp[0]->val";
}
