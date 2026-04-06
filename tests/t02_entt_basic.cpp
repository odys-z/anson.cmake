#include <gtest/gtest.h>
#include <entt/meta/meta.hpp>
#include <entt/meta/factory.hpp>
#include <nlohmann/json.hpp>
#include "io/odysz/anson.h"


namespace anson {

class T_List : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.T_List";

    vector<string> val;

    string txt;

    T_List() : Anson(_type_) {}
};

class T_List2D : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.T_List2D";

    vector<T_List> vss;

    T_List2D() : Anson(_type_) {}
};

class T_List2DPtr : public Anson {
public:
    inline static const string _type_ = "io.odysz.anson.T_List2DPtr";

    vector<shared_ptr<T_List>> vpp;

    T_List2DPtr() : Anson(_type_) {}
};

void register_asts(AstMap &asts) {
    hashed_string enttype;
    string anclass;
    //
    enttype = hashed_string{"io.odysz.anson.IJasonable"};
    entt::meta_factory<anson::IJsonable>()
        .type(enttype)
        .data<&anson::IJsonable::anclass>("anclass")
        ;

    //
    enttype = hashed_string{Anson::_type_.c_str()};
    entt::meta_factory<anson::Anson>()
        .type(enttype)
        .base<IJsonable>()
        .ctor<>()
        .ctor<const std::string&>()
        .data<&anson::Anson::type>("type")
        ;

    //
    enttype = hashed_string{T_List::_type_.c_str()};
    entt::meta_factory<anson::T_List>()
        .type(enttype)
        .base<Anson>()
        .ctor<>()
        .data<&anson::T_List::val>("val")
        .data<&anson::T_List::txt>("txt")
        ;

    anclass = T_List().anclass;
    AnsonAst *ast = new AnsonAst{anclass, false};
    ast->dataAnclass = T_List::_type_;
    ast->base = Anson::_type_;
    ast->enttypeid = enttype;

    // ast.fields is only used for serialization?
    ast->fields = map<string, AnsonField>{
                 {"txt",   {.fieldname="txt", .dataAnclass = "string"}},
                 {"val",   {.fieldname="val", .dataAnclass = "list<string"}}
                 };

    asts[anclass] = unique_ptr<AnsonAst>(ast);
}

void register_2Dasts(AstMap &asts) {
    //
    entt::meta_factory<anson::T_List2D>()
        .type(T_List2D::_type_.c_str())
        .base<Anson>()
        .ctor<>()
        .data<&anson::T_List2D::vss>("vss")
        ;

    string anclass = T_List2D().anclass;
    AnsonAst *ast = new AnsonAst{anclass, false};
    ast->dataAnclass = T_List2D::_type_;
    ast->dataBaseAst = AnsonAst::_type_;
    ast->base = Anson::_type_;
    ast->enttypeid = hashed_string{T_List2D::_type_.c_str()};

    ast->fields = map<string, AnsonField>{
        {"vss",   {.fieldname="vss", .dataAnclass = "list<"s + T_List::_type_}},
    };

    asts[anclass] = unique_ptr<AnsonAst>(ast);
}

void register_2DPtr_asts(AstMap &asts) {
    //
    entt::meta_factory<anson::T_List2DPtr>()
        .type(T_List2DPtr::_type_.c_str())
        .base<Anson>()
        .ctor<>()
        .data<&anson::T_List2DPtr::vpp>("vpp")
        ;

    string anclass = T_List2DPtr().anclass;
    AnsonAst *ast = new AnsonAst{anclass, false};
    ast->dataAnclass = T_List2DPtr::_type_;
    ast->dataBaseAst = AnsonAst::_type_;
    ast->base = Anson::_type_;
    ast->enttypeid = hashed_string{T_List2DPtr::_type_.c_str()};

    ast->fields = map<string, AnsonField>{
      {"vpp",   {.fieldname="vpp", .dataAnclass = "list<shared_ptr<"s + T_List::_type_}},
    };

    asts[anclass] = unique_ptr<AnsonAst>(ast);
}
}

/*
TEST(ENTT, T_LIST_GENERIC_SEQUENCE) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    // map<string, meta_type> enttypes;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    anson::register_asts(asts);

    auto type = entt::resolve<anson::T_List>();
    entt::meta_any instance = type.construct();
    T_List &tlist = instance.cast<T_List&>();
    auto data_member = type.data("val"_hs);

    entt::meta_any val_any = data_member.get(instance);

    andebug(tlist.val);

    auto view = val_any.as_sequence_container();

    if (view) {
        entt::meta_any newValue = std::string("Generic Insert");

        // This now modifies the vector inside 'instance'
        view.insert(view.end(), std::move(newValue));
        andebug(string_view(std::format("{}, {}", view.size(), tlist.val.size())));
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

    T_List inst_list;
    T_List &anlist = inst_list;

    std::string json_input = std::format(R"({{"type": "{}", "txt": "text", "val": ["a", "b"]}})", T_List::_type_);
    // or switch the order
    // std::string json_input = std::format(R"({{"type": "{}", "val": ["a", "b"], "txt": "text"}})", T_List::_type_);

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
*/

TEST(ENTT, T_LIST_2D_Ptr) {
    using namespace entt::literals;
    using namespace anson;

    AstMap asts;
    JsonOpt contxt{&asts};
    IJsonable::contxt_ptr = &contxt;
    register_asts(asts);
    register_2DPtr_asts(asts);

    T_List2DPtr inst_list;
    T_List2DPtr &anlist = inst_list;

    std::string json_input = std::format(R"({{"type": "{}", "vpp": [{{"txt": "y", "val": ["1"]}}] }})",
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
    ASSERT_EQ((vector<string>{"0"}), to_checkList->vpp[0]->val) << "to_check_list_pp[0]->val";
    ASSERT_EQ("x", anlist.vpp[0]->txt) << "vpp[0]->txt";
    ASSERT_EQ(1, anlist.vpp[0]->val.size()) << "vpp[0]->val->size";
    ASSERT_EQ((vector<string>{"0"}), anlist.vpp[0]->val) << "vpp[0]->val";
}
